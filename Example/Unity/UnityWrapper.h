#pragma once
#include "UnityClientOptions.h"
#include <string>
#include "Client.h"
#include "UnityPacketHandler.h"
#include "Utils/Log.h"
	 
using ConnectStateCallback = std::function<void(Inworld::Client::ConnectionState)>;

namespace NDKUnity
{
	class CUnityWrapper : public Inworld::Client
	{
	public:
		CUnityWrapper() = default;
		virtual ~CUnityWrapper() = default;
		
		void SetLoggerCallBack(const Inworld::UnityLogCallback& callback);		
		Inworld::UnityLogCallback GetLoggerCallBack() const
		{
			return m_LogCallBack;
		}
		
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
	protected:
		void AddTaskToMainThread(std::function<void()> Task) override;
		
	private:
		int32_t m_CurrentAgentIdx = -1;
		CUnityPacketHandler m_PacketHandler;
		std::string m_SavedSessionState;
		std::vector<Inworld::AgentInfo> m_AgentInfos;
		
		Inworld::UnityLogCallback m_LogCallBack = nullptr;
		ConnectStateCallback m_ConnectStateCallback;
	};
}


