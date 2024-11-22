/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "UnityWrapper.h"
#include "Data/UnityNDKInteropData.h"




 // !!! Fill out this options !!!
constexpr std::string_view g_SceneId = "workspaces/unitytest/scenes/transform";
constexpr std::string_view g_Base64 = "";
constexpr std::string_view g_ApiKey = "2og83K3KNs8ImVaoL93pVNPc0XzKk9XV";
constexpr std::string_view g_ApiSecret = "5aRjDzBGPf5PTzd25rHFiazxIyqZ88n1qLjJlQV2ZQA31nmx62QcQta9q24R8GT7";

void NDKApp::NDKWrapper::Init()
{
	_Options.ServerUrl = "api-engine.inworld.ai:443";
	_Options.UserConfig.Name = "Player";

	_Options.Base64 = g_Base64;
	_Options.ApiKey = g_ApiKey;
	_Options.ApiSecret = g_ApiSecret;
	//_Options.GameSessionId = "ndk-test-game-session";

	_Options.Capabilities.Animations = false;
	_Options.Capabilities.Audio = false;
	_Options.Capabilities.Emotions = true;
	_Options.Capabilities.Interruptions = true;
	_Options.Capabilities.EmotionStreaming = true;
	_Options.Capabilities.SilenceEvents = true;
	_Options.Capabilities.PhonemeInfo = true;
	_Options.Capabilities.NarratedActions = true;
	_Options.Capabilities.MultiAgent = true;

	std::vector<Inworld::AgentInfo> AgentInfos;

	_Client.Client().InitClientAsync(
		{},
		[this](Inworld::Client::ConnectionState ConnectionState)
		{
			std::string Error;
			int32_t Code;
			Inworld::ErrorDetails Details;
			_Client.Client().GetConnectionError(Error, Code, Details);

			std::string State;
			switch (ConnectionState)
			{
			case Inworld::Client::ConnectionState::Connected:
				State = "Connected";
				break;
			case Inworld::Client::ConnectionState::Disconnected:
				State = "Disconnected";
				break;
			case Inworld::Client::ConnectionState::Failed:
				State = "Failed";
				break;
			case Inworld::Client::ConnectionState::Idle:
				State = "Idle";
				break;
			case Inworld::Client::ConnectionState::Paused:
				State = "Paused";
				break;
			case Inworld::Client::ConnectionState::Reconnecting:
				State = "Reconnecting";
				break;
			case Inworld::Client::ConnectionState::Connecting:
				State = "Connecting";
				break;
			default:
				State = "Unknown";
				break;
			}

			Inworld::Log("Connection state: %s. %s", State.c_str(), Error.empty() ? "" : (std::string(" Error: ") + Error).c_str());

			if (ConnectionState == Inworld::Client::ConnectionState::Disconnected)
			{
				_Client.Client().ResumeClient();
			}
		},
		[this](std::shared_ptr<Inworld::Packet> Packet)
		{
			Packet->Accept(*this);
		}
	);

#if INWORLD_VAD
	Inworld::ClientSpeechOptions_VAD_DetectAndFilterAudio ClientSpeechOptions;
	ClientSpeechOptions.VADModelPath = std::filesystem::canonical("Package/resource/silero_vad_10_27_2022.onnx").string();
#else
	Inworld::ClientSpeechOptions_Default ClientSpeechOptions;
#endif
	_Client.Client().InitSpeechProcessor(ClientSpeechOptions);

	_Client.Client().SetPerceivedLatencyTrackerCallback([](const std::string& InteractonId, int32_t Latency)
		{
			//Inworld::Log("PerceivedLatencyTracker. Latency is '%d', Interaction: %s", Latency, InteractonId));
		});

	_Client.Client().SetOptions(_Options);
	std::string SceneId = std::string{ g_SceneId };
	_Client.Client().StartClientFromSceneId(SceneId);
	_Client.Client().EnableAudioDump();
}

#pragma region Logger
void NDKApp::NDKWrapper::SetLoggerCallBack(const Inworld::LogCallback& info, const Inworld::LogCallback& warning, const Inworld::LogCallback& error)
{
	m_LogInfo = info;
	m_LogWarning = warning;
	m_LogError = error;
	if (info != nullptr && warning != nullptr && error != nullptr)
		Inworld::SetLogCallbacks(info, warning, error);
	Inworld::Log("Unity NDK Logger Initialized");
}

#pragma endregion Logger

#pragma region Parent Stuff (Everything except SendAudio Data)
void NDKApp::NDKWrapper::Error(const std::string& Msg)
{
	Inworld::LogError("%s", Msg.c_str());
}
void NDKApp::NDKWrapper::SendText(std::string text)
{
	Inworld::Routing R;
	if (!GetRouting(R))
	{
		return;
	}
	_Client.Client().SendTextMessage(R, text);
}
void NDKApp::NDKWrapper::NextCharacter()
{
	if (_AgentInfos.empty())
	{
		Error("Agents are empty");
		return;
	}

	_CurrentAgentIdx = (_CurrentAgentIdx + 1) % _AgentInfos.size();
	_CurrentConversationIdx = -1;

	NotifyCurrentCharacter();
}

void NDKApp::NDKWrapper::PrevCharacter()
{
	if (_AgentInfos.empty())
	{
		Error("Agents are empty");
		return;
	}

	_CurrentAgentIdx = (_CurrentAgentIdx - 1 + _AgentInfos.size()) % _AgentInfos.size();
	_CurrentConversationIdx = -1;

	NotifyCurrentCharacter();
}

void NDKApp::NDKWrapper::SetCharacter(int32_t Idx)
{
	_CurrentAgentIdx = Idx;
	_CurrentConversationIdx = -1;
	NotifyCurrentCharacter();
}

void NDKApp::NDKWrapper::NotifyCurrentCharacter()
{
	auto& Info = _AgentInfos[_CurrentAgentIdx];
	Inworld::Log("Current character: %d %s %s", _CurrentAgentIdx, Info.GivenName.c_str(), Info.AgentId.c_str());
}

void NDKApp::NDKWrapper::SendAudioData()
{
	//std::string Data;
	//Inworld::Routing R;
	//if (GetRouting(R) && Inworld::Mic::GetAudio(Data))
	//{
	//	_Client.Client().SendSoundMessage(R, Data);
	//}

	//_LastAudioSentTime = std::chrono::steady_clock::now();
}

std::string NDKApp::NDKWrapper::GetTargetStr(const Inworld::Routing& Routing)
{
	std::string Target = Routing._Target._Name;
	if (Target.empty())
	{
		for (const auto& C : _Conversations)
		{
			if (C.Id != Routing._ConversationId)
			{
				continue;
			}

			for (const auto& AgentId : C.Agents)
			{
				Target += GetGivenName(AgentId);
				Target += " ";
			}
		}
	}
	return Target;
}

void NDKApp::NDKWrapper::Visit(const Inworld::TextEvent& Event)
{
	if (Event.IsFinal())
		Inworld::Log("%s to %s: Text: %s", GetGivenName(Event._Routing._Source._Name).c_str(), GetTargetStr(Event._Routing).c_str(), Event.GetText().c_str());
}

void NDKApp::NDKWrapper::Visit(const Inworld::CustomEvent& Event)
{
	Inworld::Log("%s: Custom: %s", GetGivenName(Event._Routing._Source._Name).c_str(), Event.GetName().c_str());
}

void NDKApp::NDKWrapper::Visit(const Inworld::AudioDataEvent& Event)
{
	Inworld::Log("%s: Audio: %d", GetGivenName(Event._Routing._Source._Name).c_str(), Event.GetDataChunk().size());
}

void NDKApp::NDKWrapper::Visit(const Inworld::ControlEventCurrentSceneStatus& Event)
{
	_AgentInfos = Event.GetAgentInfos();
	if (!_AgentInfos.empty())
	{
		_CurrentAgentIdx = 0;
		NotifyCurrentCharacter();
	}
	//_Client.TaskExec.Push([this, AgentInfos = Event.GetAgentInfos()]()
	//	{
	//		_AgentInfos = AgentInfos;
	//		if (!AgentInfos.empty())
	//		{
	//			_CurrentAgentIdx = 0;
	//			NotifyCurrentCharacter();
	//		}
	//	});
}

void NDKApp::NDKWrapper::Visit(const Inworld::ControlEventConversationUpdate& Event)
{
	_CurrentConversationIdx = -1;
	_CurrentAgentIdx = -1;

	Conversation Conv;
	Conv.Id = Event._Routing._ConversationId;
	Conv.Agents = Event.GetAgents();
	for (int32_t i = 0; i < _Conversations.size(); ++i)
	{
		auto& C = _Conversations[i];
		if (C.Id == Event._Routing._ConversationId)
		{
			C = Conv;
			_CurrentConversationIdx = i;
			break;
		}
	}

	if (_CurrentConversationIdx == -1)
	{
		_Conversations.push_back({ Conv.Id, Event.GetAgents() });
		_CurrentConversationIdx = _Conversations.size() - 1;
	}

	const std::string Type = Event.GetType() == 1 ? "STARTED" : Event.GetType() == 2 ? "UPDATED" : "EVICTED";
	std::string Agents;
	for (const auto& A : Conv.Agents)
	{
		Agents += GetGivenName(A);
		Agents += ",";
	}
	Agents.pop_back();

	Inworld::Log("Conversation %s %s: %s", Type.c_str(), Conv.Id.c_str(), Agents.c_str());
}

void NDKApp::NDKWrapper::Visit(const Inworld::CustomGestureEvent& Event)
{
	//Inworld::Log("%s: Custom gesture: %s", GetGivenName(Event._Routing._Source._Name).c_str(), Event.GetCustomGesture()));
}

void NDKApp::NDKWrapper::Visit(const Inworld::CancelResponseEvent& Event)
{

}

void NDKApp::NDKWrapper::Visit(const Inworld::EmotionEvent& Event)
{
	//Inworld::Log("%s: Emotion: Behavior %d, Strengths %d", GetGivenName(Event._Routing._Source._Name).c_str(), (int32_t)Event.GetEmotionalBehavior(), (int32_t)Event.GetStrength());
}

void NDKApp::NDKWrapper::Visit(const Inworld::ControlEvent& Event)
{
	Inworld::Log("%s: Control: %d %s", GetGivenName(Event._Routing._Source._Name).c_str(), (int32_t)Event.GetControlAction(), Event.GetDescription().c_str());
}

void NDKApp::NDKWrapper::Visit(const Inworld::SilenceEvent& Event)
{
	Inworld::Log("%s: Silence: Duration %f", GetGivenName(Event._Routing._Source._Name).c_str(), Event.GetDuration());
}

void NDKApp::NDKWrapper::Visit(const Inworld::DataEvent& Event)
{
	Inworld::Log("%s: Data: Size %d", GetGivenName(Event._Routing._Source._Name).c_str(), Event.GetDataChunk().size());
}

std::string NDKApp::NDKWrapper::GetGivenName(const std::string& AgentId) const
{
	auto it = std::find_if(_AgentInfos.begin(), _AgentInfos.end(), [&AgentId](const auto& Info) { return Info.AgentId == AgentId; });
	if (it == _AgentInfos.end())
	{
		return "InvalidName";
	}
	return it->GivenName;
}

bool NDKApp::NDKWrapper::GetRouting(Inworld::Routing& Routing)
{
	if (_CurrentAgentIdx >= 0 && _CurrentAgentIdx < _AgentInfos.size())
	{
		Routing = Inworld::Routing::Player2Agent(_AgentInfos[_CurrentAgentIdx].AgentId);
		return true;
	}

	if (_CurrentConversationIdx >= 0 && _CurrentConversationIdx < _Conversations.size())
	{
		Routing = Inworld::Routing::Player2Conversation(_Conversations[_CurrentConversationIdx].Id);
		return true;
	}

	Error("Current routing is invalid.");
	return false;
}
#pragma endregion Parent Stuff