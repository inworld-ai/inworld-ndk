#pragma once
#include "RunnableCommand.h"
#include "comutil.h"
#include "Utils/Log.h"

#if __cplusplus
extern "C"
{
#endif
	
namespace NDKUnity
{
	// YAN: BSTR is the string format used in COM as well as Unity Communication.
	//		According to https://learn.microsoft.com/zh-cn/cpp/atl-mfc-shared/allocating-and-releasing-memory-for-a-bstr?view=msvc-170
	//		Unity will handle GC for BSTR itself.		
	inline BSTR StringToBSTR(std::string rhs)
	{
		return _com_util::ConvertStringToBSTR(rhs.c_str());
	}	
	using UnityCallback = void (*)();

	

	struct Capabilities
	{
		// YAN:
		// The minimum size (char/bool) of C# in C++ is int by default.
		// Changing memory alignment (#pragma pack) on either side will lose efficiency.		
		int text;
		int audio;
		int emotions ;
		int interruptions;
		int triggers;
		int phonemeInfo;
		int turnBasedStt;
		int narratedActions;
		// YAN: No need to set others.
		
		Inworld::CapabilitySet ToNDK() const
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
	};

	struct SessionInfo
	{
		BSTR sessionId;
		BSTR token;
		BSTR sessionSavedState;
		int64_t expirationTime;
		int isValid;

		SessionInfo()
		{
			sessionId = SysAllocString(L"");
			token = SysAllocString(L"");
			sessionSavedState = SysAllocString(L"");
			expirationTime = 0;
			isValid = false;
		}

		explicit SessionInfo(const Inworld::SessionInfo& rhs)
		{
			sessionId = StringToBSTR(rhs.SessionId);
			token = StringToBSTR(rhs.Token);
			sessionSavedState = StringToBSTR(rhs.SessionSavedState);
			expirationTime = rhs.ExpirationTime;
			isValid = rhs.IsValid();
		}
		
	};

	struct AgentInfo
	{
		BSTR brainName;
		BSTR agentId;
		BSTR givenName;
		BSTR rpmModelUri;
		BSTR rpmImageUriPortrait;
		BSTR rpmImageUriPosture;
		BSTR avatarImg;
		BSTR avatarImgOriginal;

		AgentInfo()
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

		explicit AgentInfo(const Inworld::AgentInfo& rhs)
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
	};

	struct PacketId
	{
		// Always unique for given packet.
		BSTR uid;
		// Text and audio can have same utterance ids, which means they represent same utterance.
		BSTR utteranceID;
		// Interaction start when player triggers it and finished when agent answers to player.
		BSTR interactionID;

		PacketId() = default;

		explicit PacketId(const Inworld::PacketId& rhs)
		{
			uid = StringToBSTR(rhs._UID);
			utteranceID = StringToBSTR(rhs._UtteranceId);
			interactionID = StringToBSTR(rhs._InteractionId);
		}
	};

	struct Routing
	{
		BSTR source;
		BSTR target;

		Routing() = default;

		explicit Routing(const Inworld::Routing& rhs)
		{
			source = StringToBSTR(rhs._Source._Name);
			target = StringToBSTR(rhs._Target._Name);
		}
	};

	struct Packet
	{
		PacketId packetId;
		Routing routing;
		int64_t timeStamp;

		Packet() = default;

		explicit Packet(const Inworld::Packet& rhs)
		{
			packetId = PacketId(rhs._PacketId);
			routing = Routing(rhs._Routing);
			timeStamp = rhs._Timestamp.time_since_epoch().count();
		}
	};
	struct TextPacket
	{
		Packet packet;
		BSTR text;
		int isFinal;

		TextPacket() = default;

		explicit TextPacket(const Inworld::TextEvent& rhs)
		{
			packet = Packet(rhs);
			text = StringToBSTR(rhs.GetText());
			isFinal = rhs.IsFinal();
		}
	};

	using TextCallBack = void(*)(TextPacket );
}

#if __cplusplus
}
#endif