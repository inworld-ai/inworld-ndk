#define CLIENT_WRAPPER_H

#include <stdint.h>
#include "proto/inworld_ndkData.pb.h"
#include "Client.h"

extern "C" {

    struct ClientWrapper {
        Inworld::Client client;
    };

    //methods will be invoked in Unity
    typedef void (*ConnectionStateCallbackType)(int connectionState);
    typedef void (*PacketCallbackType)(const uint8_t* packet, int packetSize);
    typedef void (*LoadSceneCallbackType)(const uint8_t* serialized_agent_info_array, int serialized_agent_info_array_size);

    struct ClientWrapper;

   __declspec(dllexport) ClientWrapper* ClientWrapper_create();
   __declspec(dllexport) void ClientWrapper_destroy(ClientWrapper* wrapper);

   //possibly unneccessary creates circular dependencies with wrapping parameters and protobuf classes 
   __declspec(dllexport) void ClientWrapper_SendPacket(ClientWrapper* wrapper, const void* data, size_t size);
   //std::shared_ptr<Inworld::Packet> ResolvePackets(InworldPakets::InworldPacket packet);

   __declspec(dllexport) void ClientWrapper_SendTextMessage(ClientWrapper* wrapper, const char* AgentId, const char* Text);
   __declspec(dllexport) void ClientWrapper_SendSoundMessage(ClientWrapper* wrapper, const char* AgentId, const char* Data);
   __declspec(dllexport) void ClientWrapper_SendSoundMessageWithAEC(ClientWrapper* wrapper, const char* AgentId, const int16_t* InputData, int InputDataSize, const int16_t* OutputData, int OutputDataSize);
   __declspec(dllexport) void ClientWrapper_SendCustomEvent(ClientWrapper* wrapper, const char* AgentId, const char* Name);

   __declspec(dllexport) void ClientWrapper_CancelResponse(ClientWrapper* wrapper, const char* AgentId, const char* InteractionId, const char** UtteranceIds, int UtteranceIdsCount);

   __declspec(dllexport) void ClientWrapper_StartAudioSession(ClientWrapper* wrapper, const char* AgentId);
   __declspec(dllexport) void ClientWrapper_StopAudioSession(ClientWrapper* wrapper, const char* AgentId);

   __declspec(dllexport) void ClientWrapper_InitClient(ClientWrapper* wrapper, const char* UserId, const char* ClientId, const char* ClientVer, ConnectionStateCallbackType ConnectionStateCallback, PacketCallbackType PacketCallback);
   __declspec(dllexport) void ClientWrapper_StartClientWithCallback(ClientWrapper* wrapper, const uint8_t* serialized_options, int serialized_options_size, const uint8_t* serialized_sessionInfo, int serialized_sessionInfo_size, LoadSceneCallbackType LoadSceneCallback);
   __declspec(dllexport) void ClientWrapper_PauseClient(ClientWrapper* wrapper);
   __declspec(dllexport) void ClientWrapper_ResumeClient(ClientWrapper* wrapper);
   __declspec(dllexport) void ClientWrapper_StopClient(ClientWrapper* wrapper);
   __declspec(dllexport) void ClientWrapper_DestroyClient(ClientWrapper* wrapper);

   //__declspec(dllexport) ConnectionState GetConnectionState(ClientWrapper* wrapper);
   __declspec(dllexport) bool ClientWrapper_GetConnectionError(ClientWrapper* wrapper, char* OutErrorMessage, int BufferSize, int32_t* OutErrorCode);
    
   __declspec(dllexport) void ClientWrapper_Update(ClientWrapper* wrapper);
   __declspec(dllexport) void DebugLog(const char* message);

}