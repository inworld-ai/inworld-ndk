#include <jni.h>
#include <string>
#include "Client.h"
#include "Packets.h"
#include "Utils/Log.h"

class TextMessageVisitor : public Inworld::PacketVisitor
{
    virtual void Visit(const Inworld::TextEvent& Event)
    {
        if (Event.IsFinal()) {
            Inworld::Log("%s: %s", Event._Routing._Source._Name.c_str(), Event.GetText().c_str());
        }
    }
};

Inworld::Client InworldClient;
TextMessageVisitor MessageHandler;

extern "C" JNIEXPORT void JNICALL
Java_com_example_inworldexample_MainActivity_InworldStartClient(JNIEnv* env, jobject /* this */)
{
    Inworld::ClientOptions Options;

    Options.AuthUrl = "api-studio.inworld.ai";
    Options.LoadSceneUrl = "api-engine.inworld.ai:443";
    Options.PlayerName = "Player";

    // Fill this values!
    Options.SceneName = "workspaces/artem_v_test/scenes/demo";
    Options.ApiKey = "MGhJJ0I9PyX7T53Ua70RePz2zbAgoINp";
    Options.ApiSecret = "uoKoKBXD6zeLjgihbWHdgV3fzwS6DB0TEXlBUpuW4ts3LCzcWYxxA2VGT6SsC8RR";

    if (Options.SceneName.empty() ||
            Options.ApiKey.empty() ||
            Options.ApiSecret.empty())
    {
        Inworld::LogError("Fill data in native-lib.cpp");
    }

    Options.Capabilities.Animations = false;
    Options.Capabilities.Text = true;
    Options.Capabilities.Audio = true;
    Options.Capabilities.Emotions = true;
    Options.Capabilities.Gestures = true;
    Options.Capabilities.Interruptions = true;
    Options.Capabilities.Triggers = true;
    Options.Capabilities.EmotionStreaming = true;
    Options.Capabilities.SilenceEvents = true;
    Options.Capabilities.PhonemeInfo = true;
    Options.Capabilities.LoadSceneInSession = true;

    std::vector<Inworld::AgentInfo> AgentInfos;

    //InworldClient = std::make_unique<Inworld::Client>();
    InworldClient.InitClient(
            "DefaultAndroidUserNDK",
            "DefaultAndroidClientNDK",
            "1.0.0",
            [](Inworld::Client::ConnectionState ConnectionState)
            {
                std::string Error;
                int32_t Code;
                InworldClient.GetConnectionError(Error, Code);

                Inworld::Log("Connection state: {0}. Error: {1}", static_cast<uint32_t>(ConnectionState), Error);

                if (ConnectionState == Inworld::Client::ConnectionState::Disconnected)
                {
                    InworldClient.ResumeClient();
                }
            },
            [](std::shared_ptr<Inworld::Packet> Packet)
            {
                Packet->Accept(MessageHandler);
            }
    );

    Inworld::SessionInfo SessionInfo;
    InworldClient.StartClient(Options, SessionInfo,
                        [](std::vector<Inworld::AgentInfo> AgentInfos)
                        {
                            if (!AgentInfos.empty())
                            {
                                InworldClient.SendTextMessage(AgentInfos[0].AgentId, "Hello");
                            }
                        });
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_inworldexample_MainActivity_InworldUpdateClient(JNIEnv* env, jobject /* this */)
{
    InworldClient.Update();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_inworldexample_MainActivity_InworldStopClient(JNIEnv* env, jobject /* this */)
{
    InworldClient.StopClient();
    InworldClient.DestroyClient();
}