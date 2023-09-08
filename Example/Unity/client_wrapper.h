/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */
#define CLIENT_WRAPPER_H

#include <stdint.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <string.h>
#include "proto/inworld_ndkData.pb.h"
#include "Client.h"

extern "C" {

    enum class PacketType {
        NONE = 0, // Useful for indicating that no field is set
        TEXT = 2,
        CONTROL = 3,
        AUDIO_CHUNK = 4, // Note: This is deprecated in your proto definition
        CUSTOM = 8,
        CANCEL_RESPONSES = 10, // Note: This is deprecated in your proto definition
        EMOTION = 11,
        DATA_CHUNK = 12,
        ACTION = 13,
        MUTATION = 15,
        LOAD_SCENE_OUTPUT = 16,
        DEBUG_INFO = 18
    };
    
    struct ClientWrapper {
        Inworld::Client client;
    };

    //methods will be invoked in Unity
    typedef void (*ConnectionStateCallbackType)(int connectionState);
    //typedef void (*PacketCallbackType)(const char* packet, PacketType packetType);
    typedef void (*PacketCallbackType)(const uint8_t* packet, int length, PacketType packetSize);
    typedef void (*LoadSceneCallbackType)(const uint8_t* serialized_agent_info_array, int serialized_agent_info_array_size);
    typedef void (*SessionTokenCallbackType)(const char* sessionToken);
    typedef void (*LogCallbackType)(const char* message, int severity);
    //typedef void (*RefreshTokenCallbackType)();
    
    struct ClientWrapper;

   __declspec(dllexport) ClientWrapper* ClientWrapper_create();
   __declspec(dllexport) void ClientWrapper_destroy(ClientWrapper* wrapper);

   //possibly unneccessary creates circular dependencies with wrapping parameters and protobuf classes 
   __declspec(dllexport) void ClientWrapper_SendPacket(ClientWrapper* wrapper, const void* data, size_t size);

   __declspec(dllexport) void ClientWrapper_SendTextMessage(ClientWrapper* wrapper, const char* AgentId, const char* Text);
    __declspec(dllexport) void ClientWrapper_SendStringSoundMessage(ClientWrapper* wrapper, const char* AgentId, const char* Data);
   __declspec(dllexport) void ClientWrapper_SendSoundMessage(ClientWrapper* wrapper, const char* AgentId, const uint8_t* Data, int data_size);
   __declspec(dllexport) void ClientWrapper_SendSoundMessageWithAEC(ClientWrapper* wrapper, const char* AgentId, const int16_t* InputData, int InputDataSize, const int16_t* OutputData, int OutputDataSize);
   //__declspec(dllexport) void ClientWrapper_SendCustomEvent(ClientWrapper* wrapper, const char* AgentId, const char* Name);

   __declspec(dllexport) void ClientWrapper_CancelResponse(ClientWrapper* wrapper, const char* AgentId, const char* InteractionId, const char** UtteranceIds, int UtteranceIdsCount);

   __declspec(dllexport) void ClientWrapper_StartAudioSession(ClientWrapper* wrapper, const char* AgentId);
   __declspec(dllexport) void ClientWrapper_StopAudioSession(ClientWrapper* wrapper, const char* AgentId);
    
   __declspec(dllexport) void ClientWrapper_InitClient(ClientWrapper* wrapper, const char* ClientId, const char* ClientVer, ConnectionStateCallbackType ConnectionStateCallback, PacketCallbackType PacketCallback, LogCallbackType LogCallback, SessionTokenCallbackType SessionTokenCallback);
    __declspec(dllexport)void ClientWrapper_GenerateToken(ClientWrapper* wrapper, SessionTokenCallbackType SessionTokenCallback);

    __declspec(dllexport) void ClientWrapper_StartClientWithCallback(ClientWrapper* wrapper, const uint8_t* serialized_options, int serialized_options_size, const uint8_t* serialized_sessionInfo, int serialized_sessionInfo_size, LoadSceneCallbackType LoadSceneCallback);
   __declspec(dllexport) void ClientWrapper_PauseClient(ClientWrapper* wrapper);
   __declspec(dllexport) void ClientWrapper_ResumeClient(ClientWrapper* wrapper);
   __declspec(dllexport) void ClientWrapper_StopClient(ClientWrapper* wrapper);
   __declspec(dllexport) void ClientWrapper_DestroyClient(ClientWrapper* wrapper);

   __declspec(dllexport) bool ClientWrapper_GetConnectionError(ClientWrapper* wrapper, char* OutErrorMessage, int BufferSize, int32_t* OutErrorCode);
    
   __declspec(dllexport) void ClientWrapper_Update(ClientWrapper* wrapper);
   __declspec(dllexport) void DebugLog(const char* message);

#ifdef INWORLD_AUDIO_DUMP
    __declspec(dllexport) void ClientWrapper_SetAudioDumpEnabled(ClientWrapper* wrapper, bool enabled, const char* FilePath);
#endif
}

 std::string SerializeSessionInfo(const Inworld::SessionInfo& info) {
        std::stringstream ss;
        ss << info.SessionId << "|"
           << info.Token << "|"
           << info.SessionSavedState << "|"
           << info.ExpirationTime;
        return ss.str();
    }

    std::string SerializeProtoToJson(const InworldPakets::InworldPacket& packet) {
        std::string json_string;
        google::protobuf::util::MessageToJsonString(packet, &json_string);
        return json_string;
    }

    PacketType GetPacketType(const InworldPakets::InworldPacket& packet) {
        if (packet.has_text()) {
            return PacketType::TEXT;
        } else if (packet.has_control()) {
            return PacketType::CONTROL;
        // } else if (packet.has_audio_chunk()) { // Note: Deprecated
        //     return PacketType::AUDIO_CHUNK;
        } else if (packet.has_custom()) {
            return PacketType::CUSTOM;
        // } else if (packet.has_cancelresponses()) { // Note: Deprecated
        //     return PacketType::CANCEL_RESPONSES;
        } else if (packet.has_emotion()) {
            return PacketType::EMOTION;
        } else if (packet.has_data_chunk()) {
            return PacketType::DATA_CHUNK;
        } else if (packet.has_action()) {
            return PacketType::ACTION;
        } else if (packet.has_mutation()) {
            return PacketType::MUTATION;
        } else if (packet.has_load_scene_output()) {
            return PacketType::LOAD_SCENE_OUTPUT;
        } else if (packet.has_debug_info()) {
            return PacketType::DEBUG_INFO;
        } else {
            return PacketType::NONE; // Indicates no field is set
        }
    }

    std::string Base64Encode(const unsigned char* buffer, size_t length) {
        BIO *bio, *b64;
        BUF_MEM *bufferPtr;

        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);

        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        BIO_write(bio, buffer, length);
        BIO_flush(bio);
        BIO_get_mem_ptr(bio, &bufferPtr);
        BIO_set_close(bio, BIO_NOCLOSE);
        BIO_free_all(bio);

        std::string encodedData(bufferPtr->data, bufferPtr->length);
        return encodedData;
    }
