#include "client_wrapper.h"
#include <memory>

//Inworld::ClientBase::ConnectionState toCppConnectionState(ConnectionState state) {
//    return static_cast<Inworld::ClientBase::ConnectionState>(state);
//}

//Inworld::ClientOptions toCppClientOptions(const ClientOptions* options) {
//    Inworld::ClientOptions cppOptions;
//    cppOptions.AuthUrl = options->AuthUrl;
//    cppOptions.LoadSceneUrl = options->LoadSceneUrl;
//    cppOptions.SceneName = options->SceneName;
//    cppOptions.ApiKey = options->ApiKey;
//    cppOptions.ApiSecret = options->ApiSecret;
//    cppOptions.PlayerName = options->PlayerName;
//    cppOptions.Token = options->Token;
//    cppOptions.SessionId = options->SessionId;
//    return cppOptions;
//}
extern "C" {

  

  /*  ConnectionState toCConnectionState(Inworld::ClientBase::ConnectionState state) {
        return static_cast<ConnectionState>(state);
    }

    AgentInfoArray createAgentInfoArray(const std::vector<Inworld::AgentInfo>& agentInfos) {
        AgentInfoArray array;
        array.size = static_cast<int>(agentInfos.size());
        array.data = new AgentInfo[array.size];

        for (int i = 0; i < array.size; ++i) {
            array.data[i].brain_name() = new char[agentInfos[i].brain_name().size() + 1];
            strcpy(array.data[i].brain_name(), agentInfos[i].brain_name().c_str());

            array.data[i].agent_id() = new char[agentInfos[i].agent_id().size() + 1];
            strcpy(array.data[i].agent_id(), agentInfos[i].agent_id().c_str());

            array.data[i].given_name() = new char[agentInfos[i].given_name().size() + 1];
            strcpy(array.data[i].given_name(), agentInfos[i].given_name().c_str());
        }

        return array;
    }*/

   /* void destroyAgentInfoArray(AgentInfoArray* array) {
        for (int i = 0; i < array->size; ++i) {
            delete[] array->data[i].brain_name();
            delete[] array->data[i].agent_id();
            delete[] array->data[i].given_name();
        }

        delete[] array->data;
    }*/

    // ClientWrapper instantiation and destruction
    ClientWrapper* ClientWrapper_create() {
        return new ClientWrapper();
    }

    void ClientWrapper_destroy(ClientWrapper* wrapper) {
        DebugLog("DESTROYING CLIENT WRAPPER FROM DLL");
        delete wrapper;
    }

    // InitClient
    void ClientWrapper_InitClient(ClientWrapper* wrapper, const char* UserId, const char* ClientId, const char* ClientVer, ConnectionStateCallbackType ConnectionStateCallback, PacketCallbackType PacketCallback) {
        wrapper->client.InitClient(UserId, ClientId, ClientVer,
            [ConnectionStateCallback](ConnectionState ConnectionState) {
                ConnectionStateCallback(static_cast<int>(ConnectionState));
            },
            [PacketCallback](std::shared_ptr<Inworld::Packet> Packet) {
                if (Packet != nullptr || Packet != NULL)
                {
                    DebugLog("INVOKING PACKET CALLBACK FROM DLL");
                    InworldPakets::InworldPacket pkt = Packet.get()->ToProto();

                    if (pkt.has_audio_chunk())
                    {
                        DebugLog("DLL Packet data audio chunk is not null");
                        if (pkt.audio_chunk().chunk().empty())
                        {
                            DebugLog("DLL Packet data audio chunk is not empty");// chunk is " + Packet->ToProto().audio_chunk().chunk());

                        }
                        else
                            DebugLog("DLL PACKET CHUNK IS EMPTY DESPITE VALID AUDIO CHUNK");
                    }

                    if (pkt.has_data_chunk())
                    {
                        DebugLog("DLL Packet has valid data chunk");
                        if (pkt.data_chunk().type() == InworldPakets::DataChunk_DataType_AUDIO)
                        {
                            DebugLog("Packet DATA CHUNK TYPE IS AUDIO in the DLL");
                            if (pkt.data_chunk().additional_phoneme_info().empty())
                                DebugLog("DLL Packet data chunk phonemes are empty");
                            else
                            {
                                DebugLog("DLL SHOULD HAVE RECEIVED VALID PHONEMES");
                            }
                        }
                    }

                    int serialized_data_size = pkt.ByteSize();
                    unsigned char* serialized_data = static_cast<unsigned char*>(CoTaskMemAlloc(serialized_data_size));
                    pkt.SerializeToArray(serialized_data, serialized_data_size);

                    std::ostringstream ss;
                    ss << "PACKET NDK callback packet size is " << serialized_data_size;
                    DebugLog(ss.str().c_str());

                    if (serialized_data_size < 0)
                    {
                        DebugLog("AGENT INFO SIZE LESS THAN 0 AND IS INVALID");
                        return;
                    }
                    PacketCallback(serialized_data, serialized_data_size);
                }
            });
        DebugLog("INITIALIZED CLIENT FROM DLL");
    }

    // StartClient
    void ClientWrapper_StartClientWithCallback(ClientWrapper* wrapper, const uint8_t* serialized_options, int serialized_options_size, const uint8_t* serialized_sessionInfo, int serialized_sessionInfo_size, LoadSceneCallbackType LoadSceneCallback) {
        DebugLog("ABOUT TO START CLIENT with callback begin deserializing options");

        // Deserialize ClientOptions from byte array
        ClientOptions options;
        if (!options.ParseFromArray(serialized_options, serialized_options_size)) {
            // Handle deserialization error
            DebugLog("START CLIENT FAILED TO DESRIALIZE OPTIONS with callback");
            return;
        }

        SessionInfo sessionInfo;
        if(!sessionInfo.ParseFromArray(serialized_sessionInfo, serialized_sessionInfo_size))
        {
            // Handle deserialization error
            DebugLog("START CLIENT FAILED TO DESRIALIZE session info with callback");
            return;
        }
        else
        {
            DebugLog("STARTing CLIENT succesful deserialization of OPTIONS AND SESSION INFO with callback");
            // Start the client
            wrapper->client.StartClient(options, sessionInfo, [LoadSceneCallback](const std::vector<AgentInfo>& AgentInfos) {
                if (AgentInfos.empty())
                {
                    DebugLog("AGENT INFOS IS EMPTY IN LOADSCENE CALLBACK");
                }
                else if (LoadSceneCallback != nullptr) {
                    AgentInfoArray agent_info_array;

                    // Fill agent_info_array with data from AgentInfos vector
                    for (const auto& agent_info : AgentInfos) {
                        AgentInfo* added_agent_info = agent_info_array.add_agent_info_list();
                        *added_agent_info = agent_info;
                    }

                    int serialized_data_size = agent_info_array.ByteSize();
                    /*unsigned char* serialized_data = new unsigned char[serialized_data_size];
                    agent_info_array.SerializeToArray(serialized_data, serialized_data_size);*/

                    unsigned char* serialized_data = static_cast<unsigned char*>(CoTaskMemAlloc(serialized_data_size));
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

                    // Remember to delete the allocated buffer when you're done using it.
                    //delete[] serialized_data;
                }
                });
            DebugLog("STARTed CLIENT with callback");

        }
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

        for (int i = 0; i < UtteranceIdsCount; i++) {
            cppUtteranceIds.push_back(std::string(UtteranceIds[i]));
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
            DebugLog("Failed to deserialize InworldPacket");
            return;
        }

        std::shared_ptr<Inworld::Packet> resolvedPacket = ResolvePackets(packet);
        wrapper->client.SendPacket(resolvedPacket);
        std::ostringstream ss;
        ss << "DLL SENDING A PACKET FROM THE DLL OF TYPE " << resolvedPacket.get()->ToProto().GetTypeName();
        DebugLog(ss.str().c_str());
    }

    std::shared_ptr<Inworld::Packet> ResolvePackets(InworldPakets::InworldPacket packet)
    {
        std::shared_ptr<Inworld::Packet> pkt = nullptr;

        // TextEvent
        if (packet.has_text())
        {
            pkt = std::make_shared<Inworld::TextEvent>(packet);

        }
        // AudioDataEvent
        else if (packet.has_data_chunk() && packet.data_chunk().type() == InworldPakets::DataChunk_DataType_AUDIO)
        {
            std::cout << "AUDIO DATA EVENT CREATED IN NDK" << std::endl;

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
            auto cre = std::make_shared<Inworld::CancelResponseEvent>();
            cre->ToProto();
            pkt = cre;
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

        return pkt;
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
    void ClientWrapper_SendSoundMessage(ClientWrapper* wrapper, const char* AgentId, const char* Data) {
        wrapper->client.SendSoundMessage(AgentId, Data);
    }

    // SendSoundMessageWithAEC
    void ClientWrapper_SendSoundMessageWithAEC(ClientWrapper* wrapper, const char* AgentId, const int16_t* InputData, int InputDataSize, const int16_t* OutputData, int OutputDataSize) {
        std::vector<int16_t> inputVector(InputData, InputData + InputDataSize);
        std::vector<int16_t> outputVector(OutputData, OutputData + OutputDataSize);
        wrapper->client.SendSoundMessageWithAEC(AgentId, inputVector, outputVector);
    }

    // SendCustomEvent
    void ClientWrapper_SendCustomEvent(ClientWrapper* wrapper, const char* AgentId, const char* Name) {
        wrapper->client.SendCustomEvent(AgentId, Name);
    }


    // StartAudioSession
    void ClientWrapper_StartAudioSession(ClientWrapper* wrapper, const char* AgentId) {
        wrapper->client.StartAudioSession(AgentId);
    }

    // StopAudioSession
    void ClientWrapper_StopAudioSession(ClientWrapper* wrapper, const char* AgentId) {
        wrapper->client.StopAudioSession(AgentId);
    }

    ConnectionState GetConnectionState(ClientWrapper* wrapper)
    {
        return (wrapper->client.GetConnectionState());
    }

    // GetConnectionError
    bool ClientWrapper_GetConnectionError(ClientWrapper* wrapper, char* OutErrorMessage, int BufferSize, int32_t* OutErrorCode) {
        std::string errorMessage;
        int32_t errorCode;
        bool result = wrapper->client.GetConnectionError(errorMessage, errorCode);

        strncpy(OutErrorMessage, errorMessage.c_str(), BufferSize - 1);
        OutErrorMessage[BufferSize - 1] = '\0';
        *OutErrorCode = errorCode;

        return result;
    }


    // FixedUpdate
    void ClientWrapper_Update(ClientWrapper* wrapper) {
        wrapper->client.Update();
    }

    void DebugLog(const char* message)
    {
        std::cout << message << std::endl;
    }

}