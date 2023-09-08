#include "UnityNDKInteropData.h"

// YAN: BSTR is the string format used in COM as well as Unity Communication.
//		According to https://learn.microsoft.com/zh-cn/cpp/atl-mfc-shared/allocating-and-releasing-memory-for-a-bstr?view=msvc-170
//		Unity will handle GC for BSTR itself.		
inline BSTR StringToBSTR(std::string rhs)
{
	return _com_util::ConvertStringToBSTR(rhs.c_str());
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

NDKUnity::AgentInfo::AgentInfo(const Inworld::AgentInfo& rhs)
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
	audioChunk = StringToBSTR(rhs.GetDataChunk());
	type = 1; // AUDIO
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
	cancelInteractionID = StringToBSTR(rhs.GetCancelledInteraction());
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