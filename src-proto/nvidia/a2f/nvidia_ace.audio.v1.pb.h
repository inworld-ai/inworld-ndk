// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: nvidia/a2f/nvidia_ace.audio.v1.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3014000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3014000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto;
namespace nvidia_ace {
namespace audio {
namespace v1 {
class AudioHeader;
class AudioHeaderDefaultTypeInternal;
extern AudioHeaderDefaultTypeInternal _AudioHeader_default_instance_;
}  // namespace v1
}  // namespace audio
}  // namespace nvidia_ace
PROTOBUF_NAMESPACE_OPEN
template<> ::nvidia_ace::audio::v1::AudioHeader* Arena::CreateMaybeMessage<::nvidia_ace::audio::v1::AudioHeader>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace nvidia_ace {
namespace audio {
namespace v1 {

enum AudioHeader_AudioFormat : int {
  AudioHeader_AudioFormat_AUDIO_FORMAT_PCM = 0,
  AudioHeader_AudioFormat_AudioHeader_AudioFormat_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  AudioHeader_AudioFormat_AudioHeader_AudioFormat_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool AudioHeader_AudioFormat_IsValid(int value);
constexpr AudioHeader_AudioFormat AudioHeader_AudioFormat_AudioFormat_MIN = AudioHeader_AudioFormat_AUDIO_FORMAT_PCM;
constexpr AudioHeader_AudioFormat AudioHeader_AudioFormat_AudioFormat_MAX = AudioHeader_AudioFormat_AUDIO_FORMAT_PCM;
constexpr int AudioHeader_AudioFormat_AudioFormat_ARRAYSIZE = AudioHeader_AudioFormat_AudioFormat_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* AudioHeader_AudioFormat_descriptor();
template<typename T>
inline const std::string& AudioHeader_AudioFormat_Name(T enum_t_value) {
  static_assert(::std::is_same<T, AudioHeader_AudioFormat>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function AudioHeader_AudioFormat_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    AudioHeader_AudioFormat_descriptor(), enum_t_value);
}
inline bool AudioHeader_AudioFormat_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, AudioHeader_AudioFormat* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<AudioHeader_AudioFormat>(
    AudioHeader_AudioFormat_descriptor(), name, value);
}
// ===================================================================

class AudioHeader PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:nvidia_ace.audio.v1.AudioHeader) */ {
 public:
  inline AudioHeader() : AudioHeader(nullptr) {}
  virtual ~AudioHeader();

  AudioHeader(const AudioHeader& from);
  AudioHeader(AudioHeader&& from) noexcept
    : AudioHeader() {
    *this = ::std::move(from);
  }

  inline AudioHeader& operator=(const AudioHeader& from) {
    CopyFrom(from);
    return *this;
  }
  inline AudioHeader& operator=(AudioHeader&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const AudioHeader& default_instance();

  static inline const AudioHeader* internal_default_instance() {
    return reinterpret_cast<const AudioHeader*>(
               &_AudioHeader_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(AudioHeader& a, AudioHeader& b) {
    a.Swap(&b);
  }
  inline void Swap(AudioHeader* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(AudioHeader* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline AudioHeader* New() const final {
    return CreateMaybeMessage<AudioHeader>(nullptr);
  }

  AudioHeader* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<AudioHeader>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const AudioHeader& from);
  void MergeFrom(const AudioHeader& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(AudioHeader* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "nvidia_ace.audio.v1.AudioHeader";
  }
  protected:
  explicit AudioHeader(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto);
    return ::descriptor_table_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  typedef AudioHeader_AudioFormat AudioFormat;
  static constexpr AudioFormat AUDIO_FORMAT_PCM =
    AudioHeader_AudioFormat_AUDIO_FORMAT_PCM;
  static inline bool AudioFormat_IsValid(int value) {
    return AudioHeader_AudioFormat_IsValid(value);
  }
  static constexpr AudioFormat AudioFormat_MIN =
    AudioHeader_AudioFormat_AudioFormat_MIN;
  static constexpr AudioFormat AudioFormat_MAX =
    AudioHeader_AudioFormat_AudioFormat_MAX;
  static constexpr int AudioFormat_ARRAYSIZE =
    AudioHeader_AudioFormat_AudioFormat_ARRAYSIZE;
  static inline const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor*
  AudioFormat_descriptor() {
    return AudioHeader_AudioFormat_descriptor();
  }
  template<typename T>
  static inline const std::string& AudioFormat_Name(T enum_t_value) {
    static_assert(::std::is_same<T, AudioFormat>::value ||
      ::std::is_integral<T>::value,
      "Incorrect type passed to function AudioFormat_Name.");
    return AudioHeader_AudioFormat_Name(enum_t_value);
  }
  static inline bool AudioFormat_Parse(::PROTOBUF_NAMESPACE_ID::ConstStringParam name,
      AudioFormat* value) {
    return AudioHeader_AudioFormat_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  enum : int {
    kAudioFormatFieldNumber = 1,
    kChannelCountFieldNumber = 2,
    kSamplesPerSecondFieldNumber = 3,
    kBitsPerSampleFieldNumber = 4,
  };
  // .nvidia_ace.audio.v1.AudioHeader.AudioFormat audio_format = 1;
  void clear_audio_format();
  ::nvidia_ace::audio::v1::AudioHeader_AudioFormat audio_format() const;
  void set_audio_format(::nvidia_ace::audio::v1::AudioHeader_AudioFormat value);
  private:
  ::nvidia_ace::audio::v1::AudioHeader_AudioFormat _internal_audio_format() const;
  void _internal_set_audio_format(::nvidia_ace::audio::v1::AudioHeader_AudioFormat value);
  public:

  // uint32 channel_count = 2;
  void clear_channel_count();
  ::PROTOBUF_NAMESPACE_ID::uint32 channel_count() const;
  void set_channel_count(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_channel_count() const;
  void _internal_set_channel_count(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 samples_per_second = 3;
  void clear_samples_per_second();
  ::PROTOBUF_NAMESPACE_ID::uint32 samples_per_second() const;
  void set_samples_per_second(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_samples_per_second() const;
  void _internal_set_samples_per_second(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 bits_per_sample = 4;
  void clear_bits_per_sample();
  ::PROTOBUF_NAMESPACE_ID::uint32 bits_per_sample() const;
  void set_bits_per_sample(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_bits_per_sample() const;
  void _internal_set_bits_per_sample(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // @@protoc_insertion_point(class_scope:nvidia_ace.audio.v1.AudioHeader)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  int audio_format_;
  ::PROTOBUF_NAMESPACE_ID::uint32 channel_count_;
  ::PROTOBUF_NAMESPACE_ID::uint32 samples_per_second_;
  ::PROTOBUF_NAMESPACE_ID::uint32 bits_per_sample_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// AudioHeader

// .nvidia_ace.audio.v1.AudioHeader.AudioFormat audio_format = 1;
inline void AudioHeader::clear_audio_format() {
  audio_format_ = 0;
}
inline ::nvidia_ace::audio::v1::AudioHeader_AudioFormat AudioHeader::_internal_audio_format() const {
  return static_cast< ::nvidia_ace::audio::v1::AudioHeader_AudioFormat >(audio_format_);
}
inline ::nvidia_ace::audio::v1::AudioHeader_AudioFormat AudioHeader::audio_format() const {
  // @@protoc_insertion_point(field_get:nvidia_ace.audio.v1.AudioHeader.audio_format)
  return _internal_audio_format();
}
inline void AudioHeader::_internal_set_audio_format(::nvidia_ace::audio::v1::AudioHeader_AudioFormat value) {
  
  audio_format_ = value;
}
inline void AudioHeader::set_audio_format(::nvidia_ace::audio::v1::AudioHeader_AudioFormat value) {
  _internal_set_audio_format(value);
  // @@protoc_insertion_point(field_set:nvidia_ace.audio.v1.AudioHeader.audio_format)
}

// uint32 channel_count = 2;
inline void AudioHeader::clear_channel_count() {
  channel_count_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 AudioHeader::_internal_channel_count() const {
  return channel_count_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 AudioHeader::channel_count() const {
  // @@protoc_insertion_point(field_get:nvidia_ace.audio.v1.AudioHeader.channel_count)
  return _internal_channel_count();
}
inline void AudioHeader::_internal_set_channel_count(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  channel_count_ = value;
}
inline void AudioHeader::set_channel_count(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_channel_count(value);
  // @@protoc_insertion_point(field_set:nvidia_ace.audio.v1.AudioHeader.channel_count)
}

// uint32 samples_per_second = 3;
inline void AudioHeader::clear_samples_per_second() {
  samples_per_second_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 AudioHeader::_internal_samples_per_second() const {
  return samples_per_second_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 AudioHeader::samples_per_second() const {
  // @@protoc_insertion_point(field_get:nvidia_ace.audio.v1.AudioHeader.samples_per_second)
  return _internal_samples_per_second();
}
inline void AudioHeader::_internal_set_samples_per_second(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  samples_per_second_ = value;
}
inline void AudioHeader::set_samples_per_second(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_samples_per_second(value);
  // @@protoc_insertion_point(field_set:nvidia_ace.audio.v1.AudioHeader.samples_per_second)
}

// uint32 bits_per_sample = 4;
inline void AudioHeader::clear_bits_per_sample() {
  bits_per_sample_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 AudioHeader::_internal_bits_per_sample() const {
  return bits_per_sample_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 AudioHeader::bits_per_sample() const {
  // @@protoc_insertion_point(field_get:nvidia_ace.audio.v1.AudioHeader.bits_per_sample)
  return _internal_bits_per_sample();
}
inline void AudioHeader::_internal_set_bits_per_sample(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  bits_per_sample_ = value;
}
inline void AudioHeader::set_bits_per_sample(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_bits_per_sample(value);
  // @@protoc_insertion_point(field_set:nvidia_ace.audio.v1.AudioHeader.bits_per_sample)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace audio
}  // namespace nvidia_ace

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::nvidia_ace::audio::v1::AudioHeader_AudioFormat> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::nvidia_ace::audio::v1::AudioHeader_AudioFormat>() {
  return ::nvidia_ace::audio::v1::AudioHeader_AudioFormat_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_nvidia_2fa2f_2fnvidia_5face_2eaudio_2ev1_2eproto
