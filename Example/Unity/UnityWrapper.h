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

		void SetPacketCallBack(
			const TextCallBack& textCallBack,
			const AudioCallBack& audioCallBack,
			const ControlCallBack& controlCallBack,
			const EmotionCallBack& emotionCallBack,
			const CancelResponseCallBack& cancelResponseCallBack,
			const CustomCallBack& customCallBack,
			const PhonemeCallBack& phonemeCallBack,
			const TriggerParamCallBack& triggerParamCallBack);
		
		Inworld::LoggerCallBack GetLoggerCallBack() const
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
		
		void SetCapabilities(Inworld::CapabilitySet capabilities);

		void AddUserProfile(const std::string& strProfileID, const std::string& strProfileVal);

		void LoadScene(const std::string& strSceneName, UnityCallback callback);

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
		std::vector<Inworld::AgentInfo> GetAgentInfo()
		{
			return m_AgentInfos;
		}

		void StopClient() override;
	protected:
		void AddTaskToMainThread(std::function<void()> Task) override;
		
	private:
		int32_t m_CurrentAgentIdx = -1;
		CUnityPacketHandler m_PacketHandler;
		std::string m_SavedSessionState;
		std::vector<Inworld::AgentInfo> m_AgentInfos;
		Inworld::LoggerCallBack m_LogCallBack = nullptr;		
	};
}


