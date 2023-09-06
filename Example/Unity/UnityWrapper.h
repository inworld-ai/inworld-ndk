#pragma once
#include <string>
#include "Client.h"
#include "UnityPacketHandler.h"
#include "Data/UnityNDKInteropData.h"
#include "Utils/Log.h"
	 
using ConnectStateCallback = std::function<void(Inworld::Client::ConnectionState)>;




namespace NDKUnity
{
	
	class CUnityWrapper final : public Inworld::Client
	{
	public:
		CUnityWrapper() = default;
		~CUnityWrapper() override = default;
		
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
		void SetClientRequest(std::string strClientID, std::string strClientVersion);		

		void SetUserRequest(const std::string& strPlayerName, const std::string& strUserID);

		void AddUserProfile(const std::string& strProfileID, const std::string& strProfileVal);

		void LoadScene(const std::string& strSceneName, OnTokenGenerated callback);

		Inworld::ClientOptions GetOptions()
		{
			return _ClientOptions;
		}
		Inworld::UserSettings GetUserRequest()
		{
			return _ClientOptions.UserSettings;
		}
		void SetCapabilities(Inworld::CapabilitySet capabilities);
		

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


