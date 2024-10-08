﻿/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include <string>
#include "Client.h"
#include "UnityPacketHandler.h"
#include "Data/UnityNDKInteropData.h"
#include "Utils/Log.h"

namespace NDKUnity
{	
	class CUnityWrapper final : public Inworld::Client
	{
	public:
		CUnityWrapper()
		{			
			_OnPacketCallback = [this](const std::shared_ptr<Inworld::Packet>& packet)
			{
				packet->Accept(m_PacketHandler);
			};
		}

		~CUnityWrapper() override = default;
		
		void SetLoggerCallBack(const Inworld::LoggerCallBack& callback);

		void SetPacketCallBack(const UnityPacketCallback& pktCallBack, const PhonemeCallBack& phonemeCallBack, const TriggerParamCallBack& triggerParamCallBack);
		
		Inworld::LoggerCallBack GetLoggerCallBack() const
		{
			return m_LogCallBack;
		}

		void SetSessionState(const std::string& strSessionState);
		void SetServerURL(const std::string& strServerURL)
		{
			_ClientOptions.ServerUrl = strServerURL;
		}
		void SetAPIKey(std::string strAPIKey)
		{
			_ClientOptions.ApiKey = strAPIKey;
		}
		void SetAPISecret(std::string strAPISecret)
		{
			_ClientOptions.ApiSecret = strAPISecret;
		}
		void SetClientRequest(const std::string& strClientID, const std::string& strClientVersion, const std::string& strClientDescription);

		void SetPublicWorkspace(const std::string& strPublicWorkspace);

		void SetUserRequest(const std::string& strPlayerName, const std::string& strUserID);
		
		void SetCapabilities(Inworld::CapabilitySet capabilities);

		void AddUserProfile(const std::string& strProfileID, const std::string& strProfileVal);

		void LoadScene(const std::string& strSceneName, const std::string& strSessionState, UnityCallback callback);		

		void StartSession();

		void EndSession();		

		Inworld::ClientOptions GetOptions()
		{
			return _ClientOptions;
		}
		Inworld::UserSettings GetUserRequest()
		{
			return _ClientOptions.UserSettings;
		}
		std::vector<AgentInfo> GetAgentInfo()
		{
			return m_AgentInfos;
		}

		void StopClient() override;

	protected:
		void AddTaskToMainThread(std::function<void()> Task) override;
		
	private:
		CUnityPacketHandler m_PacketHandler;
		std::vector<AgentInfo> m_AgentInfos;
		
		Inworld::LoggerCallBack m_LogCallBack = nullptr;		
	};
}


