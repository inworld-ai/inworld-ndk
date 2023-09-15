/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "UnityNDKInteropData.h"
#include "UnityAgentInfo.h"
#include "Utils/Log.h"

static constexpr char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int NDKUnity::CharToInt(char c)
{
	for(int i = 0; i < 64; ++i)
	{
		if(base64Chars[i] == c)
		{
			return i;
		}
	}
	return -1;
}

std::string NDKUnity::StringToBase64(const std::string& input)
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
		src += 2;
		break;
	case 1:
		result += base64Chars[src[0] >> 2];
		result += base64Chars[((src[0] & 0x3) << 4)];
		result += '=';
		result += '=';
		src += 1;
		break;
	default:
		break;
	}
	return result;
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

// YAN: BSTR is the string format used in COM as well as Unity Communication.
//		According to https://learn.microsoft.com/zh-cn/cpp/atl-mfc-shared/allocating-and-releasing-memory-for-a-bstr?view=msvc-170
//		Unity will handle GC for BSTR itself.		
inline BSTR NDKUnity::StringToBSTR(const std::string& rhs)
{
	return _com_util::ConvertStringToBSTR(rhs.c_str());
}
// YAN: Cannot send Byte[] as const_cast<string> to Unity.
inline BSTR NDKUnity::DataChunkToBSTR(const std::string& rhs)
{
	const std::string base64 = StringToBase64(rhs);
	return _com_util::ConvertStringToBSTR(base64.c_str());
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
	sessionId = SysAllocString(L"");
	token = SysAllocString(L"");
	sessionSavedState = SysAllocString(L"");
	expirationTime = 0;
	isValid = false;
}

NDKUnity::SessionInfo::SessionInfo(const Inworld::SessionInfo& rhs)
{
	sessionId = StringToBSTR(rhs.SessionId);
	token = StringToBSTR(rhs.Token);
	sessionSavedState = StringToBSTR(rhs.SessionSavedState);
	expirationTime = rhs.ExpirationTime;
	isValid = rhs.IsValid();
}

NDKUnity::AgentInfo::AgentInfo()
{
	brainName = SysAllocString(L"");
	agentId = SysAllocString(L"");
	givenName = SysAllocString(L"");
	rpmModelUri = SysAllocString(L"");
	rpmImageUriPortrait = SysAllocString(L"");
	rpmImageUriPosture = SysAllocString(L"");
	avatarImg = SysAllocString(L"");
	avatarImgOriginal = SysAllocString(L"");
}

NDKUnity::AgentInfo::AgentInfo(const UnityAgentInfo& rhs)
{
	brainName = StringToBSTR(rhs.BrainName);
	agentId = StringToBSTR(rhs.AgentId);
	givenName = StringToBSTR(rhs.GivenName);
	rpmModelUri = StringToBSTR(rhs.RpmModelUri);
	rpmImageUriPortrait = StringToBSTR(rhs.RpmImageUriPortrait);
	rpmImageUriPosture = StringToBSTR(rhs.RpmImageUriPortrait);
	avatarImg = StringToBSTR(rhs.RpmImageUriPortrait);
	avatarImgOriginal = StringToBSTR(rhs.RpmImageUriPortrait);
}

NDKUnity::PacketId::PacketId(const Inworld::PacketId& rhs)
{
	uid = StringToBSTR(rhs._UID);
	utteranceID = StringToBSTR(rhs._UtteranceId);
	interactionID = StringToBSTR(rhs._InteractionId);
}

NDKUnity::Routing::Routing(const Inworld::Routing& rhs)
{
	source = StringToBSTR(rhs._Source._Name);
	target = StringToBSTR(rhs._Target._Name);
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
	text = StringToBSTR(rhs.GetText());
	isFinal = rhs.IsFinal();
}

NDKUnity::PhonemeInfo::PhonemeInfo(const Inworld::AudioDataEvent& evt, const Inworld::AudioDataEvent::PhonemeInfo& phonemeInfo)
{
	packetID = StringToBSTR(evt._PacketId._UID);
	code = StringToBSTR(phonemeInfo.Code);
	timeStamp = phonemeInfo.Timestamp;
}

NDKUnity::AudioPacket::AudioPacket(const Inworld::AudioDataEvent& rhs)
{
	packet = Packet(rhs);
	audioChunk = DataChunkToBSTR(rhs.GetDataChunk());
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
	cancelInteractionID = StringToBSTR(rhs.GetInteraction());
}

NDKUnity::CustomPacket::CustomPacket(const Inworld::CustomEvent& rhs)
{
	packet = Packet(rhs);
	triggerName = StringToBSTR(rhs.GetName());
}

NDKUnity::TriggerParam::TriggerParam(const Inworld::CustomEvent& evt, const std::string& name, const std::string& value)
{
	packetID = StringToBSTR(evt._PacketId._UID);
	paramName = StringToBSTR(name);
	paramValue = StringToBSTR(value);
}
