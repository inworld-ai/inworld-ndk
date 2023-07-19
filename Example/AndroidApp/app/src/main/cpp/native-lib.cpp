#include <jni.h>
#include <string>
#include "Client.h"
#include "Utils/Log.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_inworldexample_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

Inworld::Client* InworldClient = nullptr;

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_inworldexample_MainActivity_InworldLoadScene(JNIEnv* env, jobject /* this */)
{
    Inworld::ClientOptions Options;

    Options.AuthUrl = "api-studio.inworld.ai";
    Options.LoadSceneUrl = "api-engine.inworld.ai:443";
    Options.PlayerName = "Player";

    Options.SceneName = "workspaces/artem_v_test/scenes/demo";
    Options.ApiKey = "MGhJJ0I9PyX7T53Ua70RePz2zbAgoINp";
    Options.ApiSecret = "uoKoKBXD6zeLjgihbWHdgV3fzwS6DB0TEXlBUpuW4ts3LCzcWYxxA2VGT6SsC8RR";

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

    InworldClient = new Inworld::Client();
    InworldClient->InitClient(
            "DefaultUserNDK",
            "DefaultClientNDK",
            "1.0.0",
            [](Inworld::Client::ConnectionState ConnectionState)
            {
                std::string Error;
                int32_t Code;
                InworldClient->GetConnectionError(Error, Code);

                Inworld::Log("Connection state: {0}. Error: {1}", static_cast<uint32_t>(ConnectionState), Error);

                if (ConnectionState == Inworld::Client::ConnectionState::Disconnected)
                {
                    InworldClient->ResumeClient();
                }
            },
            [](std::shared_ptr<Inworld::Packet> Packet)
            {
                //Packet->Accept();
            }
    );

    Inworld::SessionInfo SessionInfo;
    InworldClient->StartClient(Options, SessionInfo,
                        [](std::vector<Inworld::AgentInfo> AgentInfos)
                        {

                        });

    std::string hello = "InworldClient start...";
    return env->NewStringUTF(hello.c_str());
}