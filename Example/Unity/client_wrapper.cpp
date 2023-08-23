/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */
#include "client_wrapper.h"
#include <memory>

#include "Utils/Log.h"
using namespace inworld::ndkData;

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
    void ClientWrapper_InitClient(ClientWrapper* wrapper, const char* ClientId, const char* ClientVer, ConnectionStateCallbackType ConnectionStateCallback, PacketCallbackType PacketCallback, LogCallbackType LogCallback, SessionTokenCallbackType SessionTokenCallback) {
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

        wrapper->client.UnitySessionTokenCallback.operator=([SessionTokenCallback](Inworld::SessionInfo info) {
            std::string jsonString = SerializeSessionInfo(info);
    
            // Convert the std::string to a null-terminated C-string for interop.
            char* serialized_data = static_cast<char*>(CoTaskMemAlloc(jsonString.size() + 1)); // +1 for null terminator
            strcpy(serialized_data, jsonString.c_str());

            SessionTokenCallback(serialized_data);
        });
        
        Inworld::LogSetUnityLogCallback(LogCallback);
        DebugLog("INITIALIZED CLIENT FROM DLL");
    }

    // StartClient
    void ClientWrapper_StartClientWithCallback(ClientWrapper* wrapper, const uint8_t* serialized_options, int serialized_options_size, const uint8_t* serialized_sessionInfo, int serialized_sessionInfo_size, LoadSceneCallbackType LoadSceneCallback) {
        DebugLog("ABOUT TO START CLIENT with callback begin deserializing options");

        SessionInfo sessionInfo;
        if(!sessionInfo.ParseFromArray(serialized_sessionInfo, serialized_sessionInfo_size))
        {
            // Handle deserialization error
            DebugLog("START CLIENT FAILED TO DESERIALIZE SESSION INFO");
            return;
        }

        // Deserialize ClientOptions from byte array
        ClientOptions options;
        if (!options.ParseFromArray(serialized_options, serialized_options_size))
        {
            // Handle deserialization error
            DebugLog("START CLIENT FAILED TO DESERIALIZE CLIENT OPTIONS");
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
            DebugLog("NO CAPABILITIES SET IN THE CLIENT OPTIONS");
        }

        Inworld::SessionInfo session_info;
        session_info.Token = sessionInfo.token();
        session_info.ExpirationTime = sessionInfo.expirationtime();
        session_info.SessionId = sessionInfo.sessionid();

        DebugLog("STARTing CLIENT succesful deserialization of OPTIONS AND SESSION INFO with callback");
        // Start the client
        wrapper->client.StartClient(opt, session_info,
                                    [LoadSceneCallback](const std::vector<Inworld::AgentInfo>& AgentInfos)
                                    {
                                        if (AgentInfos.empty())
                                        {
                                            DebugLog("AGENT INFOS IS EMPTY IN LOADSCENE CALLBACK");
                                        }
                                        else if (LoadSceneCallback != nullptr)
                                        {
                                            AgentInfoArray agent_info_array;

                                            // Fill agent_info_array with data from AgentInfos vector
                                            for (const auto& agent_info : AgentInfos)
                                            {
                                                AgentInfo* added_agent_info = agent_info_array.add_agent_info_list();
                                                //*added_agent_info = agent_info;
                                                added_agent_info->set_agentid(agent_info.AgentId);
                                                added_agent_info->set_brainname(agent_info.BrainName);
                                                added_agent_info->set_givenname(agent_info.GivenName);
                                            }

                                            int serialized_data_size = agent_info_array.ByteSize();
                                            /*unsigned char* serialized_data = new unsigned char[serialized_data_size];
                                            agent_info_array.SerializeToArray(serialized_data, serialized_data_size);*/

                                            unsigned char* serialized_data = static_cast<uint8_t*>(CoTaskMemAlloc(
                                                serialized_data_size));
                                            agent_info_array.SerializeToArray(serialized_data, serialized_data_size);


                                            std::ostringstream ss;
                                            ss << "Serialized_agent_info_array size: " << serialized_data_size;
                                            DebugLog(ss.str().c_str());

                                            if (serialized_data_size < 0)
                                            {
                                                DebugLog("AGENT INFO SIZE LESS THAN 0 AND IS INVALID");
                                                return;
                                            }
                                            LoadSceneCallback(serialized_data, serialized_data_size);
                                        }
                                    });
        DebugLog("STARTED CLIENT FROM DLL");
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


