/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Application.h"

#include "Microphone.h"
#include "Utils/Log.h"
#include "Packets.h"
#ifdef INWORLD_VAD
#include "InworldVAD.h"
#endif

#include <filesystem>

// !!! Fill out this options !!!
constexpr std::string_view g_SceneId = "";
constexpr std::string_view g_Base64 = "";
constexpr std::string_view g_ApiKey = "";
constexpr std::string_view g_ApiSecret = "";

static bool IsConfigValid()
{
	// cppcheck-suppress redundantCondition
	// cppcheck-suppress knownConditionTrueFalse
	if (g_SceneId.empty())
	{
		return false;
	}
	// cppcheck-suppress redundantCondition
	// cppcheck-suppress knownConditionTrueFalse
	// cppcheck-suppress identicalConditionAfterEarlyExit
	return !g_Base64.empty() || (!g_ApiKey.empty() && !g_ApiSecret.empty());
}

void NDKApp::App::Run()
{
	if (!IsConfigValid())
	{
		Inworld::LogError("Fill out client options in Application.cpp");
		return;
	}

	bool bQuit = false;
	_Cli.SetCommands({
		{
			"Text",
			"Send text message to a character (Arg: Text)",
			[this](const std::vector<std::string>& Args)
			{
				std::string Text;
				for (const auto& A : Args)
				{
					Text += A + " ";
				}

				Inworld::Routing R;
				if (!GetRouting(R))
				{
					return;
				}
				
				_Client.Client().SendTextMessage(R, Text);
			}
		},
		{
			"NextTurn",
			"Next turn in conversation between multiple chars",
			[this](const std::vector<std::string>& Args)
			{
				if (Args.size() > 0)
				{
					Error("Invalid args");
					return;
				}

				if (_CurrentConversationIdx == -1)
				{
					Error("No conversation selected");
					return;
				}

				Inworld::Routing R;
				if (!GetRouting(R))
				{
					return;
				}

				Inworld::Log("Send inworld.conversation.next_turn");
				_Client.Client().SendCustomEvent(R, "inworld.conversation.next_turn", {});
			}
		},
		{
			"Narration",
			"Send narration to a character (Arg: Text)",
			[this](const std::vector<std::string>& Args)
			{
				std::string Text;
				for (const auto& A : Args)
				{
					Text += A + " ";
				}

				Inworld::Routing R;
				if (!GetRouting(R))
				{
					return;
				}
				
				_Client.Client().SendNarrationEvent(R, Text);
				Inworld::Log("Narration sent.");
			}
		},
		{
			"NextChar",
			"Next character",
			[this](const std::vector<std::string>& Args)
			{
				NextCharacter();
			}
		},
		{
			"PrevChar",
			"Previous character",
			[this](const std::vector<std::string>& Args)
			{
				PrevCharacter();
			}
		},
		{
			"SetChar",
			"Set character index (Arg: Index)",
			[this](const std::vector<std::string>& Args)
			{
				if (Args.size() != 1)
				{
					Error("Invalid args");
					return;
				}

				int32_t Idx = std::stoi(Args[0]);
				SetCharacter(Idx);
			}
		},
		{
			"Chars",
			"Show info about all characters",
			[this](const std::vector<std::string>& Args)
			{
				for (const auto& Info : _AgentInfos)
				{
					Inworld::Log("%s %s %s", Info.GivenName.c_str(), Info.AgentId.c_str(), Info.BrainName.c_str());
				}
				NotifyCurrentCharacter();
			}
		},
		{
			"Save",
			"Save current session state",
			[this](const std::vector<std::string>& Args)
			{
				_Client.Client().SaveSessionStateAsync([this](const Inworld::SessionSave& Save, bool bSuccess)
				{
					_Client.TaskExec.Push([this, Save, bSuccess]()
					{
						if (!bSuccess)
						{
							Inworld::LogError("Save session failed");
							return;
						}

						_SessionSave = Save;
						Inworld::Log("Session state saved, size '%d'", _SessionSave.State.size());
					});
				});
			}
		},
		{
			"Restart",
			"Restart session with saved state",
			[this](const std::vector<std::string>& Args)
			{
				_Client.Client().StopClient();

				_Client.Client().SetOptions(_Options);
				_Client.Client().StartClientFromSave(_SessionSave);
			}
		},
		{
			"LoadScene",
			"Load scene",
			[this](const std::vector<std::string>& Args)
			{
				if (Args.size() != 1)
				{
					Error("Invalid args");
					return;
				}

				_Client.Client().LoadScene(Args[0]);
			}
		},
		{
			"LoadChars",
			"Load characters",
			[this](const std::vector<std::string>& Args)
			{
				if (Args.empty())
				{
					Error("Invalid args");
					return;
				}

				_Client.Client().LoadCharacters(Args);
			}
		},
		{
			"UnloadChars",
			"Unload characters",
			[this](const std::vector<std::string>& Args)
			{
				if (Args.empty())
				{
					Error("Invalid args");
					return;
				}
				
				bool bDropCurrentChar = false;
				for (auto& Id : Args)
				{
					auto It = std::find_if(_AgentInfos.begin(), _AgentInfos.end(), [&Id](const auto& Info) { return Info.BrainName == Id; });
					if (It != _AgentInfos.end())
					{
						Inworld::Log("Unload character %s", Id.c_str());
						int32_t Idx = _AgentInfos.begin() - It;
						_AgentInfos.erase(It);
						if (Idx == _CurrentAgentIdx)
						{
							bDropCurrentChar = true;
						}
					}
				}
				if (bDropCurrentChar)
				{
					_CurrentAgentIdx = 0;
					NotifyCurrentCharacter();
				}

				_Client.Client().UnloadCharacters(Args);
			}
		},
		{
			"SetSave",
			"Set save",
			[this](const std::vector<std::string>& Args)
			{
				if (Args.size() != 2)
				{
					Error("Invalid args");
					return;
				}

				_SessionSave.SceneId = Args[0];
				_SessionSave.State = Args[1];
			}
		},
		{
			"SetCapabilities",
			"Set capabilities",
			[this](const std::vector<std::string>& Args)
			{
				_Options.Capabilities.Animations = false;
				_Options.Capabilities.Audio = false;
				_Options.Capabilities.Emotions = true;
				_Options.Capabilities.Interruptions = true;
				_Options.Capabilities.EmotionStreaming = true;
				_Options.Capabilities.SilenceEvents = true;
				_Options.Capabilities.PhonemeInfo = true;
				_Options.Capabilities.NarratedActions = true;
				_Options.Capabilities.MultiAgent = true;

				if (!Args.empty() && Args[0] == "audio")
				{
					_Options.Capabilities.Audio = true;
				}
			}
		},
		{
			"SetUserConfig",
			"Set user config",
			[this](const std::vector<std::string>& Args)
			{
				if (Args.size() != 1)
				{
					Error("Invalid args");
					return;
				}

				_Options.UserConfig.Name = Args[0];
			}
		},
		{
			"ConvStart",
			"Start conversation",
			[this](const std::vector<std::string>& Args)
			{
				if (Args.size() < 3)
				{
					Error("Invalid args");
					return;
				}

				const bool bIncludePlayer = Args[0] == "true";
				std::vector<std::string> AgentIds;
				for (int32_t i = 1; i < Args.size(); ++i)
				{
					int32_t Idx = std::stoi(Args[i]);
					if (Idx < 0 || Idx >= _AgentInfos.size())
					{
						Error("Invalid agent index");
						return;
					}
					AgentIds.push_back(_AgentInfos[Idx].AgentId);
				}
						
				_Client.Client().UpdateConversation(AgentIds, "", bIncludePlayer);
			}
		},
		{
			"ConvUpd",
			"Update conversation",
			[this](const std::vector<std::string>& Args)
			{
				if (Args.size() < 4)
				{
					Error("Invalid args");
					return;
				}

				const bool bIncludePlayer = Args[0] == "true";
				std::vector<std::string> AgentIds;
				for (int32_t i = 2; i < Args.size(); ++i)
				{
					int32_t Idx = std::stoi(Args[i]);
					if (Idx < 0 || Idx >= _AgentInfos.size())
					{
						Error("Invalid agent index");
						return;
					}
					AgentIds.push_back(_AgentInfos[Idx].AgentId);
				}
								
				_Client.Client().UpdateConversation(AgentIds, Args[1], bIncludePlayer);
			}
		},
		{
			"Destroy",
			"Destroy client and restart",
			[this](const std::vector<std::string>& Args)
			{
				_Client.Client().DestroyClient();
				Run();
			}
		},
		{
			"Resume",
			"Resume client",
			[this](const std::vector<std::string>& Args)
			{
				_Client.Client().ResumeClient();
			}
		},
		{
			"Quit",
			"Quit app",
			[this, &bQuit](const std::vector<std::string>& Args)
			{
				bQuit = true;
			}
		},
#ifdef INWORLD_VAD
        {
            "VadTest",
            "Test VAD",
            [this](const std::vector<std::string>& Args)
            {
                Inworld::VAD_Initialize("model");
                std::vector<float> AudioData(300000, 0.0f);
                const float Res = Inworld::VAD_Process(AudioData.data(), AudioData.size());
                Inworld::Log("VAD result %f", Res);
				Inworld::VAD_Terminate();
            }
        },
#endif
        {
            "ba",
            "Begin audio capture",
            [this](const std::vector<std::string>& Args)
            {
                if (Inworld::Mic::IsCapturing())
                {
                    return;
                }
                
                Inworld::Routing R;
                if (!GetRouting(R))
                {
                    return;
                }

                Inworld::AudioSessionStartPayload Pl{
                    Inworld::AudioSessionStartPayload::MicrophoneMode::OpenMic,
                    Inworld::AudioSessionStartPayload::UnderstandingMode::Full
                };
                _Client.Client().StartAudioSession(R, Pl);
                Inworld::Mic::StartCapture();
                Inworld::Log("Audio capture started");
            }
        },
        {
            "ea",
            "End audio capture",
            [this](const std::vector<std::string>& Args)
            {
                if (!Inworld::Mic::IsCapturing())
                {
                    return;
                }

                Inworld::Routing R;
                if (!GetRouting(R))
                {
                    return;
                }
                
                _Client.Client().StopAudioSession(R);
                Inworld::Mic::StopCapture();
                SendAudioData();
                Inworld::Log("Audio capture stopped");
            }
        }
	});

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

    _LastAudioSentTime = std::chrono::steady_clock::now();
	while (!bQuit)
	{
		_Client.TaskExec.Execute();

		if (!_AgentInfos.empty() && !_Cli.IsRunning())
		{
			_Cli.Run();
		}

		_Cli.ExecuteCommands();

	    std::chrono::steady_clock::time_point Now = std::chrono::steady_clock::now();
	    if (Now - _LastAudioSentTime > std::chrono::milliseconds(100)
	        && Inworld::Mic::IsCapturing())
	    {
	        SendAudioData();
	    }
	}
}

void NDKApp::App::Error(const std::string& Msg)
{
	Inworld::LogError("%s", Msg.c_str());
}

void NDKApp::App::NextCharacter()
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

void NDKApp::App::PrevCharacter()
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

void NDKApp::App::SetCharacter(int32_t Idx)
{
	_CurrentAgentIdx = Idx;
	_CurrentConversationIdx = -1;
	NotifyCurrentCharacter();
}

void NDKApp::App::NotifyCurrentCharacter()
{
	auto& Info = _AgentInfos[_CurrentAgentIdx];
	Inworld::Log("Current character: %d %s %s", _CurrentAgentIdx, Info.GivenName.c_str(), Info.AgentId.c_str());
}

void NDKApp::App::SendAudioData()
{
    std::string Data;
    Inworld::Routing R;
    if (GetRouting(R) && Inworld::Mic::GetAudio(Data))
    {
        _Client.Client().SendSoundMessage(R, Data);
    }
    
    _LastAudioSentTime = std::chrono::steady_clock::now();
}

std::string NDKApp::App::GetTargetStr(const Inworld::Routing& Routing)
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

void NDKApp::App::Visit(const Inworld::TextEvent& Event)
{
    if (Event.IsFinal())
	    Inworld::Log("%s to %s: Text: %s", GetGivenName(Event._Routing._Source._Name).c_str(), GetTargetStr(Event._Routing).c_str(), Event.GetText().c_str());
}

void NDKApp::App::Visit(const Inworld::CustomEvent& Event)
{
	Inworld::Log("%s: Custom: %s", GetGivenName(Event._Routing._Source._Name).c_str(), Event.GetName().c_str());
}

void NDKApp::App::Visit(const Inworld::AudioDataEvent& Event)
{
	Inworld::Log("%s: Audio: %d", GetGivenName(Event._Routing._Source._Name).c_str(), Event.GetDataChunk().size());
}

void NDKApp::App::Visit(const Inworld::ControlEventCurrentSceneStatus& Event)
{
	_Client.TaskExec.Push([this, AgentInfos = Event.GetAgentInfos()]()
		{
			_AgentInfos = AgentInfos;
			if (!AgentInfos.empty())
			{
				_CurrentAgentIdx = 0;
				NotifyCurrentCharacter();
			}
		});
}

void NDKApp::App::Visit(const Inworld::ControlEventConversationUpdate& Event)
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

	const std::string Type = Event.GetType() ==	1 ? "STARTED" : Event.GetType() == 2 ? "UPDATED" : "EVICTED";
	std::string Agents;
	for (const auto& A : Conv.Agents)
	{
		Agents += GetGivenName(A);
		Agents += ",";
	}
	Agents.pop_back();

	Inworld::Log("Conversation %s %s: %s", Type.c_str(), Conv.Id.c_str(), Agents.c_str());
}

void NDKApp::App::Visit(const Inworld::CustomGestureEvent& Event)
{
	//Inworld::Log("%s: Custom gesture: %s", GetGivenName(Event._Routing._Source._Name).c_str(), Event.GetCustomGesture()));
}

void NDKApp::App::Visit(const Inworld::CancelResponseEvent& Event)
{

}

void NDKApp::App::Visit(const Inworld::EmotionEvent& Event)
{
	//Inworld::Log("%s: Emotion: Behavior %d, Strengths %d", GetGivenName(Event._Routing._Source._Name).c_str(), (int32_t)Event.GetEmotionalBehavior(), (int32_t)Event.GetStrength());
}

void NDKApp::App::Visit(const Inworld::ControlEvent& Event)
{
	Inworld::Log("%s: Control: %d %s", GetGivenName(Event._Routing._Source._Name).c_str(), (int32_t)Event.GetControlAction(), Event.GetDescription().c_str());
}

void NDKApp::App::Visit(const Inworld::SilenceEvent& Event)
{
	Inworld::Log("%s: Silence: Duration %f", GetGivenName(Event._Routing._Source._Name).c_str(), Event.GetDuration());
}

void NDKApp::App::Visit(const Inworld::DataEvent& Event)
{
	Inworld::Log("%s: Data: Size %d", GetGivenName(Event._Routing._Source._Name).c_str(), Event.GetDataChunk().size());
}

std::string NDKApp::App::GetGivenName(const std::string& AgentId) const
{
	auto it = std::find_if(_AgentInfos.begin(), _AgentInfos.end(), [&AgentId](const auto& Info) { return Info.AgentId == AgentId; });
	if (it == _AgentInfos.end())
	{
		return "InvalidName";
	}
	return it->GivenName;
}

bool NDKApp::App::GetRouting(Inworld::Routing& Routing)
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
