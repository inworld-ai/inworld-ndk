/**
* Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include "RunnableCommand.h"


#if __cplusplus
extern "C"
{
#endif
	
namespace NDKUnity
{
#pragma region Structs
	struct Capabilities
	{
		// YAN:
		// The minimum size (char/bool) of C# in C++ is int by default.
		// Changing memory alignment (#pragma pack) on either side will lose efficiency.		
		int32_t text;
		int32_t audio;
		int32_t emotions ;
		int32_t interruptions;
		int32_t triggers;
		int32_t phonemeInfo;
		int32_t turnBasedStt;
		int32_t narratedActions;
		// YAN: No need to set others.
		
		Inworld::CapabilitySet ToNDK() const;
	};

	struct SessionInfo
	{
		const char* sessionId;
		const char* token;
		const char* sessionSavedState;
		int64_t expirationTime;
		int32_t isValid;
		
		SessionInfo();
		explicit SessionInfo(const Inworld::SessionInfo& rhs);		
	};

	struct AgentInfo
	{
		const char* brainName;
		const char* agentId;
		const char* givenName;
		const char* rpmModelUri;
		const char* rpmImageUriPortrait;
		const char* rpmImageUriPosture;
		const char* avatarImg;
		const char* avatarImgOriginal;
		
		AgentInfo();
	};

	struct PacketId
	{
		// Always unique for given packet.
		const char* uid;
		// Text and audio can have same utterance ids, which means they represent same utterance.
		const char* utteranceID;
		// Interaction start when player triggers it and finished when agent answers to player.
		const char* interactionID;

		PacketId() = default;
		explicit PacketId(const Inworld::PacketId& rhs);

	};

	struct Routing
	{
		const char* source;
		const char* target;

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
		const char* text;
		int isFinal;

		TextPacket() = default;
		explicit TextPacket(const Inworld::TextEvent& rhs);
	};
	
	struct PhonemeInfo
	{
		const char* packetID;
		const char* code;
		float timeStamp;

		PhonemeInfo() = default;
		PhonemeInfo(const Inworld::AudioDataEvent& evt, const Inworld::AudioDataEvent::PhonemeInfo& phonemeInfo);
	};
	struct AudioPacket
	{
		Packet  packet;
		const char* audioChunk;
		int32_t type;
		int32_t phonemeCount;

		AudioPacket() = default;
		explicit AudioPacket(const Inworld::AudioDataEvent& rhs);
	};
	
	struct ControlPacket
	{
		Packet packet;
		int32_t action;

		ControlPacket() = default;
		explicit ControlPacket(const Inworld::ControlEvent& rhs);
	};
	
	struct EmotionPacket
	{
		Packet packet;
		int32_t behavior;
		int32_t strength;

		EmotionPacket() = default;
		explicit EmotionPacket(const Inworld::EmotionEvent& rhs);
	};

	struct CancelResponsePacket
	{
		Packet packet;
		const char* cancelInteractionID; // Yan: No need to receive utterance as they won't be sent.

		CancelResponsePacket() = default;
		CancelResponsePacket(const Inworld::CancelResponseEvent& rhs);
	};
	struct CustomPacket
	{
		Packet packet;
		const char* triggerName;

		CustomPacket() = default;
		CustomPacket(const Inworld::CustomEvent& rhs);
	};
	struct TriggerParam
	{
		const char* packetID;
		const char* paramName;
		const char* paramValue;
		
		TriggerParam() = default;
		TriggerParam(const Inworld::CustomEvent& evt, const std::string& name, const std::string& value);
	};
#pragma endregion 

#pragma region Converters
	int CharToInt(char c);
	const char* StringToBase64(const std::string& input);
	std::string Base64ToString(const std::string& input);  // NOLINT(clang-diagnostic-return-type-c-linkage)
	int Base64CharToValue(char c);
#pragma endregion

#pragma region Callbacks
	using UnityCallback = void (*)();	
	using TextCallBack = void(*)(TextPacket);
	using AudioCallBack = void(*)(AudioPacket);
	using PhonemeCallBack = void(*)(PhonemeInfo);
	using ControlCallBack = void(*)(ControlPacket);
	using EmotionCallBack = void(*)(EmotionPacket);
	using CancelResponseCallBack = void(*)(CancelResponsePacket);
	using CustomCallBack = void(*)(CustomPacket);
	using TriggerParamCallBack = void(*)(TriggerParam);
#pragma endregion
	
}

#if __cplusplus
}
#endif