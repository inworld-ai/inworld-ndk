/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include <codecvt>
#include "UnityNDKInteropData.h"
#include "Utils/Log.h"

static constexpr char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int NDKUnity::CharToInt(char c)
{
	for(int i = 0; i < strlen(base64Chars); ++i)
	{
		if(base64Chars[i] == c)
		{
			return i;
		}
	}
	return -1;
}

const char* NDKUnity::StringToBase64(const std::string& input)
{
	std::string result;
	size_t      remaining = input.size();
	const auto* src = reinterpret_cast<const unsigned char*>(input.c_str());
	while(remaining > 2)
	{
		result += base64Chars[src[0] >> 2];
		result += base64Chars[((src[0] & 0x3) << 4) | (src[1] >> 4)];
		result += base64Chars[((src[1] & 0xf) << 2) | (src[2] >> 6)];
		result += base64Chars[src[2] & 0x3f];
		remaining -= 3;
		src += 3;
	}
	switch(remaining)
	{
	case 2:
		result += base64Chars[src[0] >> 2];
		result += base64Chars[((src[0] & 0x3) << 4) | (src[1] >> 4)];
		result += base64Chars[(src[1] & 0xf) << 2];
		result += '=';
		break;
	case 1:
		result += base64Chars[src[0] >> 2];
		result += base64Chars[((src[0] & 0x3) << 4)];
		result += '=';
		result += '=';
		break;
	default:
		break;
	}
	size_t bufferSize = result.length() + 1;
	char* buffer = new char[bufferSize];
#if _WIN32
	strcpy_s(buffer, bufferSize, result.c_str());
#else
	strlcpy(buffer, result.c_str(), bufferSize);
#endif	
	return buffer;
}

int NDKUnity::Base64CharToValue(char c)
{
	if (const char* pos = strchr(base64Chars, c))	
		return static_cast<int>(pos - base64Chars);	
	if (c == '=') 
		return -1;
	Inworld::LogError("Invalid Base64 character");
	return 0;
}

std::string NDKUnity::Base64ToString(const std::string& input)
{
	std::vector<unsigned char> result;
	size_t remaining = input.size();
	const char* src = input.c_str();
	while (remaining > 3)
	{
		const int val1 = Base64CharToValue(src[0]);
		const int val2 = Base64CharToValue(src[1]);
		const int val3 = Base64CharToValue(src[2]);
		const int val4 = Base64CharToValue(src[3]);

		result.push_back(val1 << 2 | val2 >> 4);
		if (val3 != -1)
		{
			result.push_back((val2 & 0xf) << 4 | val3 >> 2);
		}
		if (val4 != -1)
		{
			result.push_back((val3 & 0x3) << 6 | val4);
		}
		remaining -= 4;
		src += 4;
	}
	return std::string(result.begin(), result.end());
}



Inworld::CapabilitySet NDKUnity::Capabilities::ToNDK() const
{
	Inworld::CapabilitySet set;
	set.Text = text;
	set.Audio = audio;
	set.Emotions = emotions;
	set.Interruptions = interruptions;
	set.Triggers = triggers;
	set.PhonemeInfo = phonemeInfo;
	set.TurnBasedSTT = turnBasedStt;
	set.NarratedActions = narratedActions;
	return set;
}

NDKUnity::SessionInfo::SessionInfo()
{
	sessionId = "";
	token = "";
	sessionSavedState = "";
	expirationTime = 0;
	isValid = false;
}

NDKUnity::SessionInfo::SessionInfo(const Inworld::SessionInfo& rhs)
{
	sessionId = rhs.SessionId.c_str();
	token = rhs.Token.c_str();
	sessionSavedState = rhs.SessionSavedState.c_str();
	expirationTime = rhs.ExpirationTime;
	isValid = rhs.IsValid();
}

NDKUnity::AgentInfo::AgentInfo()
{
	brainName = "";
	agentId = "";
	givenName = "";
	rpmModelUri = "";
	rpmImageUriPortrait = "";
	rpmImageUriPosture = "";
	avatarImg = "";
	avatarImgOriginal = "";
}

NDKUnity::PacketId::PacketId(const Inworld::PacketId& rhs)
{
	uid = rhs._UID.c_str();
	utteranceID = rhs._UtteranceId.c_str();
	interactionID = rhs._InteractionId.c_str();
}

NDKUnity::Routing::Routing(const Inworld::Routing& rhs)
{
	source = rhs._Source._Name.c_str();
	target = rhs._Target._Name.c_str();
}

NDKUnity::Packet::Packet(const Inworld::Packet& rhs)
{
	packetId = PacketId(rhs._PacketId);
	routing = Routing(rhs._Routing);
	timeStamp = rhs._Timestamp.time_since_epoch().count();
}

NDKUnity::TextPacket::TextPacket(const Inworld::TextEvent& rhs)
{
	packet = Packet(rhs);
	text = rhs.GetText().c_str();
	isFinal = rhs.IsFinal();
}

NDKUnity::PhonemeInfo::PhonemeInfo(const Inworld::AudioDataEvent& evt, const Inworld::AudioDataEvent::PhonemeInfo& phonemeInfo)
{
	packetID = evt._PacketId._UID.c_str();
	code = phonemeInfo.Code.c_str();
	timeStamp = phonemeInfo.Timestamp;
}

NDKUnity::AudioPacket::AudioPacket(const Inworld::AudioDataEvent& rhs)
{
	packet = Packet(rhs);
	audioChunk = StringToBase64(rhs.GetDataChunk());
	type = 1; // AUDIO
	phonemeCount = static_cast<int32_t>(rhs.GetPhonemeInfos().size());
}

NDKUnity::ControlPacket::ControlPacket(const Inworld::ControlEvent& rhs)
{
	packet = Packet(rhs);
	action = rhs.GetControlAction();
}

NDKUnity::EmotionPacket::EmotionPacket(const Inworld::EmotionEvent& rhs)
{
	packet = Packet(rhs);
	behavior = rhs.GetEmotionalBehavior();
	strength = rhs.GetStrength();
}

NDKUnity::CancelResponsePacket::CancelResponsePacket(const Inworld::CancelResponseEvent& rhs)
{
	packet = Packet(rhs);
	cancelInteractionID = rhs.GetInteraction().c_str();
}

NDKUnity::CustomPacket::CustomPacket(const Inworld::CustomEvent& rhs)
{
	packet = Packet(rhs);
	triggerName = rhs.GetName().c_str();
}

NDKUnity::TriggerParam::TriggerParam(const Inworld::CustomEvent& evt, const std::string& name, const std::string& value)
{
	packetID = evt._PacketId._UID.c_str();
	paramName = name.c_str();
	paramValue = value.c_str();
}
