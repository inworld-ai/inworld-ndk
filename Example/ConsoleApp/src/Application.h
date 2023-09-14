/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include <vector>
#include <chrono>

#include "Client.h"
#include "CommandLineInterface.h"
#include "PacketHandler.h"
#include "examples/common-sdl.h"

namespace NDKApp
{
	class App 
	{
	public:
		void Run();

	private:
		void Error(std::string Msg);

		void NextCharacter();
		void PrevCharacter();
		void SetCharacter(int32_t Idx);
		void NotifyCurrentCharacter();

		Inworld::Client _Client;
		Inworld::ClientOptions _Options;
		CommandLineInterface _Cli;
		PacketHandler _PacketHandler;

		audio_async _SdlAudio{ 30000 };
		std::chrono::system_clock::time_point AutioStartTime;
		bool _CapturingAudio = false;

		std::string _SavedSessionState;
		std::vector<Inworld::AgentInfo> _AgentInfos;
		int32_t _CurrentAgentIdx = -1;
	};
}