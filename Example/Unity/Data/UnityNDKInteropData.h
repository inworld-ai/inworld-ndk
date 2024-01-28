/**
* Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once
#include "RunnableCommand.h"
#include "UnityNDKInteropData.h"


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
		int32_t relations;
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
		const char* audioChunk;
		int32_t type;
		int32_t phonemeCount;

		AudioPacket() = default;
		explicit AudioPacket(const Inworld::AudioDataEvent& rhs);
	};
	
	struct ControlPacket
	{
		int32_t action;

		ControlPacket() = default;
		explicit ControlPacket(const Inworld::ControlEvent& rhs);
	};
	
	struct EmotionPacket
	{
		int32_t behavior;
		int32_t strength;

		EmotionPacket() = default;
		explicit EmotionPacket(const Inworld::EmotionEvent& rhs);
	};

	struct CancelResponsePacket
	{
		const char* cancelInteractionID; // Yan: No need to receive utterance as they won't be sent.

		CancelResponsePacket() = default;
		CancelResponsePacket(const Inworld::CancelResponseEvent& rhs);
	};
	struct CustomPacket
	{
		const char* triggerName;

		CustomPacket() = default;
		CustomPacket(const Inworld::CustomEvent& rhs);
	};
	struct RelationPacket
	{
		int32_t attraction;
		int32_t familiar;
		int32_t flirtatious;
		int32_t respect;
		int32_t trust;
		
		RelationPacket() = default;
		RelationPacket(const Inworld::RelationEvent& rhs);
	};
	struct ActionPacket
	{
		const char* content;
		
		ActionPacket() = default;
		ActionPacket(const Inworld::ActionEvent& rhs);
	};
	struct TriggerParam
	{
		const char* packetID;
		const char* paramName;
		const char* paramValue;
		
		TriggerParam() = default;
		TriggerParam(const Inworld::CustomEvent& evt, const std::string& name, const std::string& value);
	};

	struct UnityNDKPacket
	{
		Packet packetInfo;
		const char* packetType;
		
		TextPacket textPacket;
		AudioPacket audioPacket;
		ControlPacket ctrlPacket;
		EmotionPacket emoPacket;
		CancelResponsePacket cancelResponsePacket;
		CustomPacket customPacket;
		RelationPacket relationPacket;
		ActionPacket actPacket;

		UnityNDKPacket() = default;
		explicit UnityNDKPacket(const Inworld::TextEvent& rhs);
		explicit UnityNDKPacket(const Inworld::AudioDataEvent& rhs);
		explicit UnityNDKPacket(const Inworld::ControlEvent& rhs);
		explicit UnityNDKPacket(const Inworld::EmotionEvent& rhs);
		explicit UnityNDKPacket(const Inworld::CancelResponseEvent& rhs);
		explicit UnityNDKPacket(const Inworld::CustomEvent& rhs);
		explicit UnityNDKPacket(const Inworld::RelationEvent& rhs);
		explicit UnityNDKPacket(const Inworld::ActionEvent& rhs);		
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
	using UnityPacketCallback = void(*)(UnityNDKPacket);
	using TriggerParamCallBack = void(*)(TriggerParam);
	using PhonemeCallBack = void(*)(PhonemeInfo);
#pragma endregion
	
}

#if __cplusplus
}
#endif