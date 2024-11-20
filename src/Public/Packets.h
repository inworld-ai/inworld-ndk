/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include "Define.h"
#include "Types.h"

#include <vector>
#include <optional>
#include <unordered_map>
#include <chrono>

namespace ai { namespace inworld { namespace packets {
	class Actor;
	class Routing;
	class PacketId;
	class InworldPacket;

	enum Actor_Type : int;
	enum TextEvent_SourceType : int;
	enum DataChunk_DataType : int;
	enum ControlEvent_Action : int;
	enum AudioSessionStartPayload_MicrophoneMode : int;
	enum AudioSessionStartPayload_UnderstandingMode : int;
	enum EmotionEvent_SpaffCode : int;
	enum EmotionEvent_Strength : int;
	enum Playback : int;
    enum ConversationEventPayload_ConversationEventType : int;
	enum CustomEvent_Type : int;
	enum PingPongReport_Type : int;
	enum PerceivedLatencyReport_Precision : int;
	enum LogsEvent_LogLevel : int;

	namespace entities {
		enum ItemsInEntitiesOperation_Type : int;
	}

}}}
namespace InworldPackets = ai::inworld::packets;

namespace Inworld {

	INWORLD_EXPORT std::string RandomUUID();

	// Represents agent or player.
	struct INWORLD_EXPORT Actor
	{
		Actor() = default;
		Actor(const InworldPackets::Actor& Actor);
		Actor(const InworldPackets::Actor_Type Type, const std::string& Name) 
			: _Type(Type)
			, _Name(Name) 
		{}

        InworldPackets::Actor ToProto() const;

	    bool operator==(const Actor& Other) const { return _Type == Other._Type && _Name == Other._Name; }
	    bool operator!=(const Actor& Other) const { return !(*this == Other); }
        
		// Is Actor player or agent.
        InworldPackets::Actor_Type _Type = static_cast<InworldPackets::Actor_Type>(0);
        // agent id if this is agent.
        std::string _Name;
	};

	// Source and target for packet.
	struct INWORLD_EXPORT Routing
	{
		Routing() = default;
		Routing(const InworldPackets::Routing& Routing);
		Routing(const Actor& Source, const Actor& Target) 
			: _Source(Source)
			, _Target(Target) 
		{}
		Routing(const Actor& Source, const std::string& ConversationId) 
			: _Source(Source)
			, _ConversationId(ConversationId) 
		{}

	    bool operator==(const Routing& Other) const { return _Source == Other._Source && _Target == Other._Target && _ConversationId == Other._ConversationId; }
	    bool operator!=(const Routing& Other) const { return !(*this == Other); }

		static Routing Player2Agent(const std::string& AgentId);
		static Routing Player2Conversation(const std::string& ConversationId);
		static Routing Player2World();

        InworldPackets::Routing ToProto() const;
        
		Actor _Source;
        Actor _Target;
        std::string _ConversationId;
	};

	struct INWORLD_EXPORT PacketId {
		// Constructs with all random parameters.
        PacketId()
			: PacketId(RandomUUID(), RandomUUID(), RandomUUID(), RandomUUID())
		{}
        PacketId(const InworldPackets::PacketId& Other);
		PacketId(const std::string& UID, const std::string& UtteranceId, const std::string& InteractionId, const std::string& CorrelationId) 
			: _UID(UID)
			, _UtteranceId(UtteranceId)
			, _InteractionId(InteractionId)
			, _CorrelationId(CorrelationId)
		{}

        InworldPackets::PacketId ToProto() const;
        
		// Always unique for given packet.
        std::string _UID;
        // Text and audio can have same utterance ids, which means they represent same utterance.
        std::string _UtteranceId;
        // Interaction start when player triggers it and finished when agent answers to player.
        std::string _InteractionId;
		std::string _CorrelationId;
	};

    class TextEvent;
	class VADEvent;
    class DataEvent;
    class AudioDataEvent;
	class A2FHeaderEvent;
	class A2FContentEvent;
    class SilenceEvent;
    class ControlEvent;
	class ControlEventConversationUpdate;
	class ControlEventCurrentSceneStatus;
    class EmotionEvent;
    class CancelResponseEvent;
    class CustomGestureEvent;
    class CustomEvent;
	class ActionEvent;
	class RelationEvent;
	class PingEvent;
	class LogEvent;

    class INWORLD_EXPORT PacketVisitor
    {
    public:
        virtual void Visit(const TextEvent& Event) {  }
		virtual void Visit(const VADEvent& Event) {  }
        virtual void Visit(const DataEvent& Event) {  }
        virtual void Visit(const AudioDataEvent& Event) {  }
		virtual void Visit(const A2FHeaderEvent& Event) {  }
		virtual void Visit(const A2FContentEvent& Event) {  }
        virtual void Visit(const SilenceEvent& Event) {  }
        virtual void Visit(const ControlEvent& Event) {  }
        virtual void Visit(const ControlEventConversationUpdate& Event) {  }
		virtual void Visit(const ControlEventCurrentSceneStatus& Event) {  }
        virtual void Visit(const EmotionEvent& Event) {  }
        virtual void Visit(const CancelResponseEvent& Event) {  }
        virtual void Visit(const CustomGestureEvent& Event) {  }
        virtual void Visit(const CustomEvent& Event) {  }
    	virtual void Visit(const ActionEvent& Event) { }
    	virtual void Visit(const RelationEvent& Event) { }
		virtual void Visit(const PingEvent& Event) { }
		virtual void Visit(const LogEvent& Event) { }
    };

	struct EmotionalState;

	// Base class for all Inworld protocol packets
	class INWORLD_EXPORT Packet
    {
	public:
        Packet() = default;
		Packet(const InworldPackets::InworldPacket& GrpcPacket);
        Packet(const Routing& Routing) 
			: _Routing(Routing)
		{}
		virtual ~Packet() = default;

		virtual void Accept(PacketVisitor& Visitor) = 0;

		InworldPackets::InworldPacket ToProto() const;

		Routing GetRouting() { return _Routing; }

    protected:
        virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const {}
        
	public:
        PacketId _PacketId;
        Routing _Routing;
		std::chrono::system_clock::time_point _Timestamp = std::chrono::system_clock::now();
	};

	class INWORLD_EXPORT TextEvent : public Packet
	{
	public:
		TextEvent() = default;
        TextEvent(const InworldPackets::InworldPacket& GrpcPacket);
        TextEvent(const std::string& Text, const Routing& Routing);

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		const std::string& GetText() const { return _Text; }

        bool IsFinal() const { return _Final; }

		InworldPackets::TextEvent_SourceType GetSourceType() const { return _SourceType; }

	protected:
        virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		std::string _Text;
		bool _Final;
		InworldPackets::TextEvent_SourceType _SourceType;
	};

	class INWORLD_EXPORT DataEvent : public Packet
    {
	public:
		DataEvent() = default;
		DataEvent(const InworldPackets::InworldPacket& GrpcPacket);
		DataEvent(const std::string& Data, const Routing& Routing)
			: Packet(Routing)
			, _Chunk(Data)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

        const std::string& GetDataChunk() const { return _Chunk; }

		virtual const InworldPackets::DataChunk_DataType GetType() const = 0;

    protected:
        virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

		// protobuf stores bytes data as string, to save copy time we can use same data type.
		std::string _Chunk;
	};

	class INWORLD_EXPORT AudioDataEvent : public DataEvent
	{
	public:
		AudioDataEvent() = default;
		AudioDataEvent(const InworldPackets::InworldPacket& GrpcPacket);
		AudioDataEvent(const std::string& Data, const Routing& Routing)
			: DataEvent(Data, Routing)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		const InworldPackets::DataChunk_DataType GetType() const override;

		struct PhonemeInfo
		{
			std::string Code;
			float Timestamp;
		};

		const std::vector<PhonemeInfo>& GetPhonemeInfos() const { return _PhonemeInfos; }
	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;
		
	private:
		std::vector<PhonemeInfo> _PhonemeInfos;
	};

	class INWORLD_EXPORT VADEvent : public Packet
	{
	public:
		VADEvent() = default;
		VADEvent(bool VoiceDetected, const Routing& Routing)
			: Packet(Routing)
			, _VoiceDetected(VoiceDetected)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		bool IsVoiceDetected() const { return _VoiceDetected; }

	private:
		bool _VoiceDetected;
  };

	class INWORLD_EXPORT A2FHeaderEvent : public Packet
	{
	public:
		A2FHeaderEvent() = default;
		A2FHeaderEvent(const InworldPackets::InworldPacket& GrpcPacket);

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		int32_t GetChannelCount() const { return _ChannelCount; }
		int32_t GetSamplesPerSecond() const { return _SamplesPerSecond; }
		int32_t GetBitsPerSample() const { return _BitsPerSample; }
		const std::vector<std::string>& GetBlendShapes() const { return _BlendShapes; }

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override {}

	private:
		int32_t _ChannelCount = 0;
		int32_t _SamplesPerSecond = 0;
		int32_t _BitsPerSample = 0;
		std::vector<std::string> _BlendShapes;
	};

	class INWORLD_EXPORT A2FContentEvent : public Packet
	{
	public:
		A2FContentEvent() = default;
		A2FContentEvent(const InworldPackets::InworldPacket& GrpcPacket);

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		struct FAudioInfo
		{
			double _TimeCode;
			std::string _Audio;
		};

		struct FSkeletalAnim
		{
			struct FBlendShapeWeights
			{
				double _TimeCode;
				std::vector<float> _Values;
			};

			std::vector<FBlendShapeWeights> _BlendShapeWeights;
		};

		const FAudioInfo& GetAudioInfo() const { return _AudioInfo; }
		const FSkeletalAnim& GetSkeletalAnim() const { return _SkeletalAnimInfo; }

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override {}

	private:
		FAudioInfo _AudioInfo;
		FSkeletalAnim _SkeletalAnimInfo;
	};

	class INWORLD_EXPORT SilenceEvent : public Packet
	{
	public:
		SilenceEvent() = default;
		SilenceEvent(const InworldPackets::InworldPacket& GrpcPacket);
		SilenceEvent(float Duration, const Routing& Routing)
			: Packet(Routing)
			, _Duration(Duration)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		float GetDuration() const { return _Duration; }

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		float _Duration;
	};
    
	class INWORLD_EXPORT ControlEvent : public Packet
    {
    public:
		ControlEvent() = default;
		ControlEvent(const InworldPackets::InworldPacket& GrpcPacket);
        ControlEvent(InworldPackets::ControlEvent_Action Action, const std::string& Description, const Routing& Routing)
			: Packet(Routing)
			, _Action(Action)
			, _Description(Description)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

        InworldPackets::ControlEvent_Action GetControlAction() const { return _Action; }
        const std::string& GetDescription() const { return _Description; }

    protected:
        virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		InworldPackets::ControlEvent_Action _Action;
		std::string _Description;
    };

	class INWORLD_EXPORT ControlEventSessionConfiguration : public ControlEvent
	{
	public:
		struct INWORLD_EXPORT SessionConfiguration
		{
			std::string Id;
		};

		struct INWORLD_EXPORT Capabilities
		{
			bool Animations = false;
			bool Audio = false;
			bool Emotions = false;
			bool Interruptions = false;
			bool EmotionStreaming = false;
			bool SilenceEvents = false;
			bool PhonemeInfo = false;
			bool Continuation = true;
			bool TurnBasedSTT = true;
			bool NarratedActions = true;
			bool Relations = true;
			bool MultiAgent = true;
			bool Audio2Face = false;
			bool MultiModalActionPlanning = false;
			bool PingPongReport = true;
			bool PerceivedLatencyReport = true;
			bool Logs = false;
			bool LogsWarning = true;
			bool LogsInfo = true;
			bool LogsDebug = false;
			bool LogsInternal = false;
		};

		struct INWORLD_EXPORT UserConfiguration
		{
			struct PlayerProfile
			{
				struct PlayerField
				{
					std::string Id;
					std::string Value;
				};

				std::vector<PlayerField> Fields;
			};

			PlayerProfile Profile;

			std::string Name;
			std::string Id;
		};

		struct INWORLD_EXPORT ClientConfiguration
		{
			std::string Id;
			std::string Version;
			std::string Description;
		};

		struct INWORLD_EXPORT Continuation
		{
			std::string ExternallySavedState;
		};

	public:
		ControlEventSessionConfiguration() = default;
		ControlEventSessionConfiguration(
			const SessionConfiguration& SessionConfiguration,
			const Capabilities& Capabilities,
			const UserConfiguration& UserConfiguration,
			const ClientConfiguration& ClientConfiguration,
			const Continuation& Continuation
		);

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		SessionConfiguration _SessionConfiguration;
		Capabilities _Capabilities;
		UserConfiguration _UserConfiguration;
		ClientConfiguration _ClientConfiguration;
		Continuation _Continuation;
	};

	class INWORLD_EXPORT ControlEventAudioSessionStart : public ControlEvent
	{
	public:
		ControlEventAudioSessionStart() = default;
		ControlEventAudioSessionStart(const InworldPackets::InworldPacket& GrpcPacket);
		ControlEventAudioSessionStart(const Routing& Routing,
		    InworldPackets::AudioSessionStartPayload_MicrophoneMode MicrophoneMode,
		    InworldPackets::AudioSessionStartPayload_UnderstandingMode UnderstandingMode);

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		InworldPackets::AudioSessionStartPayload_MicrophoneMode _MicrophoneMode;
		InworldPackets::AudioSessionStartPayload_UnderstandingMode _UnderstandingMode;
	};

    class INWORLD_EXPORT ControlEventConversationUpdate : public ControlEvent
    {
    public:
        ControlEventConversationUpdate() = default;
        ControlEventConversationUpdate(const InworldPackets::InworldPacket& GrpcPacket);
    	ControlEventConversationUpdate(const std::string& ConversationId, const std::vector<std::string>& Agents, bool bIncludePlayer);

        virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

    	InworldPackets::ConversationEventPayload_ConversationEventType GetType() const { return _EventType; }
    	const std::vector<std::string>& GetAgents() const { return _Agents; }
    	bool GetIncludePlayer() const { return _bIncludePlayer; }

    protected:
        virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

    private:
        std::vector<std::string> _Agents;
        bool _bIncludePlayer;
        InworldPackets::ConversationEventPayload_ConversationEventType _EventType;
    };

	class INWORLD_EXPORT ControlEventCurrentSceneStatus : public ControlEvent
	{
	public:
		ControlEventCurrentSceneStatus() = default;
		ControlEventCurrentSceneStatus(const InworldPackets::InworldPacket& GrpcPacket);

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		const std::string& GetSceneName() const { return _SceneName; }
		const std::string& GetSceneDescription() const { return _SceneDescription; }
		const std::string& GetSceneDisplayName() const { return _SceneDisplayName; }

		const std::vector<AgentInfo>& GetAgentInfos() const { return _AgentInfos; }

	private:
		std::string _SceneName;
		std::string _SceneDescription;
		std::string _SceneDisplayName;

		std::vector<AgentInfo> _AgentInfos;
	};

    class INWORLD_EXPORT EmotionEvent : public Packet
    {
    public:
		EmotionEvent() = default;
		EmotionEvent(const InworldPackets::InworldPacket& GrpcPacket);

        virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		InworldPackets::EmotionEvent_SpaffCode GetEmotionalBehavior() const { return _Behavior; }
		InworldPackets::EmotionEvent_Strength GetStrength() const { return _Strength; }

    protected:
        virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		InworldPackets::EmotionEvent_SpaffCode _Behavior;
		InworldPackets::EmotionEvent_Strength _Strength;
    };

    class INWORLD_EXPORT CustomGestureEvent : public Packet
    {
    public:
		CustomGestureEvent() = default;
		CustomGestureEvent(const InworldPackets::InworldPacket& GrpcPacket);
		CustomGestureEvent(const std::string& Gesture, const Routing& Routing)
            : Packet(Routing)
            , _GestureName(Gesture)
        {}

        virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

        const std::string& GetCustomGesture() const { return _GestureName; }

    protected:
        virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

    private:
		std::string _GestureName;
		InworldPackets::Playback _Playback;
	};

	class INWORLD_EXPORT CustomEvent : public Packet
	{
	public:
		CustomEvent() = default;
		CustomEvent(const InworldPackets::InworldPacket& GrpcPacket);
		CustomEvent(const std::string& Name, const std::unordered_map<std::string, std::string>& Params, const Routing& Routing)
			: Packet(Routing)
			, _Name(Name)
			, _Params(Params)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		const std::string& GetName() const { return _Name; }
		const std::unordered_map<std::string, std::string>& GetParams() const { return _Params; }

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		std::string _Name;
		std::unordered_map<std::string, std::string> _Params;
	};

	class INWORLD_EXPORT RelationEvent : public Packet
	{
	public:
		RelationEvent() = default;
		RelationEvent(const InworldPackets::InworldPacket& GrpcPacket);

		int32_t GetAttraction() const { return _Attraction; }
		int32_t GetFamiliar() const { return _Familiar; }
		int32_t GetFlirtatious() const { return _Flirtatious; }
		int32_t GetRespect() const { return _Respect; }
		int32_t GetTrust() const { return _Trust; }
	
		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }
	
	
	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;
	
	private:
		int32_t _Attraction;
		int32_t _Familiar;
		int32_t _Flirtatious;
		int32_t _Respect;
		int32_t _Trust;
	};

	class INWORLD_EXPORT ActionEvent : public Packet
	{
	public:
		ActionEvent() = default;
		ActionEvent(const InworldPackets::InworldPacket& GrpcPacket);
		ActionEvent(const std::string& Content, const Routing& Routing)
			: Packet(Routing)
			, _Content(Content)
		{}
		const std::string& GetContent() const { return _Content; }
		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }	
	
	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;
	
	private:
		std::string _Content;
	};
	
	class INWORLD_EXPORT MutationEvent : public Packet
	{
	public:
		MutationEvent() = default;
		MutationEvent(const InworldPackets::InworldPacket& GrpcPacket)
			: Packet(GrpcPacket)
		{}
		MutationEvent(const Routing& Routing)
			: Packet(Routing)
		{}

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const = 0;
	};

	class INWORLD_EXPORT CancelResponseEvent : public MutationEvent
	{
	public:
		CancelResponseEvent() = default;
		CancelResponseEvent(const std::string& InteractionId, const std::vector<std::string>& UtteranceIds, const Routing& Routing)
			: MutationEvent(Routing)
			, _InteractionId(InteractionId)
			, _UtteranceIds(UtteranceIds)
		{}

		const std::string& GetInteraction() const { return _InteractionId; }

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		std::string _InteractionId;
		std::vector<std::string> _UtteranceIds;
	};

	class INWORLD_EXPORT SessionControlEvent : public MutationEvent
	{
	public:
		SessionControlEvent();

		virtual void Accept(PacketVisitor& Visitor) override { /* Outgoing Only */ }
	};

	class INWORLD_EXPORT SessionControlEvent_LoadScene : public SessionControlEvent
	{
	public:
		struct INWORLD_EXPORT Data
		{
			std::string Scene;
		};

		SessionControlEvent_LoadScene(const Data& Data)
			: SessionControlEvent()
			, _Data(Data)
		{}

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		Data _Data;
	};

	class INWORLD_EXPORT SessionControlEvent_LoadCharacters : public SessionControlEvent
	{
	public:
		struct INWORLD_EXPORT Data
		{
			std::vector<std::string> Names;
		};

		SessionControlEvent_LoadCharacters(const Data& Data)
			: SessionControlEvent()
			, _Data(Data)
		{}

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		Data _Data;
	};

	class INWORLD_EXPORT SessionControlEvent_UnloadCharacters : public SessionControlEvent
	{
	public:
		struct INWORLD_EXPORT Data
		{
			std::vector<std::string> Names;
		};

		SessionControlEvent_UnloadCharacters(const Data& Data)
			: SessionControlEvent()
			, _Data(Data)
		{}

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		Data _Data;
	};

	class INWORLD_EXPORT EntitiesItemsOperationEvent : public Packet
	{
	public:
		EntitiesItemsOperationEvent()
			: EntitiesItemsOperationEvent(Routing::Player2World())
		{}
		EntitiesItemsOperationEvent(const InworldPackets::InworldPacket& GrpcPacket)
			: Packet(GrpcPacket)
		{}
		EntitiesItemsOperationEvent(const Routing& Routing)
			: Packet(Routing)
		{}
		
		virtual void Accept(PacketVisitor & Visitor) override { /* Outgoing Only */ }

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const = 0;
	};

	class INWORLD_EXPORT CreateOrUpdateItemsOperationEvent : public EntitiesItemsOperationEvent
	{
	public:
		struct INWORLD_EXPORT EntityItem
		{
			std::string Id;
			std::string DisplayName;
			std::string Description;
			std::unordered_map<std::string, std::string> Properties;
		};

		CreateOrUpdateItemsOperationEvent(const std::vector<EntityItem>& Items, const std::vector<std::string>& AddToEntities)
			: EntitiesItemsOperationEvent()
			, _Items(Items)
			, _AddToEntities(AddToEntities)
		{}

		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const;

	private:
		std::vector<EntityItem> _Items;
		std::vector<std::string> _AddToEntities;
	};

	class INWORLD_EXPORT RemoveItemsOperationEvent : public EntitiesItemsOperationEvent
	{
	public:
		RemoveItemsOperationEvent(const std::vector<std::string>& ItemIds)
			: EntitiesItemsOperationEvent()
			, _ItemIds(ItemIds)
		{}

		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const;

	private:
		std::vector<std::string> _ItemIds;
	};

	class INWORLD_EXPORT ItemsInEntitiesOperationEventBase : public EntitiesItemsOperationEvent
	{
	public:
		ItemsInEntitiesOperationEventBase(const std::vector<std::string>& ItemIds, const std::vector<std::string>& EntityNames)
			: EntitiesItemsOperationEvent()
			, _ItemIds(ItemIds)
			, _EntityNames(EntityNames)
		{}

		virtual InworldPackets::entities::ItemsInEntitiesOperation_Type GetType() const = 0;

		virtual void ToProtoInternal(InworldPackets::InworldPacket & Proto) const override;

	private:
		std::vector<std::string> _ItemIds;
		std::vector<std::string> _EntityNames;
	};

	template<InworldPackets::entities::ItemsInEntitiesOperation_Type T>
	class INWORLD_EXPORT ItemsInEntitiesOperationEvent : public ItemsInEntitiesOperationEventBase
	{
	public:
		ItemsInEntitiesOperationEvent(const std::vector<std::string>& ItemIds, const std::vector<std::string>& EntityNames)
			: ItemsInEntitiesOperationEventBase(ItemIds, EntityNames)
		{}

		virtual InworldPackets::entities::ItemsInEntitiesOperation_Type GetType() const override
		{
			return T;
		}
	};

	class INWORLD_EXPORT PingEvent : public Packet
	{
	public:
		PingEvent() = default;
		PingEvent(const InworldPackets::InworldPacket& GrpcPacket)
			: Packet(GrpcPacket)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }
	};

	class INWORLD_EXPORT PongEvent : public Packet
	{
	public:
		PongEvent() = default;
		PongEvent(const PingEvent& Ping)
			: Packet(Routing::Player2World())
			, _PingPacketId(Ping._PacketId)
			, _PingTimestamp(Ping._Timestamp)
		{}

		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

		virtual void Accept(PacketVisitor& Visitor) override { /* Outgoing Only */ }

	private:
		PacketId _PingPacketId;
		std::chrono::system_clock::time_point _PingTimestamp;
	};

	class INWORLD_EXPORT PerceivedLatencyReportEventBase : public Packet
	{
	public:
		PerceivedLatencyReportEventBase(uint32_t Duration)
			: Packet(Routing::Player2World())
			, _Duration(Duration)
		{}

		virtual InworldPackets::PerceivedLatencyReport_Precision GetType() const = 0;

		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

		virtual void Accept(PacketVisitor& Visitor) override { /* Outgoing Only */ }

	private:
		uint32_t _Duration;
	};

	template<InworldPackets::PerceivedLatencyReport_Precision T>
	class INWORLD_EXPORT PerceivedLatencyReportEvent : public PerceivedLatencyReportEventBase
	{

	public:
		PerceivedLatencyReportEvent(uint32_t Duration)
			: PerceivedLatencyReportEventBase(Duration)
		{}

		virtual InworldPackets::PerceivedLatencyReport_Precision GetType() const override
		{
			return T;
		}
	};

	class INWORLD_EXPORT LogEvent : public Packet
	{
	public:
		LogEvent() = default;
		LogEvent(const InworldPackets::InworldPacket& GrpcPacket);

		const InworldPackets::LogsEvent_LogLevel GetLogLevel() const { return _LogLevel; }
		const std::string& GetLogText() const { return _Text; }

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

	private:
		std::string _Text;
		InworldPackets::LogsEvent_LogLevel _LogLevel;
	};

}
