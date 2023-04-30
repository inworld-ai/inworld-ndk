/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include "proto/ProtoDisableWarning.h"
#include "proto/packets.pb.h"
#include <google/protobuf/util/time_util.h>
#include <vector>
#include <chrono>

namespace InworldPakets = ai::inworld::packets;

namespace Inworld {

	std::string RandomUUID();

	// Represents agent or player.
	struct INWORLDAINDK_API Actor
	{
		Actor() = default;
		Actor(const InworldPakets::Actor& Actor) 
			: _Type(Actor.type())
			, _Name(Actor.name().c_str())
		{}
		Actor(const InworldPakets::Actor_Type Type, const std::string& Name) 
			: _Type(Type)
			, _Name(Name) 
		{}

        InworldPakets::Actor ToProto() const;
        
		// Is Actor player or agent.
        InworldPakets::Actor_Type _Type;
        // agent id if this is agent.
        std::string _Name;
	};

	// Source and target for packet.
	struct INWORLDAINDK_API Routing
	{
		Routing() = default;
		Routing(const InworldPakets::Routing& Routing) 
			: _Source(Routing.source())
			, _Target(Routing.target()) 
		{}
		Routing(const Actor& Source, const Actor& Target) 
			: _Source(Source)
			, _Target(Target) 
		{}

		static Routing Player2Agent(const std::string& AgentId);

        InworldPakets::Routing ToProto() const;
        
		Actor _Source;
        Actor _Target;
	};

	struct INWORLDAINDK_API PacketId {
		// Constructs with all random parameters.
        PacketId() 
			: PacketId(RandomUUID(), std::string(RandomUUID()), std::string(RandomUUID())) 
		{}
        PacketId(const InworldPakets::PacketId& Other)
			: PacketId(Other.packet_id().c_str(), Other.utterance_id().c_str(), Other.interaction_id().c_str()) 
		{}
		PacketId(const std::string& UID, const std::string& UtteranceId, const std::string& InteractionId) 
			: _UID(UID)
			, _UtteranceId(UtteranceId)
			, _InteractionId(InteractionId) 
		{}

        InworldPakets::PacketId ToProto() const;
        
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
    class SimpleGestureEvent;
    class CustomGestureEvent;
	class CustomEvent;

    class INWORLDAINDK_API PacketVisitor
    {
    public:
        virtual void Visit(const TextEvent& Event) {  }
        virtual void Visit(const DataEvent& Event) {  }
        virtual void Visit(const AudioDataEvent& Event) {  }
		virtual void Visit(const SilenceEvent& Event) {  }
        virtual void Visit(const ControlEvent& Event) {  }
        virtual void Visit(const EmotionEvent& Event) {  }
        virtual void Visit(const CancelResponseEvent& Event) {  }
        virtual void Visit(const SimpleGestureEvent& Event) {  }
        virtual void Visit(const CustomGestureEvent& Event) {  }
		virtual void Visit(const CustomEvent& Event) {  }
    };

	struct EmotionalState;

	// Base class for all Inworld protocol packets
	class INWORLDAINDK_API Packet
    {
	public:
        Packet() = default;
		Packet(const InworldPakets::InworldPacket& GrpcPacket) 
			: _PacketId(GrpcPacket.packet_id())
			, _Routing(GrpcPacket.routing())
			, _Timestamp(std::chrono::system_clock::time_point(std::chrono::seconds(google::protobuf::util::TimeUtil::TimestampToTimeT(GrpcPacket.timestamp()))))
		{}
        Packet(const Routing& Routing) 
			: _Routing(Routing)
		{}
		virtual ~Packet() = default;

		virtual void Accept(PacketVisitor& Visitor) = 0;

		InworldPakets::InworldPacket ToProto() const;

		Routing GetRouting() { return _Routing; }

    protected:
        virtual void ToProtoInternal(InworldPakets::InworldPacket& Proto) const = 0;
        
	public:
        PacketId _PacketId;
        Routing _Routing;
		std::chrono::system_clock::time_point _Timestamp = std::chrono::system_clock::now();
	};

	class INWORLDAINDK_API TextEvent : public Packet
	{
	public:
		TextEvent() = default;
        TextEvent(const InworldPakets::InworldPacket& GrpcPacket)
            : Packet(GrpcPacket)
            , _Text(GrpcPacket.text().text().c_str())
            , _Final(GrpcPacket.text().final())
            , _SourceType(GrpcPacket.text().source_type())
        {}
        TextEvent(const std::string& InText, const Routing& Routing)
            : Packet(Routing)
            , _Text(InText)
            , _Final(true)
            , _SourceType(InworldPakets::TextEvent_SourceType_TYPED_IN)
        {}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		const std::string& GetText() const { return _Text; }

        bool IsFinal() const { return _Final; }

	protected:
        virtual void ToProtoInternal(InworldPakets::InworldPacket& Proto) const override;

	private:
		std::string _Text;
		bool _Final;
		InworldPakets::TextEvent_SourceType _SourceType;
	};

	class INWORLDAINDK_API DataEvent : public Packet
    {
	public:
		DataEvent() = default;
		DataEvent(const InworldPakets::InworldPacket& GrpcPacket)
			: Packet(GrpcPacket)
			, _Chunk(GrpcPacket.data_chunk().chunk())
		{}
		DataEvent(const std::string& Data, const Routing& Routing)
			: Packet(Routing)
			, _Chunk(Data)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

        const std::string& GetDataChunk() const { return _Chunk; }

		virtual const InworldPakets::DataChunk_DataType GetType() const = 0;

    protected:
        virtual void ToProtoInternal(InworldPakets::InworldPacket& Proto) const override;

	private:
		// protobuf stores bytes data as string, to save copy time we can use same data type.
		std::string _Chunk;
	};

	class INWORLDAINDK_API AudioDataEvent : public DataEvent
	{
	public:
		AudioDataEvent() = default;
		AudioDataEvent(const InworldPakets::InworldPacket& GrpcPacket);
		AudioDataEvent(const std::string& Data, const Routing& Routing)
			: DataEvent(Data, Routing)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		const InworldPakets::DataChunk_DataType GetType() const override { return InworldPakets::DataChunk_DataType_AUDIO; }

		struct PhonemeInfo
		{
			std::string Code;
			float Timestamp;
		};

		const std::vector<PhonemeInfo>& GetPhonemeInfos() const { return _PhonemeInfos; }

	private:
		std::vector<PhonemeInfo> _PhonemeInfos;
	};

	class INWORLDAINDK_API SilenceEvent : public Packet
	{
	public:
		SilenceEvent() = default;
		SilenceEvent(const InworldPakets::InworldPacket& GrpcPacket)
			: Packet(GrpcPacket)
			, _Duration(GrpcPacket.data_chunk().duration_ms() * 0.001f)
		{}
		SilenceEvent(float Duration, const Routing& Routing)
			: Packet(Routing)
			, _Duration(Duration)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		float GetDuration() const { return _Duration; }

	protected:
		virtual void ToProtoInternal(InworldPakets::InworldPacket& Proto) const override;

	private:
		float _Duration;
	};
    
	class INWORLDAINDK_API ControlEvent : public Packet
    {
    public:
		ControlEvent() = default;
		ControlEvent(const InworldPakets::InworldPacket& GrpcPacket)
			: Packet(GrpcPacket)
			, _Action(GrpcPacket.control().action())
		{}
        ControlEvent(InworldPakets::ControlEvent_Action Action, const Routing& Routing)
			: Packet(Routing)
			, _Action(Action)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

        InworldPakets::ControlEvent_Action GetControlAction() const { return _Action; }

    protected:
        virtual void ToProtoInternal(InworldPakets::InworldPacket& Proto) const override;

	private:
		InworldPakets::ControlEvent_Action _Action;
    };

    class INWORLDAINDK_API EmotionEvent : public Packet
    {
    public:
		EmotionEvent() = default;
		EmotionEvent(const InworldPakets::InworldPacket& GrpcPacket);

        virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		InworldPakets::EmotionEvent_SpaffCode GetEmotionalBehavior() const { return _Behavior; }
		InworldPakets::EmotionEvent_Strength GetStrength() const { return _Strength; }

    protected:
        virtual void ToProtoInternal(InworldPakets::InworldPacket& Proto) const override;

	private:
		InworldPakets::EmotionEvent_SpaffCode _Behavior;
		InworldPakets::EmotionEvent_Strength _Strength;
    };

    class INWORLDAINDK_API CancelResponseEvent : public Packet
    {
    public:
		CancelResponseEvent() = default;
        CancelResponseEvent(const std::string& InteractionId, const std::vector<std::string>& UtteranceIds, const Routing& Routing) 
			: Packet(Routing)
			, _InteractionId(InteractionId)
            , _UtteranceIds(UtteranceIds)
		{}

        virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

    protected:
        virtual void ToProtoInternal(InworldPakets::InworldPacket& Proto) const override;

    private:
		std::string _InteractionId;
		std::vector<std::string> _UtteranceIds;
    };

    class INWORLDAINDK_API SimpleGestureEvent : public Packet
    {
    public:
		SimpleGestureEvent() = default;
		SimpleGestureEvent(const InworldPakets::InworldPacket& GrpcPacket)
            : Packet(GrpcPacket)
            , _Gesture(GrpcPacket.gesture().type())
			, _Playback(GrpcPacket.gesture().playback())
        {}
		SimpleGestureEvent(InworldPakets::GestureEvent_Type Gesture, InworldPakets::Playback Playback, const Routing& Routing)
            : Packet(Routing)
            , _Gesture(Gesture)
			, _Playback(Playback)
        {}

        virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

        InworldPakets::GestureEvent_Type GetSimpleGesture() const { return _Gesture; }
		InworldPakets::Playback GetPlayback() const { return _Playback; }

    protected:
        virtual void ToProtoInternal(InworldPakets::InworldPacket& Proto) const override;

    private:
        InworldPakets::GestureEvent_Type _Gesture;
		InworldPakets::Playback _Playback;
    };

    class INWORLDAINDK_API CustomGestureEvent : public Packet
    {
    public:
		CustomGestureEvent() = default;
		CustomGestureEvent(const InworldPakets::InworldPacket& GrpcPacket)
            : Packet(GrpcPacket)
            , _GestureName(GrpcPacket.custom().name().data())
			, _Playback(GrpcPacket.custom().playback())
        {}
		CustomGestureEvent(const std::string& Gesture, const Routing& Routing)
            : Packet(Routing)
            , _GestureName(Gesture)
        {}

        virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

        const std::string& GetCustomGesture() const { return _GestureName; }

    protected:
        virtual void ToProtoInternal(InworldPakets::InworldPacket& Proto) const override;

    private:
		std::string _GestureName;
		InworldPakets::Playback _Playback;
	};

	class INWORLDAINDK_API CustomEvent : public Packet
	{
	public:
		CustomEvent() = default;
		CustomEvent(const InworldPakets::InworldPacket& GrpcPacket)
			: Packet(GrpcPacket)
			, _Name(GrpcPacket.custom().name().data())
		{}
		CustomEvent(const std::string& Name, const Routing& Routing)
			: Packet(Routing)
			, _Name(Name)
		{}

		virtual void Accept(PacketVisitor& Visitor) override { Visitor.Visit(*this); }

		const std::string& GetName() const { return _Name; }

	protected:
		virtual void ToProtoInternal(InworldPakets::InworldPacket& Proto) const override;

	private:
		std::string _Name;
	};

}
#pragma warning(pop)
