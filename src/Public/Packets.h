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
	enum EmotionEvent_SpaffCode : int;
	enum EmotionEvent_Strength : int;
	enum Playback : int;
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
        
		// Is Actor player or agent.
        InworldPackets::Actor_Type _Type;
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
		Routing(const Actor & Source, const std::vector<Actor>& Targets)
			: _Source(Source)
			, _Targets(Targets)
		{}

		static Routing Player2Agent(const std::string& AgentId);
		static Routing Player2Agents(const std::vector<std::string>& AgentIds);

        InworldPackets::Routing ToProto() const;
        
		Actor _Source;
        Actor _Target;
		std::vector<Actor> _Targets;
	};

	struct INWORLD_EXPORT PacketId {
		// Constructs with all random parameters.
        PacketId() 
			: PacketId(RandomUUID(), std::string(RandomUUID()), std::string(RandomUUID())) 
		{}
        PacketId(const InworldPackets::PacketId& Other);
		PacketId(const std::string& UID, const std::string& UtteranceId, const std::string& InteractionId) 
			: _UID(UID)
			, _UtteranceId(UtteranceId)
			, _InteractionId(InteractionId) 
		{}

        InworldPackets::PacketId ToProto() const;
        
		// Always unique for given packet.
        std::string _UID;
        // Text and audio can have same utterance ids, which means they represent same utterance.
        std::string _UtteranceId;
        // Interaction start when player triggers it and finished when agent answers to player.
        std::string _InteractionId;
	};

    class TextEvent;
    class DataEvent;
    class AudioDataEvent;
    class SilenceEvent;
    class ControlEvent;
    class EmotionEvent;
    class CancelResponseEvent;
    class CustomGestureEvent;
    class CustomEvent;
	class ActionEvent;
	class RelationEvent;
	class SessionControlResponse_LoadScene;
	class SessionControlResponse_LoadCharacters;

    class INWORLD_EXPORT PacketVisitor
    {
    public:
        virtual void Visit(const TextEvent& Event) {  }
        virtual void Visit(const DataEvent& Event) {  }
        virtual void Visit(const AudioDataEvent& Event) {  }
        virtual void Visit(const SilenceEvent& Event) {  }
        virtual void Visit(const ControlEvent& Event) {  }
        virtual void Visit(const EmotionEvent& Event) {  }
        virtual void Visit(const CancelResponseEvent& Event) {  }
        virtual void Visit(const CustomGestureEvent& Event) {  }
        virtual void Visit(const CustomEvent& Event) {  }
    	virtual void Visit(const ActionEvent& Event) { }
    	virtual void Visit(const RelationEvent& Event) { }
    	virtual void Visit(const SessionControlResponse_LoadScene& Event) { }
    	virtual void Visit(const SessionControlResponse_LoadCharacters& Event) { }
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

		virtual void Accept(PacketVisitor& Visitor) {}

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
        TextEvent(const std::string& InText, const Routing& Routing);

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		const std::string& GetText() const { return _Text; }

        bool IsFinal() const { return _Final; }

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
	};

	class INWORLD_EXPORT SessionControlEvent_SessionConfiguration : public SessionControlEvent
	{
	public:
		struct INWORLD_EXPORT Data
		{
			std::string Id;
		};

		SessionControlEvent_SessionConfiguration(const Data& Data)
			: SessionControlEvent()
			, _Data(Data)
		{}

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		Data _Data;
	};

	class INWORLD_EXPORT SessionControlEvent_Capabilities : public SessionControlEvent
	{
	public:
		struct INWORLD_EXPORT Data
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
			bool Multiagent = true;
		};

		SessionControlEvent_Capabilities(const Data& Data)
			: SessionControlEvent()
			, _Data(Data)
		{}

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		Data _Data;
	};

	class INWORLD_EXPORT SessionControlEvent_UserConfiguration : public SessionControlEvent
	{
	public:
		struct INWORLD_EXPORT Data
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

		SessionControlEvent_UserConfiguration(const Data& Data)
			: SessionControlEvent()
			, _Data(Data)
		{}

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		Data _Data;
	};

	class INWORLD_EXPORT SessionControlEvent_ClientConfiguration : public SessionControlEvent
	{
	public:
		struct INWORLD_EXPORT Data
		{
			std::string Id;
			std::string Version;
			std::string Description;
		};

		SessionControlEvent_ClientConfiguration(const Data& Data)
			: SessionControlEvent()
			, _Data(Data)
		{}

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		Data _Data;
	};

	class INWORLD_EXPORT SessionControlEvent_SessionSave : public SessionControlEvent
	{
	public:
		struct INWORLD_EXPORT Data
		{
			std::string Bytes;
		};

		SessionControlEvent_SessionSave(const Data& Data)
			: SessionControlEvent()
			, _Data(Data)
		{}

	protected:
		virtual void ToProtoInternal(InworldPackets::InworldPacket& Proto) const override;

	private:
		Data _Data;
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

	class INWORLD_EXPORT SessionControlResponse_LoadScene : public Packet
	{
	public:
		SessionControlResponse_LoadScene(const InworldPackets::InworldPacket& GrpcPacket);

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		const std::vector<AgentInfo>& GetAgentInfos() const { return _AgentInfos; }

	private:
		std::vector<AgentInfo> _AgentInfos;
	};

	class INWORLD_EXPORT SessionControlResponse_LoadCharacters : public Packet
	{
	public:
		SessionControlResponse_LoadCharacters(const InworldPackets::InworldPacket& GrpcPacket);

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		const std::vector<AgentInfo>& GetAgentInfos() const { return _AgentInfos; }

	private:
		std::vector<AgentInfo> _AgentInfos;
	};
}
