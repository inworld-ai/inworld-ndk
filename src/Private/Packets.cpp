/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "Packets.h"
#include "ProtoDisableWarning.h"

#include <random>
#include <sstream>
#include <regex>

#include "Types.h"
#include "Utils/Log.h"

#include "google/protobuf/util/time_util.h"
#include "google/protobuf/util/json_util.h"
#include "ai/inworld/packets/packets.pb.h"
#include "nvidia/a2f/nvidia_ace.controller.v1.pb.h"

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
        if (_Source._Type != InworldPackets::Actor_Type_UNKNOWN)
        {
            *routing.mutable_source() = _Source.ToProto();
        }
        if (_Target._Type != InworldPackets::Actor_Type_UNKNOWN)
        {
            *routing.mutable_target() = _Target.ToProto();
        }

        return routing;
    }

	Routing::Routing(const InworldPackets::Routing& Routing)
		: _Source(Routing.source())
		, _Target(Routing.target())
	{
	}

	Routing Routing::Player2Agent(const std::string& AgentId) {
        return { { InworldPackets::Actor_Type_PLAYER, "" }, { InworldPackets::Actor_Type_AGENT, AgentId} };
    }

	Routing Routing::Player2Conversation(const std::string& ConversationId)
	{
    	return { { InworldPackets::Actor_Type_PLAYER, "" }, ConversationId };
	}

	Routing Routing::Player2World() {
		return { { InworldPackets::Actor_Type_PLAYER, "" }, { InworldPackets::Actor_Type_WORLD, ""}};
	}

	PacketId::PacketId(const InworldPackets::PacketId& Other)
		: PacketId(Other.packet_id(), Other.utterance_id(), Other.interaction_id(), Other.correlation_id())
	{}

	InworldPackets::PacketId PacketId::ToProto() const
    {
        InworldPackets::PacketId proto;
        proto.set_packet_id(_UID);
        proto.set_utterance_id(_UtteranceId);
        proto.set_interaction_id(_InteractionId);
		proto.set_conversation_id(_CorrelationId);
        return proto;
    }

	Packet::Packet(const InworldPackets::InworldPacket& GrpcPacket)
		: _PacketId(GrpcPacket.packet_id())
		, _Routing(GrpcPacket.routing())
		, _Timestamp(std::chrono::system_clock::time_point(std::chrono::seconds(google::protobuf_inworld::util::TimeUtil::TimestampToTimeT(GrpcPacket.timestamp()))))
    {
    	_Routing._ConversationId = GrpcPacket.packet_id().conversation_id();
    }

	InworldPackets::InworldPacket Packet::ToProto() const
    {
        InworldPackets::InworldPacket Proto;
        *Proto.mutable_packet_id() = _PacketId.ToProto();
        *Proto.mutable_routing() = _Routing.ToProto();
        *Proto.mutable_timestamp() = 
            ::google::protobuf_inworld::util::TimeUtil::TimeTToTimestamp(std::chrono::duration_cast<std::chrono::seconds>(_Timestamp.time_since_epoch()).count());
    	*Proto.mutable_packet_id()->mutable_conversation_id() = _Routing._ConversationId;
        ToProtoInternal(Proto);
        return Proto;
    }

	TextEvent::TextEvent(const InworldPackets::InworldPacket& GrpcPacket)
		: Packet(GrpcPacket)
		, _Text(GrpcPacket.text().text().c_str())
		, _Final(GrpcPacket.text().final())
		, _SourceType(GrpcPacket.text().source_type())
    {
    	
    }

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
		, _Description(GrpcPacket.control().description())
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

	A2FHeaderEvent::A2FHeaderEvent(const InworldPackets::InworldPacket& GrpcPacket) : Packet(GrpcPacket)
	{
		nvidia_ace::controller::v1::AnimationDataStream AnimationDataStream;
		AnimationDataStream.ParseFromString(GrpcPacket.data_chunk().chunk());

		nvidia_ace::controller::v1::AnimationDataStreamHeader AnimationDataStreamHeader = AnimationDataStream.animation_data_stream_header();
		_ChannelCount = AnimationDataStreamHeader.audio_header().channel_count();
		_SamplesPerSecond = AnimationDataStreamHeader.audio_header().samples_per_second();
		_BitsPerSample = AnimationDataStreamHeader.audio_header().bits_per_sample();

		for (const auto& BlendShape : AnimationDataStreamHeader.skel_animation_header().blend_shapes())
		{
			_BlendShapes.push_back(BlendShape.c_str());
		}
	}

	A2FContentEvent::A2FContentEvent(const InworldPackets::InworldPacket& GrpcPacket) : Packet(GrpcPacket)
	{
		nvidia_ace::controller::v1::AnimationDataStream AnimationDataStream;
		AnimationDataStream.ParseFromString(GrpcPacket.data_chunk().chunk());

		nvidia_ace::animation_data::v1::AnimationData AnimationData = AnimationDataStream.animation_data();

		_AudioInfo._Audio = AnimationData.audio().audio_buffer();
		_AudioInfo._TimeCode = AnimationData.audio().time_code();

		for (const auto& BlendShapeWeight : AnimationData.skel_animation().blend_shape_weights())
		{
			auto& _BlendShapeWeight = _SkeletalAnimInfo._BlendShapeWeights.emplace_back();
			_BlendShapeWeight._TimeCode = BlendShapeWeight.time_code();
			for (const auto& Value : BlendShapeWeight.values())
			{
				_BlendShapeWeight._Values.push_back(Value);
			}
		}
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
		auto& Custom = GrpcPacket.custom();
        _Name = Custom.name().data();
        for(const auto& Param : Custom.parameters())
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

	SessionControlEvent::SessionControlEvent() 
		: MutationEvent(Routing::Player2World()) 
	{}

	ControlEventSessionConfiguration::ControlEventSessionConfiguration(
		const SessionConfiguration& SessionConfiguration,
		const Capabilities& Capabilities,
		const UserConfiguration& UserConfiguration,
		const ClientConfiguration& ClientConfiguration,
		const Continuation& Continuation
	)
		: ControlEvent(InworldPackets::ControlEvent_Action_SESSION_CONFIGURATION, "", Routing::Player2World())
		, _SessionConfiguration(SessionConfiguration)
		, _Capabilities(Capabilities)
		, _UserConfiguration(UserConfiguration)
		, _ClientConfiguration(ClientConfiguration)
		, _Continuation(Continuation)
	{}

	void ControlEventSessionConfiguration::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		ControlEvent::ToProtoInternal(Proto);

		auto* MutableSessionConfiguration = Proto.mutable_control()->mutable_session_configuration();

		MutableSessionConfiguration->mutable_session_configuration()->set_game_session_id(_SessionConfiguration.Id);

		auto* MutableCapabilities = MutableSessionConfiguration->mutable_capabilities_configuration();
		MutableCapabilities->set_audio(_Capabilities.Audio);
		MutableCapabilities->set_emotions(_Capabilities.Emotions);
		MutableCapabilities->set_interruptions(_Capabilities.Interruptions);
		MutableCapabilities->set_emotion_streaming(_Capabilities.EmotionStreaming);
		MutableCapabilities->set_silence_events(_Capabilities.SilenceEvents);
		MutableCapabilities->set_phoneme_info(_Capabilities.PhonemeInfo);
		MutableCapabilities->set_narrated_actions(_Capabilities.NarratedActions);
		MutableCapabilities->set_continuation(_Capabilities.Continuation);
		MutableCapabilities->set_turn_based_stt(_Capabilities.TurnBasedSTT);
		MutableCapabilities->set_relations(_Capabilities.Relations);
		MutableCapabilities->set_debug_info(_Capabilities.Relations);
		MutableCapabilities->set_multi_agent(_Capabilities.MultiAgent);
		MutableCapabilities->set_audio2face(_Capabilities.Audio2Face);
		MutableCapabilities->set_multi_modal_action_planning(_Capabilities.MultiModalActionPlanning);
		MutableCapabilities->set_ping_pong_report(_Capabilities.PingPongReport);
		MutableCapabilities->set_perceived_latency_report(_Capabilities.PerceivedLatencyReport);
		MutableCapabilities->set_logs(_Capabilities.Logs);
		MutableCapabilities->set_logs_warning(_Capabilities.LogsWarning);
		MutableCapabilities->set_logs_info(_Capabilities.LogsInfo);
		MutableCapabilities->set_logs_debug(_Capabilities.LogsDebug);
		MutableCapabilities->set_logs_internal(_Capabilities.LogsInternal);

		auto* MutableUserConfiguration = MutableSessionConfiguration->mutable_user_configuration();
		MutableUserConfiguration->set_id(_UserConfiguration.Id);
		MutableUserConfiguration->set_name(_UserConfiguration.Name);

		Inworld::Log("ControlEventSessionConfiguration User id: %s", _UserConfiguration.Id.c_str());
		Inworld::Log("ControlEventSessionConfiguration User name: %s", _UserConfiguration.Name.c_str());

		auto* MutablePlayerProfile = MutableUserConfiguration->mutable_user_settings()->mutable_player_profile();
		for (const auto& Field : _UserConfiguration.Profile.Fields)
		{
			MutablePlayerProfile->add_fields();
			auto* MutablePlayerField = MutablePlayerProfile->mutable_fields(MutablePlayerProfile->fields_size() - 1);
			MutablePlayerField->set_field_id(Field.Id);
			MutablePlayerField->set_field_value(Field.Value);
		}

		auto* MutableClientConfiguration = MutableSessionConfiguration->mutable_client_configuration();

		Inworld::Log("ControlEventSessionConfiguration Client id: %s", _ClientConfiguration.Id.c_str());
		Inworld::Log("ControlEventSessionConfiguration Client version: %s", _ClientConfiguration.Version.c_str());
		Inworld::Log("ControlEventSessionConfiguration Client description: %s", _ClientConfiguration.Description.c_str());

		MutableClientConfiguration->set_id(_ClientConfiguration.Id);
		MutableClientConfiguration->set_version(_ClientConfiguration.Version);
		MutableClientConfiguration->set_description(_ClientConfiguration.Description);

		if (_Continuation.ExternallySavedState.size() > 0)
		{
			Inworld::Log("ControlEventSessionConfiguration Continuation (Externally Saved State)");

			auto* MutableContinuation = MutableSessionConfiguration->mutable_continuation();

			MutableContinuation->set_continuation_type(InworldPackets::Continuation_ContinuationType_CONTINUATION_TYPE_EXTERNALLY_SAVED_STATE);
			MutableContinuation->set_externally_saved_state(_Continuation.ExternallySavedState);
		}
	}

	ControlEventAudioSessionStart::ControlEventAudioSessionStart(const InworldPackets::InworldPacket& GrpcPacket)
		: ControlEvent(GrpcPacket)
	{
		const auto& AudioStartEvent = GrpcPacket.control().audio_session_start();
		_MicrophoneMode = AudioStartEvent.mode();
	}

	ControlEventAudioSessionStart::ControlEventAudioSessionStart(const Routing& Routing,
	    InworldPackets::AudioSessionStartPayload_MicrophoneMode MicrophoneMode,
	    InworldPackets::AudioSessionStartPayload_UnderstandingMode UnderstandingMode)
		: ControlEvent(InworldPackets::ControlEvent_Action_AUDIO_SESSION_START, "", Routing)
		, _MicrophoneMode(MicrophoneMode)
		, _UnderstandingMode(UnderstandingMode)
	{}

	void ControlEventAudioSessionStart::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		ControlEvent::ToProtoInternal(Proto);

		if (_MicrophoneMode != InworldPackets::AudioSessionStartPayload_MicrophoneMode::AudioSessionStartPayload_MicrophoneMode_UNSPECIFIED)
		{
			Proto.mutable_control()->mutable_audio_session_start()->set_mode(_MicrophoneMode);
		}
        if (_UnderstandingMode != InworldPackets::AudioSessionStartPayload_UnderstandingMode::AudioSessionStartPayload_UnderstandingMode_UNSPECIFIED_UNDERSTANDING_MODE)
        {
            Proto.mutable_control()->mutable_audio_session_start()->set_understanding_mode(_UnderstandingMode);
        }
	}

	ControlEventConversationUpdate::ControlEventConversationUpdate(const InworldPackets::InworldPacket& GrpcPacket)
		: ControlEvent(GrpcPacket)
	{
    	const auto& ConvEvent = GrpcPacket.control().conversation_event();
    	for (const auto& Participant : ConvEvent.participants())
		{
			if (Participant.type() == InworldPackets::Actor_Type_AGENT)
			{
				_Agents.push_back(Participant.name());
			}
			else if (Participant.type() == InworldPackets::Actor_Type_PLAYER)
			{
				_bIncludePlayer = true;
			}
		}
    	_EventType = ConvEvent.event_type();
	}

	ControlEventConversationUpdate::ControlEventConversationUpdate(const std::string& ConversationId, const std::vector<std::string>& Agents, bool bIncludePlayer)
		: ControlEvent(InworldPackets::ControlEvent_Action_CONVERSATION_UPDATE, "",
			Routing::Player2Conversation(ConversationId.empty() ? RandomUUID() : ConversationId))
		, _Agents(Agents)
		, _bIncludePlayer(bIncludePlayer)
	{
	}

	void ControlEventConversationUpdate::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
        ControlEvent::ToProtoInternal(Proto);

        for (const auto& Participant : _Agents)
        {
            auto* ParticipantProto = Proto.mutable_control()->mutable_conversation_update()->add_participants();
            ParticipantProto->set_type(InworldPackets::Actor_Type_AGENT);
            ParticipantProto->set_name(Participant);
    		Inworld::Log("ControlEventConversationUpdate::ToProtoInternal. Add character '%s' to conversation '%s'", Participant.c_str(), _Routing._ConversationId.c_str());
        }

        if (_bIncludePlayer)
        {
            auto* ParticipantProto = Proto.mutable_control()->mutable_conversation_update()->add_participants();
            ParticipantProto->set_type(InworldPackets::Actor_Type_PLAYER);
    		Inworld::Log("ControlEventConversationUpdate::ToProtoInternal. Add player to conversation '%s'", _Routing._ConversationId.c_str());
        }
    }

	ControlEventCurrentSceneStatus::ControlEventCurrentSceneStatus(const InworldPackets::InworldPacket& GrpcPacket)
		: ControlEvent(GrpcPacket)
	{
		const auto& SceneStatus = GrpcPacket.control().current_scene_status();

		_SceneName = SceneStatus.scene_name();
		_SceneDescription = SceneStatus.scene_description();
		_SceneDisplayName = SceneStatus.scene_display_name();

		_AgentInfos.reserve(SceneStatus.agents_size());
		for (int32_t i = 0; i < SceneStatus.agents_size(); i++)
		{
			AgentInfo& Info = _AgentInfos.emplace_back();
			Info.BrainName = SceneStatus.agents(i).brain_name().c_str();
			Info.AgentId = SceneStatus.agents(i).agent_id().c_str();
			Info.GivenName = SceneStatus.agents(i).given_name().c_str();

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

	void CreateOrUpdateItemsOperationEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		auto* MutableCreateOrUpdateOperation = Proto.mutable_entities_items_operation()->mutable_create_or_update_items();
		for (const EntityItem& Item : _Items)
		{
			auto* MutableItem = MutableCreateOrUpdateOperation->add_items();
			MutableItem->set_id(Item.Id);
			MutableItem->set_display_name(Item.DisplayName);
			MutableItem->set_description(Item.Description);
			auto* MutableProperties = MutableItem->mutable_properties();
			for (const std::pair<std::string, std::string>& Property : Item.Properties)
			{
				MutableProperties->insert({ Property.first, Property.second });
			}
		}
		for (const std::string& AddToEntity : _AddToEntities)
		{
			auto* MutableAddToEntity = MutableCreateOrUpdateOperation->add_add_to_entities();
			*MutableAddToEntity = AddToEntity;
		}
	}

	void RemoveItemsOperationEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		auto* MutableRemoveItemsOperation = Proto.mutable_entities_items_operation()->mutable_remove_items();
		for (const std::string& ItemId : _ItemIds)
		{
			auto* MutableItemId = MutableRemoveItemsOperation->add_item_ids();
			*MutableItemId = ItemId;
		}
	}

	void ItemsInEntitiesOperationEventBase::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		auto* MutableItemsInEntitiesOperation = Proto.mutable_entities_items_operation()->mutable_items_in_entities();
		
		MutableItemsInEntitiesOperation->set_type(GetType());

		for (const std::string& ItemId : _ItemIds)
		{
			auto* MutableItemId = MutableItemsInEntitiesOperation->add_item_ids();
			*MutableItemId = ItemId;
		}

		for (const std::string& EntityName : _EntityNames)
		{
			auto* MutableEntityName = MutableItemsInEntitiesOperation->add_entity_names();
			*MutableEntityName = EntityName;
		}
	}

	void PongEvent::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		auto* MutablePingPong = Proto.mutable_latency_report()->mutable_ping_pong();
		MutablePingPong->set_type(InworldPackets::PingPongReport_Type_PONG);

		auto* MutablePingPacketId = MutablePingPong->mutable_ping_packet_id();
		MutablePingPacketId->set_packet_id(_PingPacketId._UID);
		MutablePingPacketId->set_utterance_id(_PingPacketId._UtteranceId);
		MutablePingPacketId->set_interaction_id(_PingPacketId._InteractionId);
		MutablePingPacketId->set_correlation_id(_PingPacketId._CorrelationId);

		*MutablePingPong->mutable_ping_timestamp() = 
			::google::protobuf_inworld::util::TimeUtil::TimeTToTimestamp(std::chrono::duration_cast<std::chrono::seconds>(_Timestamp.time_since_epoch()).count());
	}

	void PerceivedLatencyReportEventBase::ToProtoInternal(InworldPackets::InworldPacket& Proto) const
	{
		auto* MutablePerceivedLatencyReport = Proto.mutable_latency_report()->mutable_perceived_latency();
		MutablePerceivedLatencyReport->set_precision(GetType());

		*MutablePerceivedLatencyReport->mutable_latency() = ::google::protobuf_inworld::util::TimeUtil::MillisecondsToDuration(_Duration);
	}

	LogEvent::LogEvent(const InworldPackets::InworldPacket& GrpcPacket) 
		: Packet(GrpcPacket)
			, _LogLevel(GrpcPacket.log().level())
	{ 
		const int indent = 4;
		std::stringstream ss;
		for(const InworldPackets::LogsEvent_LogDetail& logDetail : GrpcPacket.log().details()) {
			ss << std::string(indent, ' ') << logDetail.text() << ": ";
			std::string jsonOutput;
			google::protobuf_inworld::util::MessageToJsonString(logDetail.detail(), &jsonOutput);
			ss << jsonOutput << std::endl;
		}
		_Text = GrpcPacket.log().text() + "\n" + ss.str();
	}

}
