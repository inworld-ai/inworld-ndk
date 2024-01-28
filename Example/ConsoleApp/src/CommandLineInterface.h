/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include <string>
#include <functional>

#include "RunnableCommand.h"
#include "AsyncRoutine.h"

namespace NDKApp
{
	class RunnableReadKeyboard : public Inworld::Runnable
	{
	public:
		RunnableReadKeyboard(Inworld::SharedQueue<std::string>& Messages) : _Messages(Messages) {}

		virtual void Run() override
		{
			for (;;)
			{
				std::string Msg;
				std::getline(std::cin, Msg);
				_Messages.PushBack(Msg);
			}
		}

	private:
		Inworld::SharedQueue<std::string>& _Messages;
	};

	struct ConsoleCommand
	{
		std::string _Id;
		std::string _Desc;
		std::function<void(std::vector<std::string> Args)> _Task;
	};

	class CommandLineInterface
	{
	public:
		CommandLineInterface() = default;

		void SetCommands(std::vector<ConsoleCommand> Commands);

		void Run();
		void Stop();

		bool IsRunning() const { return _AsyncReadKeys.IsValid(); }

		void ExecuteCommands();

	private:
		std::vector<ConsoleCommand> _Commands;
		Inworld::SharedQueue<std::string> _Messages;
		Inworld::AsyncRoutine _AsyncReadKeys;
	};
}