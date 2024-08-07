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

		void Push(const std::function<void()>& Task)
		{
			_Tasks.PushBack(Task);
		}

	private:
		Inworld::SharedQueue<std::function<void()>> _Tasks;
	};

	struct Conversation
	{
		std::string Id;
		std::vector<std::string> Agents;
	};
	
	class InworldClient
	{
	public:
		InworldClient()
			: _Client(Inworld::CreateClient())
			, _StudioClient(Inworld::CreateStudioClient())
		{}

		~InworldClient()
		{
			Inworld::DestroyClient(std::move(_Client));
			Inworld::DestroyStudioClient(std::move(_StudioClient));
		}

		Inworld::Client& Client() { return *_Client; }
		Inworld::StudioClient& Studio() { return *_StudioClient; }

		TaskExecutor TaskExec;

	private:
		std::unique_ptr<Inworld::Client> _Client;
		std::unique_ptr<Inworld::StudioClient> _StudioClient;
	};
	
	class App : public Inworld::PacketVisitor
	{
	public:
		void Run();

	private:
		void Error(const std::string& Msg);

		void NextCharacter();
		void PrevCharacter();
		void SetCharacter(int32_t Idx);
		void NotifyCurrentCharacter();

	    void SendAudioData();
		
		std::string GetTargetStr(const Inworld::Routing& Routing);

		virtual void Visit(const Inworld::TextEvent& Event) override;
		virtual void Visit(const Inworld::DataEvent& Event) override;
		virtual void Visit(const Inworld::SilenceEvent& Event) override;
		virtual void Visit(const Inworld::ControlEvent& Event) override;
		virtual void Visit(const Inworld::EmotionEvent& Event) override;
		virtual void Visit(const Inworld::CancelResponseEvent& Event) override;
		virtual void Visit(const Inworld::CustomGestureEvent& Event) override;
		virtual void Visit(const Inworld::CustomEvent& Event) override;
		virtual void Visit(const Inworld::AudioDataEvent& Event) override;
		virtual void Visit(const Inworld::ControlEventCurrentSceneStatus& Event) override;
		virtual void Visit(const Inworld::ControlEventConversationUpdate& Event) override;
		
		std::string GetGivenName(const std::string& AgentId) const;

		bool GetRouting(Inworld::Routing& Routing);

	    std::chrono::steady_clock::time_point _LastAudioSentTime;
	    
		Inworld::ClientOptions _Options;
		CommandLineInterface _Cli;

		InworldClient _Client;
		std::string _SavedSessionState;
		std::vector<Inworld::AgentInfo> _AgentInfos;
		std::vector<Conversation> _Conversations;
		int32_t _CurrentAgentIdx = -1;
		int32_t _CurrentConversationIdx = -1;
	};
}
