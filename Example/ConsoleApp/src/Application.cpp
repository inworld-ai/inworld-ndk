/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Application.h"
#include "Utils/Log.h"

// !!! Fill out this options !!!
constexpr std::string_view g_SceneName = "";
constexpr std::string_view g_Base64 = "";
constexpr std::string_view g_ApiKey = "";
constexpr std::string_view g_ApiSecret = "";

void NDKApp::App::Run()
{
	if (
		g_SceneName.empty() ||
		(g_Base64.empty() &&
		(g_ApiKey.empty() ||
		g_ApiSecret.empty())))
	{
		Inworld::LogError("Fill out client options in Application.cpp");
		return;
	}

	_Cli.SetCommands({
		{
			"Text",
			"Send text message to a character (Arg0: Text)",
			[this](std::vector<std::string> Args)
			{
				if (Args.size() != 1)
				{
					Error("Invalid args");
					return;
				}

				auto CurAgents = GetCurrentAgentBrains();
				if (CurAgents.empty())
				{
					Error("Invalid character");
					return;
				}

				_Client.SendTextMessage(CurAgents, Args[0]);
			}
		},
		{
			"NextTurn",
			"Next turn in conversation between multiple chars",
			[this](std::vector<std::string> Args)
			{
				if (Args.size() > 0)
				{
					Error("Invalid args");
					return;
				}

				auto CurAgents = GetCurrentAgentBrains();
				if (CurAgents.size() < 2)
				{
					Error("Need multiple characters");
					return;
				}

				Inworld::Log("Send inworld.conversation.next_turn");
				_Client.SendCustomEvent(CurAgents, "inworld.conversation.next_turn", {});
			}
		},
		{
			"Narration",
			"Send narration to a character (Arg0: Text)",
			[this](std::vector<std::string> Args)
			{
				if (_CurrentAgentIdxs.empty())
				{
					Error("Invalid character");
					return;
				}

				_Client.SendNarrationEvent(_AgentInfos[_CurrentAgentIdxs[0]].AgentId, Args[0]);
				Inworld::Log("Narration sent.");
			}
		},
		{
			"NextChar",
			"Next character",
			[this](std::vector<std::string> Args)
			{
				NextCharacter();
			}
		},
		{
			"PrevChar",
			"Previous character",
			[this](std::vector<std::string> Args)
			{
				PrevCharacter();
			}
		},
		{
			"SetChar",
			"Set character index (Arg0: Index)",
			[this](std::vector<std::string> Args)
			{
				if (Args.empty())
				{
					Error("Invalid args");
					return;
				}

				std::vector<int32_t> Idxs;
				for (std::string& Arg : Args)
				{
					Idxs.push_back(std::stoi(Arg));
				}
				SetCharacter(Idxs);
			}
		},
		{
			"Chars",
			"Show info about all characters",
			[this](std::vector<std::string> Args)
			{
				for (auto& Info : _AgentInfos)
				{
					Inworld::Log("{0} {1} {2}", ARG_STR(Info.GivenName), ARG_STR(Info.AgentId), ARG_STR(Info.BrainName));
				}
			}
		},
		{
			"Save",
			"Save current session state",
			[this](std::vector<std::string> Args)
			{
				_Client.SaveSessionState([this](std::string State, bool bSuccess)
					{
						if (!bSuccess)
						{
							Inworld::LogError("Save session failed");
							return;
						}

						_SavedSessionState = State;
						Inworld::Log("Session state saved, size '{0}'", State.size());
					});
			}
		},
		{
			"Restart",
			"Restart session with saved state",
			[this](std::vector<std::string> Args)
			{
				_Client.StopClient();

				Inworld::SessionInfo SessionInfo;
				SessionInfo.SessionSavedState = _SavedSessionState;
				_Client.StartClient(_Options, SessionInfo,
					[this](std::vector<Inworld::AgentInfo> AgentInfos)
					{
						_AgentInfos = AgentInfos;
						if (!AgentInfos.empty())
						{
							_CurrentAgentIdxs.empty();
							_CurrentAgentIdxs.push_back(0);
							NotifyCurrentCharacter();
						}
					});
			}
		}
		});

		_Options.ServerUrl = "api-engine.inworld.ai:443";
	_Options.PlayerName = "Player";

	_Options.SceneName = g_SceneName;
	_Options.Base64 = g_Base64;
	_Options.ApiKey = g_ApiKey;
	_Options.ApiSecret = g_ApiSecret;

	_Options.Capabilities.Animations = false;
	_Options.Capabilities.Text = true;
	_Options.Capabilities.Audio = true;
	_Options.Capabilities.Emotions = true;
	_Options.Capabilities.Gestures = true;
	_Options.Capabilities.Interruptions = true;
	_Options.Capabilities.Triggers = true;
	_Options.Capabilities.EmotionStreaming = true;
	_Options.Capabilities.SilenceEvents = true;
	_Options.Capabilities.PhonemeInfo = true;
	_Options.Capabilities.LoadSceneInSession = true;
	_Options.Capabilities.NarratedActions = true;
	_Options.Capabilities.Multiagent = true;

	std::vector<Inworld::AgentInfo> AgentInfos;

	_Client.InitClient(
		{},
		[this](Inworld::Client::ConnectionState ConnectionState)
		{
			std::string Error;
			int32_t Code;
			_Client.GetConnectionError(Error, Code);

			Inworld::Log("Connection state: {0}. {1}", static_cast<uint32_t>(ConnectionState), Error.empty() ? "" : ARG_STR(std::string(" Error: ") + Error));

			if (ConnectionState == Inworld::Client::ConnectionState::Disconnected)
			{
				_Client.ResumeClient();
			}
		},
		[this](std::shared_ptr<Inworld::Packet> Packet)
		{
			Packet->Accept(_PacketHandler);
		}
		);

	_Client.SetPerceivedLatencyTrackerCallback([](const std::string& InteractonId, int32_t Latency)
		{
			//Inworld::Log("PerceivedLatencyTracker. Latency is '%d', Interaction: %s", Latency, ARG_STR(InteractonId));
		});

	Inworld::SessionInfo SessionInfo;
	_Client.StartClient(_Options, SessionInfo,
		[this](std::vector<Inworld::AgentInfo> AgentInfos)
		{
			_AgentInfos = AgentInfos;
			_PacketHandler._AgentInfos = _AgentInfos;
			if (!AgentInfos.empty())
			{
				_CurrentAgentIdxs.clear();
				_CurrentAgentIdxs.push_back(0);
				if (_AgentInfos.size() > 1)
					_CurrentAgentIdxs.push_back(1);

				NotifyCurrentCharacter();
			}
		});

	for (;;)
	{
		_Client.Update();

		if (!_AgentInfos.empty() && !_Cli.IsRunning())
		{
			_Cli.Run();
		}

		_Cli.ExecuteCommands();

		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
}

void NDKApp::App::Error(std::string Msg)
{
	Inworld::LogError("{0}", ARG_STR(Msg));
}

void NDKApp::App::NextCharacter()
{
	if (_AgentInfos.empty())
	{
		Error("Agents are empty");
		return;
	}

	if (_CurrentAgentIdxs.empty())
	{
		_CurrentAgentIdxs.push_back(0);
	}
	else if (_CurrentAgentIdxs.size() > 1)
	{
		Error("Multiple characters, use SetChar");
	}
	else if (++_CurrentAgentIdxs[0] == _AgentInfos.size())
	{
		_CurrentAgentIdxs[0] = 0;
	}
	NotifyCurrentCharacter();
}

void NDKApp::App::PrevCharacter()
{
	if (_AgentInfos.empty())
	{
		Error("Agents are empty");
		return;
	}

	if (_CurrentAgentIdxs.empty())
	{
		_CurrentAgentIdxs.push_back(0);
	}
	else if (_CurrentAgentIdxs.size() > 1)
	{
		Error("Multiple characters, use SetChar");
	}
	else if (--_CurrentAgentIdxs[0] == 0)
	{
		_CurrentAgentIdxs[0] = _AgentInfos.size() - 1;
	}

	NotifyCurrentCharacter();
}

void NDKApp::App::SetCharacter(const std::vector<int32_t>& Idxs)
{
	if (Idxs.empty())
	{
		Error("Empty char list");
		return;
	}

	_CurrentAgentIdxs.clear();
	for (int32_t Idx : Idxs)
	{
		if (Idx >= 0 && Idx < _AgentInfos.size())
		{
			_CurrentAgentIdxs.push_back(Idx);
			NotifyCurrentCharacter();
		}
	}
}

void NDKApp::App::NotifyCurrentCharacter()
{
	if (_CurrentAgentIdxs.empty())
	{
		Inworld::Log("No current character");
	}

	for (int32_t Idx : _CurrentAgentIdxs)
	{
		auto& Info = _AgentInfos[Idx];
		Inworld::Log("Current character: {} {}", Idx, ARG_STR(Info.GivenName), ARG_STR(Info.AgentId));
	}
}

std::vector<std::string> NDKApp::App::GetCurrentAgentBrains() const
{
	std::vector<std::string> Brains;
	for (int32_t i = 0; i < _CurrentAgentIdxs.size(); i++)
	{
		Brains.push_back(_AgentInfos[i].AgentId);
	}
	return Brains;
}
