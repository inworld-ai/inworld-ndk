/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include <vector>

#include "Client.h"
#include "StudioClient.h"
#include "CommandLineInterface.h"

namespace NDKApp
{
	class TaskExecutor
	{
	public:
		void Execute()
		{
			std::function<void()> Task;
			while (_Tasks.PopFront(Task))
			{
				Task();
			}
		}

		void Push(std::function<void()> Task)
		{
			_Tasks.PushBack(Task);
		}

	private:
		Inworld::SharedQueue<std::function<void()>> _Tasks;
	};
	
	class InworldClient
	{
	public:
		InworldClient() 
		{
			Inworld::CreateClient();
			Inworld::CreateStudioClient();
		}

		~InworldClient()
		{
			Inworld::DestroyClient();
			Inworld::DestroyStudioClient();
		}

		Inworld::Client& Client() { return *Inworld::GetClient(); }
		Inworld::StudioClient& Studio() { return *Inworld::GetStudioClient(); }

		TaskExecutor TaskExec;
	};
	
	class App : public Inworld::PacketVisitor
	{
	public:
		void Run();

	private:
		void Error(std::string Msg);

		void NextCharacter();
		void PrevCharacter();
		void SetCharacter(const std::vector<int32_t>& Idxs);
		void NotifyCurrentCharacter();

		virtual void Visit(const Inworld::TextEvent& Event) override;
		virtual void Visit(const Inworld::DataEvent& Event) override;
		virtual void Visit(const Inworld::SilenceEvent& Event) override;
		virtual void Visit(const Inworld::ControlEvent& Event) override;
		virtual void Visit(const Inworld::EmotionEvent& Event) override;
		virtual void Visit(const Inworld::CancelResponseEvent& Event) override;
		virtual void Visit(const Inworld::CustomGestureEvent& Event) override;
		virtual void Visit(const Inworld::CustomEvent& Event) override;
		virtual void Visit(const Inworld::AudioDataEvent& Event) override;
		virtual void Visit(const Inworld::SessionControlResponse_LoadScene& Event) override;
		virtual void Visit(const Inworld::SessionControlResponse_LoadCharacters& Event) override;
		
		std::string GetGivenName(const std::string& AgentId) const;

		std::vector<std::string> GetCurrentAgentBrains() const;

		Inworld::ClientOptions _Options;
		CommandLineInterface _Cli;

		InworldClient _Client;
		std::string _SavedSessionState;
		std::vector<Inworld::AgentInfo> _AgentInfos;
		std::vector<int32_t> _CurrentAgentIdxs;
	};
}