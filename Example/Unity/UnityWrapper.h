#pragma once
#include "ClientOptions.h"
#include "PacketHandler.h"
#include <string>
#include "Client.h"
#include "Utils/Log.h"
	 
using ConnectStateCallback = std::function<void(Inworld::Client::ConnectionState)>;

namespace NDKUnity
{
	class Wrapper
	{
	public:
		void SetLoggerCallBack(const Inworld::UnityLogCallback& callback);
	private:
		Inworld::Client m_Client;
		ClientOptions m_Options;
		int32_t m_CurrentAgentIdx = -1;
		PacketHandler m_PacketHandler;
		std::string m_SavedSessionState;
		std::vector<Inworld::AgentInfo> m_AgentInfos;
		
		Inworld::UnityLogCallback m_LogCallBack;
		ConnectStateCallback m_ConnectStateCallback;
	};
}


