#pragma once
#include "UnityClientOptions.h"
#include <string>
#include "Client.h"
#include "UnityPacketHandler.h"
#include "Utils/Log.h"
	 
using ConnectStateCallback = std::function<void(Inworld::Client::ConnectionState)>;

namespace NDKUnity
{
	class CUnityWrapper final
	{
	public:
		CUnityWrapper() = default;
		~CUnityWrapper() = default;
		void SetLoggerCallBack(const Inworld::UnityLogCallback& callback);		
		Inworld::UnityLogCallback GetLoggerCallBack() const
		{
			return m_LogCallBack;
		}
		
		void SetServerURL(const std::string& strServerURL)
		{
			m_Options.ServerUrl = strServerURL;
		}
		void SetAPIKey(std::string strAPIKey)
		{
			m_Options.ApiKey = strAPIKey;
		}
		void SetAPISecret(std::string strAPISecret)
		{
			m_Options.ApiSecret = strAPISecret;
		}
		Inworld::SessionInfo GetAccessToken();
		CUnityClientOptions GetOptions() const
		{
			return m_Options;
		}
		
	private:
		Inworld::Client m_Client;
		CUnityClientOptions m_Options;
		int32_t m_CurrentAgentIdx = -1;
		CUnityPacketHandler m_PacketHandler;
		std::string m_SavedSessionState;
		std::vector<Inworld::AgentInfo> m_AgentInfos;
		
		Inworld::UnityLogCallback m_LogCallBack = nullptr;
		ConnectStateCallback m_ConnectStateCallback;
	};
}


