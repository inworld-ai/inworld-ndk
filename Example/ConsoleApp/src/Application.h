/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
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
		void SetCharacter(const std::vector<int32_t>& Idxs);
		void NotifyCurrentCharacter();

		std::vector<std::string> GetCurrentAgentBrains() const;

		Inworld::Client _Client;
		Inworld::ClientOptions _Options;
		CommandLineInterface _Cli;
		PacketHandler _PacketHandler;

		std::string _SavedSessionState;
		std::vector<Inworld::AgentInfo> _AgentInfos;
		std::vector<int32_t> _CurrentAgentIdxs;
	};
}