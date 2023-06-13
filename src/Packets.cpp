/**
 * Copyright 2022 Theai, Inc. (DBA Inworld)
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Packets.h"
#include "proto/ProtoDisableWarning.h"

#include <random>

namespace Inworld {

    std::string RandomUUID()
    {
        const std::string Symbols = "0123456789abcdef";

        std::string Result = "00000000-0000-0000-0000-0000000000";

		std::random_device Rd;
		std::mt19937 Gen(Rd());
		std::uniform_int_distribution<> Distr(0, Symbols.size() - 1);

        for (int i = 0; i < Result.size(); i++) {
            if (Result[i] != '-')
            {
				Result[i] = Symbols[Distr(Gen)];
            }
        }
        return Result;
    }

    InworldPakets::Actor Actor::ToProto() const
    {
        InworldPakets::Actor actor;
        actor.set_type(_Type);
        actor.set_name(_Name);
        return actor;
    }

    InworldPakets::Routing Routing::ToProto() const
    {
        InworldPakets::Routing routing;
        *routing.mutable_source() = _Source.ToProto();
        *routing.mutable_target() = _Target.ToProto();
        return routing;
    }

    Routing Routing::Player2Agent(const std::string& AgentId) {
        return Routing(Actor(InworldPakets::Actor_Type_PLAYER, ""), Actor(InworldPakets::Actor_Type_AGENT, AgentId));
    }

    InworldPakets::PacketId PacketId::ToProto() const
    {
        InworldPakets::PacketId proto;
        proto.set_packet_id(_UID);
        proto.set_utterance_id(_UtteranceId);
        proto.set_interaction_id(_InteractionId);
        return proto;
    }

    InworldPakets::InworldPacket Packet::ToProto() const
    {
        InworldPakets::InworldPacket Proto;
        *Proto.mutable_packet_id() = _PacketId.ToProto();
        *Proto.mutable_routing() = _Routing.ToProto();
        *Proto.mutable_timestamp() = 
            ::google::protobuf::util::TimeUtil::TimeTToTimestamp(std::chrono::duration_cast<std::chrono::seconds>(_Timestamp.time_since_epoch()).count());
        ToProtoInternal(Proto);
        return Proto;
    }

    void TextEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const 
    {
        Proto.mutable_text()->set_text(_Text);
        Proto.mutable_text()->set_final(_Final);
        Proto.mutable_text()->set_source_type(_SourceType);
    }

    void ControlEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
    {
        Proto.mutable_control()->set_action(_Action);
    }

    void DataEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
    {
        Proto.mutable_audio_chunk()->set_chunk(std::move(_Chunk));
    }

    AudioDataEvent::AudioDataEvent(const InworldPakets::InworldPacket& GrpcPacket) : DataEvent(GrpcPacket)
    {
        _PhonemeInfos.reserve(GrpcPacket.data_chunk().additional_phoneme_info_size());
        for (const auto& phoneme_info : GrpcPacket.data_chunk().additional_phoneme_info())
        {
            _PhonemeInfos.emplace_back();
            PhonemeInfo& back = _PhonemeInfos.back();
            back.Code = phoneme_info.phoneme();
            back.Timestamp = phoneme_info.start_offset().seconds() + (phoneme_info.start_offset().nanos() / 1000000000.f);
        }
    }

    EmotionEvent::EmotionEvent(const InworldPakets::InworldPacket& GrpcPacket) : Packet(GrpcPacket)
    {
        _Behavior = GrpcPacket.emotion().behavior();
        _Strength = GrpcPacket.emotion().strength();
    }

    void EmotionEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
    {
        Proto.mutable_emotion()->set_behavior(_Behavior);
    }

    void CancelResponseEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
    {
        Proto.mutable_cancelresponses()->set_interaction_id(_InteractionId);
        for (const auto& UtteranceId : _UtteranceIds)
        {
            Proto.mutable_cancelresponses()->add_utterance_id(UtteranceId);
        }
    }

    void SimpleGestureEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
    {
        Proto.mutable_gesture()->set_type(_Gesture);
        Proto.mutable_gesture()->set_playback(_Playback);
	}

	void CustomGestureEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
		
	}

	void CustomEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
        Proto.mutable_custom()->set_name(_Name);
	}

	void SilenceEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
    
	}

}
