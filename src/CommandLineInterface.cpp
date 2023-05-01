/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "CommandLineInterface.h"
#include "spdlog/spdlog.h"
#include "Utils/Log.h"

void NDKApp::CommandLineInterface::SetCommands(std::vector<ConsoleCommand> Commands)
{
	_Commands = Commands;
	_Commands.push_back({
			"help",
			"show all commands",
			[this](std::vector<std::string> Args)
			{
				for (auto& Command : _Commands)
				{
					Inworld::Log("{0} - {1}", Command._Id, Command._Desc);
				}
			}
		});
}

void NDKApp::CommandLineInterface::Run()
{
	_AsyncReadKeys.Start("CommandLineInterface", std::make_unique<RunnableReadKeyboard>(_Messages));
}

void NDKApp::CommandLineInterface::Stop()
{
	_AsyncReadKeys.Stop();
}

void NDKApp::CommandLineInterface::ExecuteCommands()
{
	std::string Msg;
	while (_Messages.PopFront(Msg))
	{
		std::stringstream Stream(Msg);

		std::string CmdName;
		std::getline(Stream, CmdName, ' ');

		if (CmdName.empty() || CmdName[0] != '/')
		{
			Inworld::Log("Invalid command: {0}", CmdName);
			continue;
		}

		CmdName.erase(0, 1);

		auto It = std::find_if(_Commands.begin(), _Commands.end(), [&CmdName](const ConsoleCommand& C) { 
			return std::equal(CmdName.begin(), CmdName.end(),
				C._Id.begin(), C._Id.end(),
				[](char a, char b) {
					return tolower(a) == tolower(b);
				}); });

		if (It == _Commands.end())
		{
			Inworld::Log("Invalid command: {0}", CmdName);
			continue;
		}

		std::vector<std::string> Args;
		std::string Tmp;

		if (std::getline(Stream, Tmp))
		{
			Args.push_back(Tmp);
		}

		It->_Task(Args);
	}
}
