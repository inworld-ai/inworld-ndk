#include <jni.h>
#include <string>
#include <sstream>
#include "Public/Client.h"
#include "Public/Packets.h"
#include "Public/Utils/Log.h"

class InworldVisitor : public Inworld::PacketVisitor
{
    Inworld::Client &InworldClient;

public:
    explicit InworldVisitor(Inworld::Client &inworldClient)
            : InworldClient(inworldClient) {}

private:
        void Visit(const Inworld::TextEvent& Event) override
        {
            if (Event.IsFinal()) {
                Inworld::Log("%s: %s", Event._Routing._Source._Name.c_str(), Event.GetText().c_str());
            }
        }

        void Visit(const Inworld::ControlEventCurrentSceneStatus& Event) override {
            const std::vector<Inworld::AgentInfo>& AgentInfos = Event.GetAgentInfos();

            if (!AgentInfos.empty())
            {
                // Send a message to the first agent in the list
                InworldClient.SendTextMessage(AgentInfos[0].AgentId, "Hello");
            }
    }
};

Inworld::Client InworldClient;
InworldVisitor MessageHandler(InworldClient);

extern "C" JNIEXPORT void JNICALL
Java_com_example_inworldexample_MainActivity_InworldStartClient(JNIEnv* env, jobject /* this */)
{
    Inworld::ClientOptions Options;

    Options.Capabilities.Animations = false;
    Options.Capabilities.Audio = true;
    Options.Capabilities.Emotions = true;
    Options.Capabilities.Interruptions = true;

    Options.UserConfig.Name = "Player";

    Options.ServerUrl = "api-engine.inworld.ai:443";

    // Fill this values!
    Options.Resource = "";
    Options.ApiKey = "";
    Options.ApiSecret = "";
    std::string SceneID = "";

    if (Options.Resource.empty() ||
            Options.ApiKey.empty() ||
            Options.ApiSecret.empty() ||
            SceneID.empty())
    {
        Inworld::LogError("Fill data in native-lib.cpp");
    }

    InworldClient.SetOptions(Options);

    Inworld::SdkInfo Info;
    Info.Type = "DefaultAndroidUserNDK";
    Info.Subtype =  "DefaultAndroidClientNDK";
    Info.Version = "1.0.0";

    InworldClient.InitClientAsync(Info,
            [](Inworld::Client::ConnectionState ConnectionState)
            {
                std::string Error;
                int32_t Code;
                Inworld::ErrorDetails ErrorDetails;
                InworldClient.GetConnectionError(Error, Code, ErrorDetails);

                Inworld::Log("Connection state: %u. %s", ConnectionState, Error.c_str());

                if (ConnectionState == Inworld::Client::ConnectionState::Disconnected)
                {
                    InworldClient.ResumeClient();
                }
            },
            [](const std::shared_ptr<Inworld::Packet>& Packet)
            {
                Packet->Accept(MessageHandler);
            }
    );

    InworldClient.StartClientFromSceneId(Options.Resource + "/scenes/" + SceneID);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_inworldexample_MainActivity_InworldStopClient(JNIEnv* env, jobject /* this */)
{
    InworldClient.StopClient();
    InworldClient.DestroyClient();
}
