/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include <string>
#include "../ConsoleApp/src/Application.h"
#include "Client.h"
#include "UnityPacketHandler.h"
#include "Data/UnityNDKInteropData.h"
#include "Utils/Log.h"

namespace NDKApp
{	
	class NDKWrapper : public Inworld::PacketVisitor
	{
	public:
		void Init();
		virtual ~NDKWrapper() = default;
		void SetLoggerCallBack(const Inworld::LogCallback& info, const Inworld::LogCallback& warning, const Inworld::LogCallback& error);
		void SendText(std::string text);
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
		Inworld::SessionSave _SessionSave;
		std::vector<Inworld::AgentInfo> _AgentInfos;
		std::vector<Conversation> _Conversations;
		int32_t _CurrentAgentIdx = -1;
		int32_t _CurrentConversationIdx = -1;
		
		Inworld::LogCallback m_LogInfo = nullptr;
		Inworld::LogCallback m_LogWarning = nullptr;
		Inworld::LogCallback m_LogError = nullptr;

	};
}


