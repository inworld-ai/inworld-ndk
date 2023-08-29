/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */
#include "client_wrapper.h"
#include <memory>

#include "Utils/Log.h"
#include "NdkData.h"
//using namespace inworld::ndkData;

extern "C" {

    // ClientWrapper instantiation and destruction
    ClientWrapper* ClientWrapper_create() {
        return new ClientWrapper();
    }

    void ClientWrapper_destroy(ClientWrapper* wrapper) {
        DebugLog("DESTROYING CLIENT WRAPPER FROM DLL");
        delete wrapper;
    }

    // InitClient
    void ClientWrapper_InitClient(ClientWrapper* wrapper, const char* ClientId, const char* ClientVer, ConnectionStateCallbackType ConnectionStateCallback, PacketCallbackType PacketCallback, LogCallbackType LogCallback) {
        wrapper->client.InitClient(ClientId, ClientVer,
            [ConnectionStateCallback](Inworld::Client::ConnectionState ConnectionState) {
                ConnectionStateCallback(static_cast<int>(ConnectionState));
            },
            [PacketCallback](std::shared_ptr<Inworld::Packet> Packet) {
                if (Packet != nullptr)
                {
                    InworldPakets::InworldPacket pkt = Packet.get()->ToProto();                    

                    int serialized_data_size = pkt.ByteSize();
                    unsigned char* serialized_data = static_cast<unsigned char*>(CoTaskMemAlloc(serialized_data_size));
                    pkt.SerializeToArray(serialized_data, serialized_data_size);

                    if (serialized_data_size < 0)
                    {
                        DebugLog("AGENT INFO SIZE LESS THAN 0 AND IS INVALID");
                        return;
                    }
                    PacketCallback(serialized_data, serialized_data_size);
                }
            });
        
        Inworld::LogSetUnityLogCallback(LogCallback);
        DebugLog("INITIALIZED CLIENT FROM DLL");
    }

    static void HandleAgentInfos(const std::vector<Inworld::AgentInfo>& AgentInfos, LoadSceneCallbackType LoadSceneCallback) {
        if (AgentInfos.empty())
        {
            Inworld::LogWarning("AGENT INFOS IS EMPTY IN LOADSCENE CALLBACK");
        }
        else if (LoadSceneCallback != nullptr)
        {
            AgentInfoArray agent_info_array;

            // Fill agent_info_array with data from AgentInfos vector
            int32_t agentIndex = 0; // Keep track of how many agents we've added to avoid out-of-bounds access
            for (const auto& agent_info : AgentInfos)
            {
                if (agentIndex >= MAX_AGENTINFO_COUNT)
                {
                    Inworld::LogError("MAX_AGENTINFO_COUNT exceeded. Some agent info may be discarded.");
                    break; // We've reached the max allowed agents in the array
                }
                strncpy_s(agent_info_array.agentInfoList[agentIndex].agentId, agent_info.AgentId.c_str(),
                        MAX_STRING_SIZE);
                strncpy_s(agent_info_array.agentInfoList[agentIndex].brainName, agent_info.BrainName.c_str(),
                        MAX_STRING_SIZE);
                strncpy_s(agent_info_array.agentInfoList[agentIndex].givenName, agent_info.GivenName.c_str(),
                        MAX_STRING_SIZE);
                agent_info_array.agentInfoCount++;
                agentIndex++;
            }

            // Serialize agent_info_array
            std::vector<uint8_t> serialized_data_vector;
            for (int32_t i = 0; i < agent_info_array.agentInfoCount; ++i)
            {
                const AgentInfo& agent_info = agent_info_array.agentInfoList[i];

                auto serializedAgentId = SerializeString(agent_info.agentId);
                auto serializedBrainName = SerializeString(agent_info.brainName);
                auto serializedGivenName = SerializeString(agent_info.givenName);

                serialized_data_vector.insert(serialized_data_vector.end(), serializedAgentId.begin(),
                                              serializedAgentId.end());
                serialized_data_vector.insert(serialized_data_vector.end(), serializedBrainName.begin(),
                                              serializedBrainName.end());
                serialized_data_vector.insert(serialized_data_vector.end(), serializedGivenName.begin(),
                                              serializedGivenName.end());
            }

            int serialized_data_size = static_cast<int>(serialized_data_vector.size());

            std::ostringstream ss;
            ss << "Serialized_agent_info_array size: " << serialized_data_size;
            Inworld::Log(ss.str().c_str());

            if (serialized_data_size <= 0) // Added = check because size shouldn't be 0 either
            {
                Inworld::LogError("AGENT INFO SIZE LESS THAN OR EQUAL TO 0 AND IS INVALID");
                return;
            }
            LoadSceneCallback(serialized_data_vector.data(), serialized_data_size);

        }
    }
    
    // StartClient
    void ClientWrapper_StartClientWithCallback(ClientWrapper* wrapper, const uint8_t* serialized_options, int serialized_options_size, const uint8_t* serialized_sessionInfo, int serialized_sessionInfo_size, LoadSceneCallbackType LoadSceneCallback, SessionTokenCallbackType SessionTokenCallback) {
        SessionInfo sessionInfo;
        if(!sessionInfo.ParseFromArray(serialized_sessionInfo, serialized_sessionInfo_size))
        {
            // Handle deserialization error
            Inworld::LogError("START CLIENT FAILED TO DESERIALIZE SESSION INFO");
            return;
        }

        // Deserialize ClientOptions from byte array
        ClientOptions options;
        if (!options.ParseFromArray(serialized_options, serialized_options_size))
        {
            // Handle deserialization error
            Inworld::LogError("START CLIENT FAILED TO DESERIALIZE CLIENT OPTIONS");
            return;
        }

        Inworld::ClientOptions opt;
        opt.ApiKey = options.api_key();
        opt.ApiSecret = options.api_secret();
        opt.ServerUrl = options.server_url();
        opt.PlayerName = options.player_name();
        opt.UserId = options.user_id();
        opt.SceneName = options.scene_name();
        if (options.has_capabilities())
        {
            opt.Capabilities.Animations = options.capabilities().animations();
            opt.Capabilities.Audio = options.capabilities().audio();
            opt.Capabilities.Text = options.capabilities().text();
            opt.Capabilities.Emotions = options.capabilities().emotions();
            opt.Capabilities.Gestures = options.capabilities().gestures();
            opt.Capabilities.Interruptions = options.capabilities().interruptions();
            opt.Capabilities.Triggers = options.capabilities().triggers();
            opt.Capabilities.EmotionStreaming = options.capabilities().emotionstreaming();
            opt.Capabilities.SilenceEvents = options.capabilities().silenceevents();
            opt.Capabilities.PhonemeInfo = options.capabilities().phonemeinfo();
            opt.Capabilities.LoadSceneInSession = options.capabilities().loadsceneinsession();
            opt.Capabilities.TurnBasedSTT = options.capabilities().turnbasedstt();
            opt.Capabilities.NarratedActions = options.capabilities().narratedactions();
            opt.Capabilities.Continuation = options.capabilities().continuation();
        }
        else
        {
            Inworld::Log("NO CAPABILITIES SET IN THE CLIENT OPTIONS");
        }

        wrapper->client.SetOptions(opt);

        Inworld::SessionInfo session_info;
        session_info.Token = sessionInfo.token();
        session_info.ExpirationTime = sessionInfo.expirationtime();
        session_info.SessionId = sessionInfo.sessionid();

        
        auto agentInfosHandler = [LoadSceneCallback](const std::vector<Inworld::AgentInfo>& AgentInfos) {
            HandleAgentInfos(AgentInfos, LoadSceneCallback);
        };
        
        if(session_info.IsValid())
        {
            Inworld::Log("Session info is valid in NDK, starting client");
            wrapper->client.StartClient(opt, session_info, agentInfosHandler);
        }
        else
        {
            Inworld::Log("Session info is invalid in NDK, starting client");
            wrapper->client.GenerateToken([SessionTokenCallback, wrapper, opt, agentInfosHandler]()
            {
                const auto& info = wrapper->client.GetSessionInfo();
                std::string jsonString = SerializeSessionInfo(info);

                // Convert the std::string to a null-terminated C-string for interop.
                char* serialized_data = static_cast<char*>(CoTaskMemAlloc(jsonString.size() + 1)); // +1 for null terminator
                strcpy(serialized_data, jsonString.c_str());

                SessionTokenCallback(serialized_data);
                
                // Start the client
                wrapper->client.StartClient(opt, info, agentInfosHandler);
            });
        }
        Inworld::Log("STARTED CLIENT FROM DLL");
    }
    
    //Cancel Response 
    //This function takes a ClientWrapper pointer, two const char* pointers for agent_id() and InteractionId, 
    //a const char** pointer for an array of UtteranceIds, 
    //and an int for the count of UtteranceIds.
    //It converts the C - style strings and array into their C++ equivalents(std::string and std::vector<std::string>), 
    //and then calls the CancelResponse method on the Client instance within the ClientWrapper.
    void ClientWrapper_CancelResponse(ClientWrapper* wrapper, const char* AgentId, const char* InteractionId, const char** UtteranceIds, int UtteranceIdsCount) {
        std::string cppagent_id(AgentId);
        std::string cppInteractionId(InteractionId);
        std::vector<std::string> cppUtteranceIds;
        cppUtteranceIds.reserve(UtteranceIdsCount);

        for (int i = 0; i < UtteranceIdsCount; i++) {
            cppUtteranceIds.emplace_back(UtteranceIds[i]);
        }

        wrapper->client.CancelResponse(cppagent_id, cppInteractionId, cppUtteranceIds);
    }

    // SendPacket
    void ClientWrapper_SendPacket(ClientWrapper* wrapper, const void* data, size_t size) {
        // Deserialize the byte array to an InworldPakets::InworldPacket
        InworldPakets::InworldPacket packet;

        // ParseFromArray returns false if deserialization failed (e.g., corrupted data)
        if (!packet.ParseFromArray(data, size)) {
            // Handle deserialization error
            DebugLog("Failed to deserialize InworldPacket before sending");
            return;
        }
        
        std::shared_ptr<Inworld::Packet> pkt = nullptr;
        std::string type = "unknown";
        // TextEvent
        if (packet.has_text())
        {
            pkt = std::make_shared<Inworld::TextEvent>(packet);
        }
        // AudioDataEvent
        else if (packet.has_data_chunk() && packet.data_chunk().type() == InworldPakets::DataChunk_DataType_AUDIO)
        {
            pkt = std::make_shared<Inworld::AudioDataEvent>(packet);
        }
        // ControlEvent
        else if (packet.has_control())
        {
            pkt = std::make_shared<Inworld::ControlEvent>(packet);            
        }
        // CancelResponseEvent
        else if (packet.has_cancelresponses())
        {
            auto pkt = std::make_shared<Inworld::CancelResponseEvent>();            
        }
        // CustomEvent
        else if (packet.has_custom())
        {
            pkt = std::make_shared<Inworld::CustomEvent>(packet);            
        }
        else
        {
            //std::make_shared<Inworld::Packet>(packet);
            DebugLog("Unsupported outgoing event: ");
            DebugLog(packet.DebugString().c_str());
        }

        if(pkt)
        {
            type = pkt.get()->ToProto().GetTypeName();
            wrapper->client.SendPacket(pkt);
        }
        // commented to avoid spam/clutter
        // leaving in case of future debugging in future
        // std::ostringstream ss;
        // ss << "DLL SENDING A PACKET FROM THE DLL OF TYPE " << type;
        // DebugLog(ss.str().c_str());
    }


    // PauseClient
    void ClientWrapper_PauseClient(ClientWrapper* wrapper) {
        wrapper->client.PauseClient();
    }

    // ResumeClient
    void ClientWrapper_ResumeClient(ClientWrapper* wrapper) {
        wrapper->client.ResumeClient();
    }

    // StopClient
    void ClientWrapper_StopClient(ClientWrapper* wrapper) {
        wrapper->client.StopClient();
    }

    // DestroyClient
    void ClientWrapper_DestroyClient(ClientWrapper* wrapper) {
        wrapper->client.DestroyClient();
    }

    // SendTextMessage
    void ClientWrapper_SendTextMessage(ClientWrapper* wrapper, const char* AgentId, const char* Text) {
        wrapper->client.SendTextMessage(AgentId, Text);
    }

    // SendSoundMessage
    void ClientWrapper_SendSoundMessage(ClientWrapper* wrapper, const char* AgentId, const uint8_t* Data, int data_size) {
        std::vector<uint8_t> byteArray(Data, Data + data_size);
        std::string audioData(byteArray.begin(), byteArray.end());
        wrapper->client.SendSoundMessage(AgentId, audioData);
    }

    // SendSoundMessageWithAEC
    void ClientWrapper_SendSoundMessageWithAEC(ClientWrapper* wrapper, const char* AgentId, const int16_t* InputData, int InputDataSize, const int16_t* OutputData, int OutputDataSize) {
        std::vector<int16_t> inputVector(InputData, InputData + InputDataSize);
        std::vector<int16_t> outputVector(OutputData, OutputData + OutputDataSize);
        wrapper->client.SendSoundMessageWithAEC(AgentId, inputVector, outputVector);
    }

    // Leaving commented in case protobufs are removed in the future currently already accounted for through
    // SendPacket
    // // SendCustomEvent
    // void ClientWrapper_SendCustomEvent(ClientWrapper* wrapper, const char* AgentId, const char* Name) {
    //     wrapper->client.SendCustomEvent(AgentId, Name);
    // }


    // StartAudioSession
    void ClientWrapper_StartAudioSession(ClientWrapper* wrapper, const char* AgentId) {
        wrapper->client.StartAudioSession(AgentId);
    }

    // StopAudioSession
    void ClientWrapper_StopAudioSession(ClientWrapper* wrapper, const char* AgentId) {
        wrapper->client.StopAudioSession(AgentId);
    }

    // GetConnectionError
    bool ClientWrapper_GetConnectionError(ClientWrapper* wrapper, char* OutErrorMessage, int BufferSize, int32_t* OutErrorCode) {
        std::string errorMessage;
        int32_t errorCode;
        bool result = wrapper->client.GetConnectionError(errorMessage, errorCode);

        if(!result)
            return false;
        
        strncpy(OutErrorMessage, errorMessage.c_str(), BufferSize - 1);
        OutErrorMessage[BufferSize - 1] = '\0';
        *OutErrorCode = errorCode;

        return result;
    }


    // FixedUpdate
    void ClientWrapper_Update(ClientWrapper* wrapper) {
        wrapper->client.Update();
    }

    void ClientWrapper_SetAudioDumpEnabled(ClientWrapper* wrapper, bool enabled, const char* FilePath)
    {
        wrapper->client.SetAudioDumpEnabled(enabled, FilePath);
    }
    
    void DebugLog(const char* message)
    {
        std::cout << message << std::endl;
    }

}


