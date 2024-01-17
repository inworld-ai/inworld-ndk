/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include <vector>

#include "Client.h"
#include "CommandLineInterface.h"
#include "PacketHandler.h"

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

		std::string _SavedSessionState;
		std::vector<Inworld::AgentInfo> _AgentInfos;
		int32_t _CurrentAgentIdx = -1;
	};
}