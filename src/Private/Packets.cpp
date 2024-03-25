/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Packets.h"
#include "ProtoDisableWarning.h"

#include <random>

#include "Types.h"
#include "Utils/Log.h"

#include "google/protobuf/util/time_util.h"
#include "ai/inworld/packets/packets.pb.h"

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

	Actor::Actor(const InworldPackets::Actor& Actor)
		: _Type(Actor.type())
		, _Name(Actor.name().c_str())
	{}

	InworldPackets::Actor Actor::ToProto() const
    {
        InworldPackets::Actor actor;
        actor.set_type(_Type);
        actor.set_name(_Name);
        return actor;
    }

    InworldPackets::Routing Routing::ToProto() const
    {
        InworldPackets::Routing routing;
        *routing.mutable_source() = _Source.ToProto();
        *routing.mutable_target() = _Target.ToProto();

        for (auto& T : _Targets)
        {
            auto* Actor = routing.add_targets();
            *Actor = T.ToProto();
        }
        return routing;
    }

	Routing::Routing(const InworldPackets::Routing& Routing)
		: _Source(Routing.source())
		, _Target(Routing.target())
	{
        for (uint32_t i = 0; i < Routing.targets_size(); i++)
        {
            _Targets.emplace_back(Routing.targets(i));
        }
	}

	Routing Routing::Player2Agent(const std::string& AgentId) {
        return { { InworldPackets::Actor_Type_PLAYER, "" }, { InworldPackets::Actor_Type_AGENT, AgentId} };
    }

	Routing Routing::Player2Agents(const std::vector<std::string>& AgentIds)
	{
        if (AgentIds.size() == 1)
        {
            return Player2Agent(AgentIds[0]);
        }

        std::vector<Actor> Actors;
        for (auto& Id : AgentIds)
        {
            Actors.emplace_back(InworldPackets::Actor_Type_AGENT, Id);
        }
        return { { InworldPackets::Actor_Type_PLAYER, "" }, Actors };
	}

	PacketId::PacketId(const InworldPackets::PacketId& Other)
		: PacketId(Other.packet_id().c_str(), Other.utterance_id().c_str(), Other.interaction_id().c_str())
	{}

	InworldPackets::PacketId PacketId::ToProto() const
    {
        InworldPackets::PacketId proto;
        proto.set_packet_id(_UID);
        proto.set_utterance_id(_UtteranceId);
        proto.set_interaction_id(_InteractionId);
        return proto;
    }

	Packet::Packet(const InworldPackets::InworldPacket& GrpcPacket)
		: _PacketId(GrpcPacket.packet_id())
		, _Routing(GrpcPacket.routing())
		, _Timestamp(std::chrono::system_clock::time_point(std::chrono::seconds(google::protobuf_inworld::util::TimeUtil::TimestampToTimeT(GrpcPacket.timestamp()))))
	{}

	InworldPackets::InworldPacket Packet::ToProto() const
    {
        InworldPackets::InworldPacket Proto;
        *Proto.mutable_packet_id() = _PacketId.ToProto();
        *Proto.mutable_routing() = _Routing.ToProto();
        *Proto.mutable_timestamp() = 
            ::google::protobuf_inworld::util::TimeUtil::TimeTToTimestamp(std::chrono::duration_cast<std::chrono::seconds>(_Timestamp.time_since_epoch()).count());
        ToProtoInternal(Proto);
        return Proto;
    }

	TextEvent::TextEvent(const InworldPackets::InworldPacket& GrpcPacket)
		: Packet(GrpcPacket)
		, _Text(GrpcPacket.text().text().c_str())
		, _Final(GrpcPacket.text().final())
		, _SourceType(GrpcPacket.text().source_type())
	{}

	TextEvent::TextEvent(const std::string& InText, const Routing& Routing)
		: Packet(Routing)
		, _Text(InText)
		, _Final(true)
		, _SourceType(InworldPackets::TextEvent_SourceType_TYPED_IN)
	{}

	void TextEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
    {
        Proto.mutable_text()->set_text(_Text);
        Proto.mutable_text()->set_final(_Final);
        Proto.mutable_text()->set_source_type(_SourceType);
    }

	ControlEvent::ControlEvent(const InworldPackets::InworldPacket& GrpcPacket)
		: Packet(GrpcPacket)
		, _Action(GrpcPacket.control().action())
	{}

	void ControlEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
    {
        Proto.mutable_control()->set_action(_Action);
    }

	DataEvent::DataEvent(const InworldPackets::InworldPacket& GrpcPacket)
		: Packet(GrpcPacket)
		, _Chunk(GrpcPacket.data_chunk().chunk())
	{}

	void DataEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
    {
        Proto.mutable_data_chunk()->set_chunk(_Chunk);
    }

    void AudioDataEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
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

    AudioDataEvent::AudioDataEvent(const InworldPackets::InworldPacket& GrpcPacket) : DataEvent(GrpcPacket)
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

	const InworldPackets::DataChunk_DataType AudioDataEvent::GetType() const
	{
        return InworldPackets::DataChunk_DataType_AUDIO;
	}

	EmotionEvent::EmotionEvent(const InworldPackets::InworldPacket& GrpcPacket) : Packet(GrpcPacket)
    {
        _Behavior = GrpcPacket.emotion().behavior();
        _Strength = GrpcPacket.emotion().strength();
    }

    void EmotionEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
    {
        Proto.mutable_emotion()->set_behavior(_Behavior);
    }

	CustomGestureEvent::CustomGestureEvent(const InworldPackets::InworldPacket& GrpcPacket)
		: Packet(GrpcPacket)
		, _GestureName(GrpcPacket.custom().name().data())
		, _Playback(GrpcPacket.custom().playback())
	{}

	void CustomGestureEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		
	}

    CustomEvent::CustomEvent(const InworldPackets::InworldPacket& GrpcPacket) : Packet(GrpcPacket)
    {
        _Name = GrpcPacket.custom().name().data();
        for(const auto& Param : GrpcPacket.custom().parameters())
        {
            _Params.insert(std::make_pair<std::string, std::string>(Param.name().data(), Param.value().data()));
        }
    }

	void CustomEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
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

	SilenceEvent::SilenceEvent(const InworldPackets::InworldPacket& GrpcPacket)
		: Packet(GrpcPacket)
		, _Duration(GrpcPacket.data_chunk().duration_ms() * 0.001f)
	{}

	void SilenceEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
    
	}

    void CancelResponseEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
    {
        auto* mutable_cancel_responses = Proto.mutable_mutation()->mutable_cancel_responses();
        mutable_cancel_responses->set_interaction_id(_InteractionId);
        for (const auto& UtteranceId : _UtteranceIds)
        {
            mutable_cancel_responses->add_utterance_id(UtteranceId);
        }
    }

    RelationEvent::RelationEvent(const InworldPackets::InworldPacket& GrpcPacket) : Packet(GrpcPacket)
    {
        const auto currState = GrpcPacket.debug_info().relation().relation_state();
        _Attraction = currState.attraction();
        _Familiar = currState.familiar();
        _Flirtatious = currState.flirtatious();
        _Respect = currState.respect();
        _Trust = currState.trust();
    }

    void RelationEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
    {
        
    }

    ActionEvent::ActionEvent(const InworldPackets::InworldPacket& GrpcPacket) : Packet(GrpcPacket)
    {
        _Content = GrpcPacket.action().narrated_action().content();
    }

    void ActionEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
    {
        Proto.mutable_action()->mutable_narrated_action()->set_content(_Content);
	}

	void SessionControlEvent_SessionConfiguration::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		Proto.mutable_session_control()->mutable_session_configuration()->set_game_session_id(_Data.Id);
	}

	void SessionControlEvent_Capabilities::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
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

	void SessionControlEvent_UserConfiguration::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		Proto.mutable_session_control()->mutable_user_configuration()->set_id(_Data.Id);
		Proto.mutable_session_control()->mutable_user_configuration()->set_name(_Data.Name);

		Inworld::Log("SessionControlEvent_UserConfiguration User id: %s", ARG_STR(_Data.Id));
		Inworld::Log("SessionControlEvent_UserConfiguration User name: %s", ARG_STR(_Data.Name));

		auto* PlayerProfile = Proto.mutable_session_control()->mutable_user_configuration()->mutable_user_settings()->mutable_player_profile();
		for (const auto& Field : _Data.Profile.Fields)
		{
			PlayerProfile->add_fields();
			auto* PlayerField = PlayerProfile->mutable_fields(PlayerProfile->fields_size() - 1);
			PlayerField->set_field_id(Field.Id);
			PlayerField->set_field_value(Field.Value);
		}
	}

	void SessionControlEvent_ClientConfiguration::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		auto* Config = Proto.mutable_session_control()->mutable_client_configuration();
        Config->set_id(_Data.Id);
        Config->set_version(_Data.Version);
		Config->set_description(_Data.Description);

		Inworld::Log("SessionControlEvent_ClientConfiguration Client id: %s", ARG_STR(_Data.Id));
		Inworld::Log("SessionControlEvent_ClientConfiguration Client version: %s", ARG_STR(_Data.Version));
		Inworld::Log("SessionControlEvent_ClientConfiguration Client description: %s", ARG_STR(_Data.Description));
	}

	void SessionControlEvent_SessionSave::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		Proto.mutable_session_control()->mutable_continuation()->set_continuation_type(InworldPackets::Continuation_ContinuationType_CONTINUATION_TYPE_EXTERNALLY_SAVED_STATE);
		Proto.mutable_session_control()->mutable_continuation()->set_externally_saved_state(_Data.Bytes);
	}

	void SessionControlEvent_LoadScene::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
        Proto.mutable_mutation()->mutable_load_scene()->set_name(_Data.Scene);
	}

	void SessionControlEvent_LoadCharacters::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
        auto* LoadCharacters = Proto.mutable_mutation()->mutable_load_characters();
        for (auto& Name : _Data.Names)
        {
            LoadCharacters->add_name()->set_name(Name);
        }
	}

	void SessionControlEvent_UnloadCharacters::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		auto* UnloadCharacters = Proto.mutable_mutation()->mutable_unload_characters();
		for (auto& Name : _Data.Names)
		{
            UnloadCharacters->add_agents()->set_agent_id(Name);
		}
	}

    template<typename TSource>
    void ExtractAgentInfos(const TSource& Source, std::vector<AgentInfo>& AgentInfos)
    {
		AgentInfos.reserve(Source.agents_size());
		for (int32_t i = 0; i < Source.agents_size(); i++)
		{
			AgentInfo& Info = AgentInfos.emplace_back();
			Info.BrainName = Source.agents(i).brain_name().c_str();
			Info.AgentId = Source.agents(i).agent_id().c_str();
			Info.GivenName = Source.agents(i).given_name().c_str();

			const size_t Idx = Info.BrainName.find("__");
			if (Idx != std::string::npos)
			{
				const auto Workspace = Info.BrainName.substr(0, Idx);
				const auto Character = Info.BrainName.substr(Idx + 2, Info.BrainName.size() - Idx + 1);
                const auto Normalized = Inworld::Format("workspaces/%s/characters/%s", Workspace.c_str(), Character.c_str());
				Inworld::Log("Normalizing character brain name '%s', new name '%s'", Info.BrainName.c_str(), Normalized.c_str());
                Info.BrainName = Normalized;
			}
		}
    }

    SessionControlResponse_LoadScene::SessionControlResponse_LoadScene(const InworldPackets::InworldPacket& GrpcPacket)
	{
		const auto& Scene = GrpcPacket.session_control_response().loaded_scene();
        ExtractAgentInfos(Scene, _AgentInfos);
	}

	SessionControlResponse_LoadCharacters::SessionControlResponse_LoadCharacters(const InworldPackets::InworldPacket& GrpcPacket)
	{
		const auto& Characters = GrpcPacket.session_control_response().loaded_characters();
		ExtractAgentInfos(Characters, _AgentInfos);
	}

	SessionControlEvent::SessionControlEvent() 
		: MutationEvent(Routing{ { InworldPackets::Actor_Type_PLAYER, "" }, { InworldPackets::Actor_Type_WORLD, ""} }) 
	{}

}
