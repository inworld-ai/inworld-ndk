/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Application.h"
#include "Utils/Log.h"

// !!! Fill out this options !!!
constexpr std::string_view g_SceneName = "";
constexpr std::string_view g_ApiKey = "";
constexpr std::string_view g_ApiSecret = "";

void NDKApp::App::Run()
{
	if (
		g_SceneName.empty() ||
		g_ApiKey.empty() ||
		g_ApiSecret.empty())
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

				if (_CurrentAgentIdx == -1)
				{
					Error("Invalid character");
					return;
				}

				_Client.SendTextMessage(_AgentInfos[_CurrentAgentIdx].AgentId, Args[0]);
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
			[this](std::vector<std::string> Args)
			{
				for (auto& Info : _AgentInfos)
				{
					Inworld::Log("{0} {1} {2}", Info.GivenName, Info.AgentId, Info.BrainName);
				}
			}
		}
		});

	Inworld::ClientOptions Options;

	Options.AuthUrl = "api-studio.inworld.ai";
	Options.LoadSceneUrl = "api-engine.inworld.ai:443";
	Options.PlayerName = "Player";

	Options.SceneName = g_SceneName;
	Options.ApiKey = g_ApiKey;
	Options.ApiSecret = g_ApiSecret;

	Options.Capabilities.Animations = false;
	Options.Capabilities.Text = true;
	Options.Capabilities.Audio = true;
	Options.Capabilities.Emotions = true;
	Options.Capabilities.Gestures = true;
	Options.Capabilities.Interruptions = true;
	Options.Capabilities.Triggers = true;
	Options.Capabilities.EmotionStreaming = true;
	Options.Capabilities.SilenceEvents = true;
	Options.Capabilities.PhonemeInfo = true;
	Options.Capabilities.LoadSceneInSession = true;

	std::vector<Inworld::AgentInfo> AgentInfos;

	_Client.InitClient(
		"DefaultClientNDK",
		"1.0.0",
		[this](Inworld::Client::ConnectionState ConnectionState)
		{
			std::string Error;
			int32_t Code;
			_Client.GetConnectionError(Error, Code);

			Inworld::Log("Connection state: {0}. Error: {1}", static_cast<uint32_t>(ConnectionState), Error);

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

	Inworld::SessionInfo SessionInfo;
	_Client.StartClient(Options, SessionInfo,
		[this](std::vector<Inworld::AgentInfo> AgentInfos)
		{
			_AgentInfos = AgentInfos;
			if (!AgentInfos.empty())
			{
				_CurrentAgentIdx = 0;
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
	Inworld::Log("{0}", Msg);
}

void NDKApp::App::NextCharacter()
{
	if (_AgentInfos.empty())
	{
		Error("Agents are empty");
		return;
	}

	if (++_CurrentAgentIdx == _AgentInfos.size())
	{
		_CurrentAgentIdx = 0;
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

	if (--_CurrentAgentIdx == 0)
	{
		_CurrentAgentIdx = _AgentInfos.size() - 1;
	}
	NotifyCurrentCharacter();
}

void NDKApp::App::SetCharacter(int32_t Idx)
{
	if (Idx >= 0 && Idx < _AgentInfos.size())
	{
		_CurrentAgentIdx = Idx;
		NotifyCurrentCharacter();
	}
}

void NDKApp::App::NotifyCurrentCharacter()
{
	if (_CurrentAgentIdx != -1)
	{
		auto& Info = _AgentInfos[_CurrentAgentIdx];
		Inworld::Log("Current character: {} {}", Info.GivenName, Info.AgentId);
	}
}
