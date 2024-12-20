#include "ProtoDisableWarning.h"
// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: nvidia/a2f/nvidia_ace.audio.v1.proto

#include "nvidia/a2f/nvidia_ace.audio.v1.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
namespace nvidia_ace {
namespace audio {
namespace v1 {
class AudioHeaderDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<AudioHeader> _instance;
} _AudioHeader_default_instance_;
}  // namespace v1
}  // namespace audio
}  // namespace nvidia_ace
static void InitDefaultsscc_info_AudioHeader_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::nvidia_ace::audio::v1::_AudioHeader_default_instance_;
    new (ptr) ::nvidia_ace::audio::v1::AudioHeader();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_AudioHeader_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_AudioHeader_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto[1];
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::nvidia_ace::audio::v1::AudioHeader, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::nvidia_ace::audio::v1::AudioHeader, audio_format_),
  PROTOBUF_FIELD_OFFSET(::nvidia_ace::audio::v1::AudioHeader, channel_count_),
  PROTOBUF_FIELD_OFFSET(::nvidia_ace::audio::v1::AudioHeader, samples_per_second_),
  PROTOBUF_FIELD_OFFSET(::nvidia_ace::audio::v1::AudioHeader, bits_per_sample_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::nvidia_ace::audio::v1::AudioHeader)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::nvidia_ace::audio::v1::_AudioHeader_default_instance_),
};

const char descriptor_table_protodef_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n$nvidia/a2f/nvidia_ace.audio.v1.proto\022\023"
  "nvidia_ace.audio.v1\"\302\001\n\013AudioHeader\022B\n\014a"
  "udio_format\030\001 \001(\0162,.nvidia_ace.audio.v1."
  "AudioHeader.AudioFormat\022\025\n\rchannel_count"
  "\030\002 \001(\r\022\032\n\022samples_per_second\030\003 \001(\r\022\027\n\017bi"
  "ts_per_sample\030\004 \001(\r\"#\n\013AudioFormat\022\024\n\020AU"
  "DIO_FORMAT_PCM\020\000b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto_sccs[1] = {
  &scc_info_AudioHeader_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto = {
  false, false, descriptor_table_protodef_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto, "nvidia/a2f/nvidia_ace.audio.v1.proto", 264,
  &descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto_once, descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto_sccs, descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto_deps, 1, 0,
  schemas, file_default_instances, TableStruct_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto::offsets,
  file_level_metadata_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto, 1, file_level_enum_descriptors_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto, file_level_service_descriptors_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto = (static_cast<void>(::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto)), true);
namespace nvidia_ace {
namespace audio {
namespace v1 {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* AudioHeader_AudioFormat_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto);
  return file_level_enum_descriptors_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto[0];
}
bool AudioHeader_AudioFormat_IsValid(int value) {
  switch (value) {
    case 0:
      return true;
    default:
      return false;
  }
}

#if (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)
constexpr AudioHeader_AudioFormat AudioHeader::AUDIO_FORMAT_PCM;
constexpr AudioHeader_AudioFormat AudioHeader::AudioFormat_MIN;
constexpr AudioHeader_AudioFormat AudioHeader::AudioFormat_MAX;
constexpr int AudioHeader::AudioFormat_ARRAYSIZE;
#endif  // (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)

// ===================================================================

class AudioHeader::_Internal {
 public:
};

AudioHeader::AudioHeader(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:nvidia_ace.audio.v1.AudioHeader)
}
AudioHeader::AudioHeader(const AudioHeader& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&audio_format_, &from.audio_format_,
    static_cast<size_t>(reinterpret_cast<char*>(&bits_per_sample_) -
    reinterpret_cast<char*>(&audio_format_)) + sizeof(bits_per_sample_));
  // @@protoc_insertion_point(copy_constructor:nvidia_ace.audio.v1.AudioHeader)
}

void AudioHeader::SharedCtor() {
  ::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
      reinterpret_cast<char*>(&audio_format_) - reinterpret_cast<char*>(this)),
      0, static_cast<size_t>(reinterpret_cast<char*>(&bits_per_sample_) -
      reinterpret_cast<char*>(&audio_format_)) + sizeof(bits_per_sample_));
}

AudioHeader::~AudioHeader() {
  // @@protoc_insertion_point(destructor:nvidia_ace.audio.v1.AudioHeader)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void AudioHeader::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void AudioHeader::ArenaDtor(void* object) {
  AudioHeader* _this = reinterpret_cast< AudioHeader* >(object);
  (void)_this;
}
void AudioHeader::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void AudioHeader::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const AudioHeader& AudioHeader::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_AudioHeader_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto.base);
  return *internal_default_instance();
}


void AudioHeader::Clear() {
// @@protoc_insertion_point(message_clear_start:nvidia_ace.audio.v1.AudioHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&audio_format_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&bits_per_sample_) -
      reinterpret_cast<char*>(&audio_format_)) + sizeof(bits_per_sample_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* AudioHeader::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // .nvidia_ace.audio.v1.AudioHeader.AudioFormat audio_format = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_audio_format(static_cast<::nvidia_ace::audio::v1::AudioHeader_AudioFormat>(val));
        } else goto handle_unusual;
        continue;
      // uint32 channel_count = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          channel_count_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint32 samples_per_second = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          samples_per_second_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint32 bits_per_sample = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          bits_per_sample_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* AudioHeader::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:nvidia_ace.audio.v1.AudioHeader)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .nvidia_ace.audio.v1.AudioHeader.AudioFormat audio_format = 1;
  if (this->audio_format() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      1, this->_internal_audio_format(), target);
  }

  // uint32 channel_count = 2;
  if (this->channel_count() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_channel_count(), target);
  }

  // uint32 samples_per_second = 3;
  if (this->samples_per_second() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(3, this->_internal_samples_per_second(), target);
  }

  // uint32 bits_per_sample = 4;
  if (this->bits_per_sample() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(4, this->_internal_bits_per_sample(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:nvidia_ace.audio.v1.AudioHeader)
  return target;
}

size_t AudioHeader::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:nvidia_ace.audio.v1.AudioHeader)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // .nvidia_ace.audio.v1.AudioHeader.AudioFormat audio_format = 1;
  if (this->audio_format() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_audio_format());
  }

  // uint32 channel_count = 2;
  if (this->channel_count() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_channel_count());
  }

  // uint32 samples_per_second = 3;
  if (this->samples_per_second() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_samples_per_second());
  }

  // uint32 bits_per_sample = 4;
  if (this->bits_per_sample() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_bits_per_sample());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void AudioHeader::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:nvidia_ace.audio.v1.AudioHeader)
  GOOGLE_DCHECK_NE(&from, this);
  const AudioHeader* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<AudioHeader>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:nvidia_ace.audio.v1.AudioHeader)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:nvidia_ace.audio.v1.AudioHeader)
    MergeFrom(*source);
  }
}

void AudioHeader::MergeFrom(const AudioHeader& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:nvidia_ace.audio.v1.AudioHeader)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.audio_format() != 0) {
    _internal_set_audio_format(from._internal_audio_format());
  }
  if (from.channel_count() != 0) {
    _internal_set_channel_count(from._internal_channel_count());
  }
  if (from.samples_per_second() != 0) {
    _internal_set_samples_per_second(from._internal_samples_per_second());
  }
  if (from.bits_per_sample() != 0) {
    _internal_set_bits_per_sample(from._internal_bits_per_sample());
  }
}

void AudioHeader::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:nvidia_ace.audio.v1.AudioHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void AudioHeader::CopyFrom(const AudioHeader& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:nvidia_ace.audio.v1.AudioHeader)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool AudioHeader::IsInitialized() const {
  return true;
}

void AudioHeader::InternalSwap(AudioHeader* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(AudioHeader, bits_per_sample_)
      + sizeof(AudioHeader::bits_per_sample_)
      - PROTOBUF_FIELD_OFFSET(AudioHeader, audio_format_)>(
          reinterpret_cast<char*>(&audio_format_),
          reinterpret_cast<char*>(&other->audio_format_));
}

::PROTOBUF_NAMESPACE_ID::Metadata AudioHeader::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace v1
}  // namespace audio
}  // namespace nvidia_ace
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::nvidia_ace::audio::v1::AudioHeader* Arena::CreateMaybeMessage< ::nvidia_ace::audio::v1::AudioHeader >(Arena* arena) {
  return Arena::CreateMessageInternal< ::nvidia_ace::audio::v1::AudioHeader >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
