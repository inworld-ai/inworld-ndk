/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
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

        for (auto& T : _Targets)
        {
            auto* Actor = routing.add_targets();
            *Actor = T.ToProto();
        }
        return routing;
    }

	Routing::Routing(const InworldPakets::Routing& Routing)
		: _Source(Routing.source())
		, _Target(Routing.target())
	{
        for (uint32_t i = 0; i < Routing.targets_size(); i++)
        {
            _Targets.emplace_back(Routing.targets(i));
        }
	}

	Routing Routing::Player2Agent(const std::string& AgentId) {
        return { { InworldPakets::Actor_Type_PLAYER, "" }, { InworldPakets::Actor_Type_AGENT, AgentId} };
    }

	Routing Routing::Player2Agents(const std::vector<std::string>& AgentIds)
	{
        std::vector<Actor> Actors;
        for (auto& Id : AgentIds)
        {
            Actors.emplace_back(InworldPakets::Actor_Type_AGENT, Id);
        }
        return { { InworldPakets::Actor_Type_PLAYER, "" }, Actors };
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
            ::google::protobuf_inworld::util::TimeUtil::TimeTToTimestamp(std::chrono::duration_cast<std::chrono::seconds>(_Timestamp.time_since_epoch()).count());
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
        Proto.mutable_data_chunk()->set_chunk(_Chunk);
    }

    void AudioDataEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
    {
        DataEvent::ToProtoInternal(Proto);
        Proto.mutable_data_chunk()->set_type(GetType());

        for (const auto& phoneme_info : GetPhonemeInfos())
        {
            auto* info = Proto.mutable_data_chunk()->add_additional_phoneme_info();
            info->set_phoneme(phoneme_info.Code);
            info->mutable_start_offset()->set_seconds(phoneme_info.Timestamp);
            info->mutable_start_offset()->set_nanos(
                (phoneme_info.Timestamp - std::floor(phoneme_info.Timestamp)) * 1000000000);
        }
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

	void CustomGestureEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
		
	}

    CustomEvent::CustomEvent(const InworldPakets::InworldPacket& GrpcPacket) : Packet(GrpcPacket)
    {
        _Name = GrpcPacket.custom().name().data();
        for(const auto& Param : GrpcPacket.custom().parameters())
        {
            _Params.insert(std::make_pair<std::string, std::string>(Param.name().data(), Param.value().data()));
        }
    }

	void CustomEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
        auto* mutable_custom = Proto.mutable_custom();
        mutable_custom->set_name(_Name);
        for (const std::pair<const std::string, std::string>& Param : _Params)
        {
            auto* param = mutable_custom->add_parameters();
            param->set_name(Param.first);
            param->set_value(Param.second);
        }
	}

    void SilenceEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
    
	}

    void CancelResponseEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
    {
        auto* mutable_cancel_responses = Proto.mutable_mutation()->mutable_cancel_responses();
        mutable_cancel_responses->set_interaction_id(_InteractionId);
        for (const auto& UtteranceId : _UtteranceIds)
        {
            mutable_cancel_responses->add_utterance_id(UtteranceId);
        }
    }

    ChangeSceneEvent::ChangeSceneEvent(const InworldPakets::InworldPacket& GrpcPacket) : MutationEvent(GrpcPacket)
    {
        _AgentInfos.reserve(GrpcPacket.load_scene_output().agents_size());
        for (const auto& agent : GrpcPacket.load_scene_output().agents())
        {
            _AgentInfos.emplace_back();
            Inworld::AgentInfo& back = _AgentInfos.back();
            back.AgentId = agent.agent_id();
            back.BrainName = agent.brain_name();
            back.GivenName = agent.given_name();
        }
    }

    void ChangeSceneEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
    {
        auto* mutable_load_scene = Proto.mutable_mutation()->mutable_load_scene();
        mutable_load_scene->set_name(_SceneName);
    }

    RelationEvent::RelationEvent(const InworldPakets::InworldPacket& GrpcPacket) : Packet(GrpcPacket)
    {
        const auto currState = GrpcPacket.debug_info().relation().relation_state();
        _Attraction = currState.attraction();
        _Familiar = currState.familiar();
        _Flirtatious = currState.flirtatious();
        _Respect = currState.respect();
        _Trust = currState.trust();
    }

    void RelationEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
    {
        
    }

    ActionEvent::ActionEvent(const InworldPakets::InworldPacket& GrpcPacket) : Packet(GrpcPacket)
    {
        _Content = GrpcPacket.action().narrated_action().content();
    }

    void ActionEvent::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
    {
        Proto.mutable_action()->mutable_narrated_action()->set_content(_Content);
    }

	void SessionControlEvent_SessionConfiguration::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
		Proto.mutable_session_control()->mutable_session_configuration()->set_game_session_id(_Data.Id);
	}

	void SessionControlEvent_Capabilities::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
		auto* Capabilities = Proto.mutable_session_control()->mutable_capabilities_configuration();
		Capabilities->set_audio(_Data.Audio);
		Capabilities->set_emotions(_Data.Emotions);
		Capabilities->set_interruptions(_Data.Interruptions);
		Capabilities->set_emotion_streaming(_Data.EmotionStreaming);
		Capabilities->set_silence_events(_Data.SilenceEvents);
		Capabilities->set_phoneme_info(_Data.PhonemeInfo);
		Capabilities->set_narrated_actions(_Data.NarratedActions);
		Capabilities->set_continuation(_Data.Continuation);
		Capabilities->set_turn_based_stt(_Data.TurnBasedSTT);
		Capabilities->set_relations(_Data.Relations);
		Capabilities->set_debug_info(_Data.Relations);
		Capabilities->set_multi_agent(_Data.Multiagent);
	}

	void SessionControlEvent_UserConfiguration::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
		Proto.mutable_session_control()->mutable_user_configuration()->set_id(_Data.Id);
		Proto.mutable_session_control()->mutable_user_configuration()->set_name(_Data.Name);
		auto* PlayerProfile = Proto.mutable_session_control()->mutable_user_configuration()->mutable_user_settings()->mutable_player_profile();
		for (const auto& Field : _Data.Profile.Fields)
		{
			PlayerProfile->add_fields();
			auto* PlayerField = PlayerProfile->mutable_fields(PlayerProfile->fields_size() - 1);
			PlayerField->set_field_id(Field.Id);
			PlayerField->set_field_value(Field.Value);
		}
	}

	void SessionControlEvent_ClientConfiguration::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
		auto* Config = Proto.mutable_session_control()->mutable_client_configuration();
        Config->set_id(_Data.Id);
        Config->set_version(_Data.Version);
        Config->set_description(_Data.Description);
	}

	void SessionControlEvent_SessionSave::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
		Proto.mutable_session_control()->mutable_continuation()->set_continuation_type(InworldPakets::Continuation_ContinuationType_CONTINUATION_TYPE_EXTERNALLY_SAVED_STATE);
		Proto.mutable_session_control()->mutable_continuation()->set_externally_saved_state(_Data.Bytes);
	}

	void SessionControlEvent_LoadScene::ToProtoInternal(InworldPakets::InworldPacket& Proto) const
	{
        Proto.mutable_mutation()->mutable_load_scene()->set_name(_Data.Scene);
	}

}
