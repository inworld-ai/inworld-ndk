/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include "RunnableCommand.h"
#include "comutil.h"
#include "UnityAgentInfo.h"
#include "Utils/Log.h"

#if __cplusplus
extern "C"
{
#endif
	
namespace NDKUnity
{
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
		
		Inworld::CapabilitySet ToNDK() const;
	};

	struct SessionInfo
	{
		BSTR sessionId;
		BSTR token;
		BSTR sessionSavedState;
		int64_t expirationTime;
		int isValid;
		
		SessionInfo();
		explicit SessionInfo(const Inworld::SessionInfo& rhs);		
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
		
		AgentInfo();
		explicit AgentInfo(const UnityAgentInfo& rhs);
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
		explicit PacketId(const Inworld::PacketId& rhs);

	};

	struct Routing
	{
		BSTR source;
		BSTR target;

		Routing() = default;
		explicit Routing(const Inworld::Routing& rhs);
	};

	struct Packet
	{
		PacketId packetId;
		Routing routing;
		int64_t timeStamp;

		Packet() = default;
		explicit Packet(const Inworld::Packet& rhs);
	};
	
	struct TextPacket
	{
		Packet packet;
		BSTR text;
		int isFinal;

		TextPacket() = default;
		explicit TextPacket(const Inworld::TextEvent& rhs);
	};
	
	struct PhonemeInfo
	{
		BSTR packetID;
		BSTR code;
		float timeStamp;

		PhonemeInfo() = default;
		PhonemeInfo(const Inworld::AudioDataEvent& evt, const Inworld::AudioDataEvent::PhonemeInfo& phonemeInfo);
	};
	struct AudioPacket
	{
		Packet packet;
		BSTR audioChunk;
		int type;

		AudioPacket() = default;
		explicit AudioPacket(const Inworld::AudioDataEvent& rhs);
	};
	
	struct ControlPacket
	{
		Packet packet;
		int action;

		ControlPacket() = default;
		explicit ControlPacket(const Inworld::ControlEvent& rhs);
	};
	
	struct EmotionPacket
	{
		Packet packet;
		int behavior;
		int strength;

		EmotionPacket() = default;
		explicit EmotionPacket(const Inworld::EmotionEvent& rhs);
	};

	struct CancelResponsePacket
	{
		Packet packet;
		BSTR cancelInteractionID; // Yan: No need to receive utterance as they won't be sent.

		CancelResponsePacket() = default;
		CancelResponsePacket(const Inworld::CancelResponseEvent& rhs);
	};
	struct CustomPacket
	{
		Packet packet;
		BSTR triggerName;

		CustomPacket() = default;
		CustomPacket(const Inworld::CustomEvent& rhs);
	};
	struct TriggerParam
	{
		BSTR packetID;
		BSTR paramName;
		BSTR paramValue;
		
		TriggerParam() = default;
		TriggerParam(const Inworld::CustomEvent& evt, const std::string& name, const std::string& value);
	};
	
	using UnityCallback = void (*)();	
	using TextCallBack = void(*)(TextPacket);
	using AudioCallBack = void(*)(AudioPacket);
	using PhonemeCallBack = void(*)(PhonemeInfo);
	using ControlCallBack = void(*)(ControlPacket);
	using EmotionCallBack = void(*)(EmotionPacket);
	using CancelResponseCallBack = void(*)(CancelResponsePacket);
	using CustomCallBack = void(*)(CustomPacket);
	using TriggerParamCallBack = void(*)(TriggerParam);
}

#if __cplusplus
}
#endif