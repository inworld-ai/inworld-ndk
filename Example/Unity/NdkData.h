#pragma once
#include <string>
#include <vector>
#include <ctime>

#include <cstdint>

const int MAX_STRING_SIZE = 256;
const int MAX_FIELDS_COUNT = 50;
const int MAX_AGENTINFO_COUNT = 100;

class Token 
{
public:
    char token[MAX_STRING_SIZE];
    char type[MAX_STRING_SIZE];
    int64_t expirationTime;
    char sessionId[MAX_STRING_SIZE];
    bool isValid;
};

class UserRequest 
{
public:
    char name[MAX_STRING_SIZE];
};

class Fields 
{
public:
    char fieldId[MAX_STRING_SIZE];
    char fieldValue[MAX_STRING_SIZE];
};

class PlayerProfile 
{
public:
    Fields fields[MAX_FIELDS_COUNT];
    int32_t fieldsCount = 0;
};

class UserSettings 
{
public:
    bool viewTranscriptConsent;
    PlayerProfile playerProfile;
};

class ClientRequest 
{
public:
    char id[MAX_STRING_SIZE];
    char version[MAX_STRING_SIZE];
};

class CapabilitiesRequest 
{
public:
    bool audio;
    bool emotions;
    bool interruptions;
    bool narratedActions;
    bool silence;
    bool text;
    bool triggers;
    bool continuation;
    bool turnBasedStt;
    bool phonemeInfo;
    bool animations;
    bool gestures;
    bool emotionStreaming;
    bool silenceEvents;
    bool loadSceneInSession;
};

class CharacterAssets 
{
public:
    char rpmModelUri[MAX_STRING_SIZE];
    char rpmImageUriPortrait[MAX_STRING_SIZE];
    char rpmImageUriPosture[MAX_STRING_SIZE];
    char avatarImg[MAX_STRING_SIZE];
    char avatarImgOriginal[MAX_STRING_SIZE];
};

class InworldCharacterData 
{
public:
    char agentId[MAX_STRING_SIZE];
    char brainName[MAX_STRING_SIZE];
    char givenName[MAX_STRING_SIZE];
    CharacterAssets characterAssets;
    char thumbnail[MAX_STRING_SIZE];
};

class ClientOptions 
{
public:
    char serverUrl[MAX_STRING_SIZE];
    char sceneName[MAX_STRING_SIZE];
    char apiKey[MAX_STRING_SIZE];
    char apiSecret[MAX_STRING_SIZE];
    char playerName[MAX_STRING_SIZE];
    char token[MAX_STRING_SIZE];
    char sessionId[MAX_STRING_SIZE];
    char userId[MAX_STRING_SIZE];
    CapabilitiesRequest capabilities;
};

class LoadSceneRequest 
{
public:
    ClientRequest client;
    UserRequest user;
    CapabilitiesRequest capabilities;
    UserSettings userSetting;
};

enum ConnectionState : int32_t 
{
    Idle = 0,
    Connecting = 1,
    Connected = 2,
    Failed = 3,
    Paused = 4,
    Disconnected = 5,
    Reconnecting = 6
};

class AgentInfo 
{
public:
    char brainName[MAX_STRING_SIZE];
    char agentId[MAX_STRING_SIZE];
    char givenName[MAX_STRING_SIZE];
};

class SessionInfo 
{
public:
    char sessionId[MAX_STRING_SIZE];
    char token[MAX_STRING_SIZE];
    int64_t expirationTime;
    bool isValid;
};

class AgentInfoArray 
{
public:
    AgentInfo agentInfoList[MAX_AGENTINFO_COUNT];
    int32_t agentInfoCount = 0;
};

// class Token 
// {
// public:
//     std::string token;
//     std::string type;
//     time_t expirationTime;
//     std::string sessionId;
//     bool isValid;
// };
//
// class UserRequest 
// {
// public:
//     std::string name;
// };
//
// class Fields 
// {
// public:
//     std::string fieldId;
//     std::string fieldValue;
// };
//
// class PlayerProfile 
// {
// public:
//     std::vector<Fields> fields;
// };
//
// class UserSettings 
// {
// public:
//     bool viewTranscriptConsent;
//     PlayerProfile playerProfile;
// };
//
// class ClientRequest 
// {
// public:
//     std::string id;
//     std::string version;
// };
//
// class CapabilitiesRequest 
// {
// public:
//     bool audio;
//     bool emotions;
//     bool interruptions;
//     bool narratedActions;
//     bool silence;
//     bool text;
//     bool triggers;
//     bool continuation;
//     bool turnBasedStt;
//     bool phonemeInfo;
//     bool animations;
//     bool gestures;
//     bool emotionStreaming;
//     bool silenceEvents;
//     bool loadSceneInSession;
// };
//
// class CharacterAssets 
// {
// public:
//     std::string rpmModelUri;
//     std::string rpmImageUriPortrait;
//     std::string rpmImageUriPosture;
//     std::string avatarImg;
//     std::string avatarImgOriginal;
// };
//
// class InworldCharacterData 
// {
// public:
//     std::string agentId;
//     std::string brainName;
//     std::string givenName;
//     CharacterAssets characterAssets;
//     std::vector<char> thumbnail;
// };
//
// class ClientOptions 
// {
// public:
//     std::string serverUrl;
//     std::string sceneName;
//     std::string apiKey;
//     std::string apiSecret;
//     std::string playerName;
//     std::string token;
//     std::string sessionId;
//     std::string userId;
//     CapabilitiesRequest capabilities;
// };
//
// class LoadSceneRequest 
// {
// public:
//     ClientRequest client;
//     UserRequest user;
//     CapabilitiesRequest capabilities;
//     UserSettings userSetting;
// };
//
// enum ConnectionState 
// {
//     Idle = 0,
//     Connecting = 1,
//     Connected = 2,
//     Failed = 3,
//     Paused = 4,
//     Disconnected = 5,
//     Reconnecting = 6
// };
//
// class AgentInfo 
// {
// public:
//     AgentInfo() = default; // Default constructor
//     AgentInfo(const std::string& aId, const std::string& bName, const std::string& gName)
//         : agentId(aId), brainName(bName), givenName(gName) {}
//
//     std::string brainName;
//     std::string agentId;
//     std::string givenName;
// };
//
//
// class SessionInfo 
// {
// public:
//     std::string sessionId;
//     std::string token;
//     int64_t expirationTime;
//     bool isValid;
// };
//
// class AgentInfoArray 
// {
// public:
//     std::vector<AgentInfo> agentInfoList;
// };
