// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: nvidia/animation-common-interface-proto/nvidia.ace.status.v1.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_nvidia_2fanimation_2dcommon_2dinterface_2dproto_2fnvidia_2eace_2estatus_2ev1_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_nvidia_2fanimation_2dcommon_2dinterface_2dproto_2fnvidia_2eace_2estatus_2ev1_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_nvidia_2fanimation_2dcommon_2dinterface_2dproto_2fnvidia_2eace_2estatus_2ev1_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_nvidia_2fanimation_2dcommon_2dinterface_2dproto_2fnvidia_2eace_2estatus_2ev1_2eproto {
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
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_nvidia_2fanimation_2dcommon_2dinterface_2dproto_2fnvidia_2eace_2estatus_2ev1_2eproto;
namespace nvidia {
namespace ace {
namespace status {
namespace v1 {
class Status;
class StatusDefaultTypeInternal;
extern StatusDefaultTypeInternal _Status_default_instance_;
}  // namespace v1
}  // namespace status
}  // namespace ace
}  // namespace nvidia
PROTOBUF_NAMESPACE_OPEN
template<> ::nvidia::ace::status::v1::Status* Arena::CreateMaybeMessage<::nvidia::ace::status::v1::Status>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace nvidia {
namespace ace {
namespace status {
namespace v1 {

enum Status_Code : int {
  Status_Code_SUCCESS = 0,
  Status_Code_INFO = 1,
  Status_Code_WARNING = 2,
  Status_Code_ERROR = 3,
  Status_Code_Status_Code_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  Status_Code_Status_Code_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool Status_Code_IsValid(int value);
constexpr Status_Code Status_Code_Code_MIN = Status_Code_SUCCESS;
constexpr Status_Code Status_Code_Code_MAX = Status_Code_ERROR;
constexpr int Status_Code_Code_ARRAYSIZE = Status_Code_Code_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* Status_Code_descriptor();
template<typename T>
inline const std::string& Status_Code_Name(T enum_t_value) {
  static_assert(::std::is_same<T, Status_Code>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function Status_Code_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    Status_Code_descriptor(), enum_t_value);
}
inline bool Status_Code_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, Status_Code* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<Status_Code>(
    Status_Code_descriptor(), name, value);
}
// ===================================================================

class Status PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:nvidia.ace.status.v1.Status) */ {
 public:
  inline Status() : Status(nullptr) {}
  virtual ~Status();

  Status(const Status& from);
  Status(Status&& from) noexcept
    : Status() {
    *this = ::std::move(from);
  }

  inline Status& operator=(const Status& from) {
    CopyFrom(from);
    return *this;
  }
  inline Status& operator=(Status&& from) noexcept {
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
  static const Status& default_instance();

  static inline const Status* internal_default_instance() {
    return reinterpret_cast<const Status*>(
               &_Status_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Status& a, Status& b) {
    a.Swap(&b);
  }
  inline void Swap(Status* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Status* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Status* New() const final {
    return CreateMaybeMessage<Status>(nullptr);
  }

  Status* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Status>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const Status& from);
  void MergeFrom(const Status& from);
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
  void InternalSwap(Status* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "nvidia.ace.status.v1.Status";
  }
  protected:
  explicit Status(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_nvidia_2fanimation_2dcommon_2dinterface_2dproto_2fnvidia_2eace_2estatus_2ev1_2eproto);
    return ::descriptor_table_nvidia_2fanimation_2dcommon_2dinterface_2dproto_2fnvidia_2eace_2estatus_2ev1_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  typedef Status_Code Code;
  static constexpr Code SUCCESS =
    Status_Code_SUCCESS;
  static constexpr Code INFO =
    Status_Code_INFO;
  static constexpr Code WARNING =
    Status_Code_WARNING;
  static constexpr Code ERROR =
    Status_Code_ERROR;
  static inline bool Code_IsValid(int value) {
    return Status_Code_IsValid(value);
  }
  static constexpr Code Code_MIN =
    Status_Code_Code_MIN;
  static constexpr Code Code_MAX =
    Status_Code_Code_MAX;
  static constexpr int Code_ARRAYSIZE =
    Status_Code_Code_ARRAYSIZE;
  static inline const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor*
  Code_descriptor() {
    return Status_Code_descriptor();
  }
  template<typename T>
  static inline const std::string& Code_Name(T enum_t_value) {
    static_assert(::std::is_same<T, Code>::value ||
      ::std::is_integral<T>::value,
      "Incorrect type passed to function Code_Name.");
    return Status_Code_Name(enum_t_value);
  }
  static inline bool Code_Parse(::PROTOBUF_NAMESPACE_ID::ConstStringParam name,
      Code* value) {
    return Status_Code_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  enum : int {
    kMessageFieldNumber = 2,
    kCodeFieldNumber = 1,
  };
  // string message = 2;
  void clear_message();
  const std::string& message() const;
  void set_message(const std::string& value);
  void set_message(std::string&& value);
  void set_message(const char* value);
  void set_message(const char* value, size_t size);
  std::string* mutable_message();
  std::string* release_message();
  void set_allocated_message(std::string* message);
  private:
  const std::string& _internal_message() const;
  void _internal_set_message(const std::string& value);
  std::string* _internal_mutable_message();
  public:

  // .nvidia.ace.status.v1.Status.Code code = 1;
  void clear_code();
  ::nvidia::ace::status::v1::Status_Code code() const;
  void set_code(::nvidia::ace::status::v1::Status_Code value);
  private:
  ::nvidia::ace::status::v1::Status_Code _internal_code() const;
  void _internal_set_code(::nvidia::ace::status::v1::Status_Code value);
  public:

  // @@protoc_insertion_point(class_scope:nvidia.ace.status.v1.Status)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr message_;
  int code_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_nvidia_2fanimation_2dcommon_2dinterface_2dproto_2fnvidia_2eace_2estatus_2ev1_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Status

// .nvidia.ace.status.v1.Status.Code code = 1;
inline void Status::clear_code() {
  code_ = 0;
}
inline ::nvidia::ace::status::v1::Status_Code Status::_internal_code() const {
  return static_cast< ::nvidia::ace::status::v1::Status_Code >(code_);
}
inline ::nvidia::ace::status::v1::Status_Code Status::code() const {
  // @@protoc_insertion_point(field_get:nvidia.ace.status.v1.Status.code)
  return _internal_code();
}
inline void Status::_internal_set_code(::nvidia::ace::status::v1::Status_Code value) {
  
  code_ = value;
}
inline void Status::set_code(::nvidia::ace::status::v1::Status_Code value) {
  _internal_set_code(value);
  // @@protoc_insertion_point(field_set:nvidia.ace.status.v1.Status.code)
}

// string message = 2;
inline void Status::clear_message() {
  message_.ClearToEmpty();
}
inline const std::string& Status::message() const {
  // @@protoc_insertion_point(field_get:nvidia.ace.status.v1.Status.message)
  return _internal_message();
}
inline void Status::set_message(const std::string& value) {
  _internal_set_message(value);
  // @@protoc_insertion_point(field_set:nvidia.ace.status.v1.Status.message)
}
inline std::string* Status::mutable_message() {
  // @@protoc_insertion_point(field_mutable:nvidia.ace.status.v1.Status.message)
  return _internal_mutable_message();
}
inline const std::string& Status::_internal_message() const {
  return message_.Get();
}
inline void Status::_internal_set_message(const std::string& value) {
  
  message_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void Status::set_message(std::string&& value) {
  
  message_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:nvidia.ace.status.v1.Status.message)
}
inline void Status::set_message(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  message_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:nvidia.ace.status.v1.Status.message)
}
inline void Status::set_message(const char* value,
    size_t size) {
  
  message_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:nvidia.ace.status.v1.Status.message)
}
inline std::string* Status::_internal_mutable_message() {
  
  return message_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* Status::release_message() {
  // @@protoc_insertion_point(field_release:nvidia.ace.status.v1.Status.message)
  return message_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void Status::set_allocated_message(std::string* message) {
  if (message != nullptr) {
    
  } else {
    
  }
  message_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), message,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:nvidia.ace.status.v1.Status.message)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace status
}  // namespace ace
}  // namespace nvidia

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::nvidia::ace::status::v1::Status_Code> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::nvidia::ace::status::v1::Status_Code>() {
  return ::nvidia::ace::status::v1::Status_Code_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_nvidia_2fanimation_2dcommon_2dinterface_2dproto_2fnvidia_2eace_2estatus_2ev1_2eproto