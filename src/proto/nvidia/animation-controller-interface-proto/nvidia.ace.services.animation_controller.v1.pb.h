// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: nvidia/animation-controller-interface-proto/nvidia.ace.services.animation_controller.v1.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto

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
#include <google/protobuf/unknown_field_set.h>
#include "nvidia/animation-controller-interface-proto/nvidia.ace.controller.v1.pb.h"
#include "nvidia/animation-common-interface-proto/nvidia.ace.status.v1.pb.h"
#include "nvidia/animation-common-interface-proto/nvidia.ace.animation_id.v1.pb.h"
#include "nvidia/animation-common-interface-proto/nvidia.ace.animation_data.v1.pb.h"
#include "nvidia/a2x-interface-proto/nvidia.ace.a2x.v1.pb.h"
#include <google/protobuf/empty.pb.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto;
namespace nvidia {
namespace ace {
namespace services {
namespace animation_controller {
namespace v1 {
class AnimationGraphRequest;
class AnimationGraphRequestDefaultTypeInternal;
extern AnimationGraphRequestDefaultTypeInternal _AnimationGraphRequest_default_instance_;
class AnimationIdsOrStatus;
class AnimationIdsOrStatusDefaultTypeInternal;
extern AnimationIdsOrStatusDefaultTypeInternal _AnimationIdsOrStatus_default_instance_;
}  // namespace v1
}  // namespace animation_controller
}  // namespace services
}  // namespace ace
}  // namespace nvidia
PROTOBUF_NAMESPACE_OPEN
template<> ::nvidia::ace::services::animation_controller::v1::AnimationGraphRequest* Arena::CreateMaybeMessage<::nvidia::ace::services::animation_controller::v1::AnimationGraphRequest>(Arena*);
template<> ::nvidia::ace::services::animation_controller::v1::AnimationIdsOrStatus* Arena::CreateMaybeMessage<::nvidia::ace::services::animation_controller::v1::AnimationIdsOrStatus>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace nvidia {
namespace ace {
namespace services {
namespace animation_controller {
namespace v1 {

// ===================================================================

class AnimationGraphRequest PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest) */ {
 public:
  inline AnimationGraphRequest() : AnimationGraphRequest(nullptr) {}
  virtual ~AnimationGraphRequest();

  AnimationGraphRequest(const AnimationGraphRequest& from);
  AnimationGraphRequest(AnimationGraphRequest&& from) noexcept
    : AnimationGraphRequest() {
    *this = ::std::move(from);
  }

  inline AnimationGraphRequest& operator=(const AnimationGraphRequest& from) {
    CopyFrom(from);
    return *this;
  }
  inline AnimationGraphRequest& operator=(AnimationGraphRequest&& from) noexcept {
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
  static const AnimationGraphRequest& default_instance();

  static inline const AnimationGraphRequest* internal_default_instance() {
    return reinterpret_cast<const AnimationGraphRequest*>(
               &_AnimationGraphRequest_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(AnimationGraphRequest& a, AnimationGraphRequest& b) {
    a.Swap(&b);
  }
  inline void Swap(AnimationGraphRequest* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(AnimationGraphRequest* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline AnimationGraphRequest* New() const final {
    return CreateMaybeMessage<AnimationGraphRequest>(nullptr);
  }

  AnimationGraphRequest* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<AnimationGraphRequest>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const AnimationGraphRequest& from);
  void MergeFrom(const AnimationGraphRequest& from);
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
  void InternalSwap(AnimationGraphRequest* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "nvidia.ace.services.animation_controller.v1.AnimationGraphRequest";
  }
  protected:
  explicit AnimationGraphRequest(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto);
    return ::descriptor_table_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kAnimationGraphVariableNameFieldNumber = 2,
    kAnimationGraphVariableValueFieldNumber = 3,
    kAnimationIdsFieldNumber = 1,
  };
  // string animation_graph_variable_name = 2;
  void clear_animation_graph_variable_name();
  const std::string& animation_graph_variable_name() const;
  void set_animation_graph_variable_name(const std::string& value);
  void set_animation_graph_variable_name(std::string&& value);
  void set_animation_graph_variable_name(const char* value);
  void set_animation_graph_variable_name(const char* value, size_t size);
  std::string* mutable_animation_graph_variable_name();
  std::string* release_animation_graph_variable_name();
  void set_allocated_animation_graph_variable_name(std::string* animation_graph_variable_name);
  private:
  const std::string& _internal_animation_graph_variable_name() const;
  void _internal_set_animation_graph_variable_name(const std::string& value);
  std::string* _internal_mutable_animation_graph_variable_name();
  public:

  // string animation_graph_variable_value = 3;
  void clear_animation_graph_variable_value();
  const std::string& animation_graph_variable_value() const;
  void set_animation_graph_variable_value(const std::string& value);
  void set_animation_graph_variable_value(std::string&& value);
  void set_animation_graph_variable_value(const char* value);
  void set_animation_graph_variable_value(const char* value, size_t size);
  std::string* mutable_animation_graph_variable_value();
  std::string* release_animation_graph_variable_value();
  void set_allocated_animation_graph_variable_value(std::string* animation_graph_variable_value);
  private:
  const std::string& _internal_animation_graph_variable_value() const;
  void _internal_set_animation_graph_variable_value(const std::string& value);
  std::string* _internal_mutable_animation_graph_variable_value();
  public:

  // .nvidia.ace.animation_id.v1.AnimationIds animation_ids = 1;
  bool has_animation_ids() const;
  private:
  bool _internal_has_animation_ids() const;
  public:
  void clear_animation_ids();
  const ::nvidia::ace::animation_id::v1::AnimationIds& animation_ids() const;
  ::nvidia::ace::animation_id::v1::AnimationIds* release_animation_ids();
  ::nvidia::ace::animation_id::v1::AnimationIds* mutable_animation_ids();
  void set_allocated_animation_ids(::nvidia::ace::animation_id::v1::AnimationIds* animation_ids);
  private:
  const ::nvidia::ace::animation_id::v1::AnimationIds& _internal_animation_ids() const;
  ::nvidia::ace::animation_id::v1::AnimationIds* _internal_mutable_animation_ids();
  public:
  void unsafe_arena_set_allocated_animation_ids(
      ::nvidia::ace::animation_id::v1::AnimationIds* animation_ids);
  ::nvidia::ace::animation_id::v1::AnimationIds* unsafe_arena_release_animation_ids();

  // @@protoc_insertion_point(class_scope:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr animation_graph_variable_name_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr animation_graph_variable_value_;
  ::nvidia::ace::animation_id::v1::AnimationIds* animation_ids_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto;
};
// -------------------------------------------------------------------

class AnimationIdsOrStatus PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus) */ {
 public:
  inline AnimationIdsOrStatus() : AnimationIdsOrStatus(nullptr) {}
  virtual ~AnimationIdsOrStatus();

  AnimationIdsOrStatus(const AnimationIdsOrStatus& from);
  AnimationIdsOrStatus(AnimationIdsOrStatus&& from) noexcept
    : AnimationIdsOrStatus() {
    *this = ::std::move(from);
  }

  inline AnimationIdsOrStatus& operator=(const AnimationIdsOrStatus& from) {
    CopyFrom(from);
    return *this;
  }
  inline AnimationIdsOrStatus& operator=(AnimationIdsOrStatus&& from) noexcept {
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
  static const AnimationIdsOrStatus& default_instance();

  enum ResponseCase {
    kAnimationIds = 1,
    kStatus = 2,
    RESPONSE_NOT_SET = 0,
  };

  static inline const AnimationIdsOrStatus* internal_default_instance() {
    return reinterpret_cast<const AnimationIdsOrStatus*>(
               &_AnimationIdsOrStatus_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(AnimationIdsOrStatus& a, AnimationIdsOrStatus& b) {
    a.Swap(&b);
  }
  inline void Swap(AnimationIdsOrStatus* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(AnimationIdsOrStatus* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline AnimationIdsOrStatus* New() const final {
    return CreateMaybeMessage<AnimationIdsOrStatus>(nullptr);
  }

  AnimationIdsOrStatus* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<AnimationIdsOrStatus>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const AnimationIdsOrStatus& from);
  void MergeFrom(const AnimationIdsOrStatus& from);
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
  void InternalSwap(AnimationIdsOrStatus* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus";
  }
  protected:
  explicit AnimationIdsOrStatus(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto);
    return ::descriptor_table_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kAnimationIdsFieldNumber = 1,
    kStatusFieldNumber = 2,
  };
  // .nvidia.ace.animation_id.v1.AnimationIds animation_ids = 1;
  bool has_animation_ids() const;
  private:
  bool _internal_has_animation_ids() const;
  public:
  void clear_animation_ids();
  const ::nvidia::ace::animation_id::v1::AnimationIds& animation_ids() const;
  ::nvidia::ace::animation_id::v1::AnimationIds* release_animation_ids();
  ::nvidia::ace::animation_id::v1::AnimationIds* mutable_animation_ids();
  void set_allocated_animation_ids(::nvidia::ace::animation_id::v1::AnimationIds* animation_ids);
  private:
  const ::nvidia::ace::animation_id::v1::AnimationIds& _internal_animation_ids() const;
  ::nvidia::ace::animation_id::v1::AnimationIds* _internal_mutable_animation_ids();
  public:
  void unsafe_arena_set_allocated_animation_ids(
      ::nvidia::ace::animation_id::v1::AnimationIds* animation_ids);
  ::nvidia::ace::animation_id::v1::AnimationIds* unsafe_arena_release_animation_ids();

  // .nvidia.ace.status.v1.Status status = 2;
  bool has_status() const;
  private:
  bool _internal_has_status() const;
  public:
  void clear_status();
  const ::nvidia::ace::status::v1::Status& status() const;
  ::nvidia::ace::status::v1::Status* release_status();
  ::nvidia::ace::status::v1::Status* mutable_status();
  void set_allocated_status(::nvidia::ace::status::v1::Status* status);
  private:
  const ::nvidia::ace::status::v1::Status& _internal_status() const;
  ::nvidia::ace::status::v1::Status* _internal_mutable_status();
  public:
  void unsafe_arena_set_allocated_status(
      ::nvidia::ace::status::v1::Status* status);
  ::nvidia::ace::status::v1::Status* unsafe_arena_release_status();

  void clear_response();
  ResponseCase response_case() const;
  // @@protoc_insertion_point(class_scope:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus)
 private:
  class _Internal;
  void set_has_animation_ids();
  void set_has_status();

  inline bool has_response() const;
  inline void clear_has_response();

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  union ResponseUnion {
    ResponseUnion() {}
    ::nvidia::ace::animation_id::v1::AnimationIds* animation_ids_;
    ::nvidia::ace::status::v1::Status* status_;
  } response_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::uint32 _oneof_case_[1];

  friend struct ::TableStruct_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// AnimationGraphRequest

// .nvidia.ace.animation_id.v1.AnimationIds animation_ids = 1;
inline bool AnimationGraphRequest::_internal_has_animation_ids() const {
  return this != internal_default_instance() && animation_ids_ != nullptr;
}
inline bool AnimationGraphRequest::has_animation_ids() const {
  return _internal_has_animation_ids();
}
inline const ::nvidia::ace::animation_id::v1::AnimationIds& AnimationGraphRequest::_internal_animation_ids() const {
  const ::nvidia::ace::animation_id::v1::AnimationIds* p = animation_ids_;
  return p != nullptr ? *p : reinterpret_cast<const ::nvidia::ace::animation_id::v1::AnimationIds&>(
      ::nvidia::ace::animation_id::v1::_AnimationIds_default_instance_);
}
inline const ::nvidia::ace::animation_id::v1::AnimationIds& AnimationGraphRequest::animation_ids() const {
  // @@protoc_insertion_point(field_get:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_ids)
  return _internal_animation_ids();
}
inline void AnimationGraphRequest::unsafe_arena_set_allocated_animation_ids(
    ::nvidia::ace::animation_id::v1::AnimationIds* animation_ids) {
  if (GetArena() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(animation_ids_);
  }
  animation_ids_ = animation_ids;
  if (animation_ids) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_ids)
}
inline ::nvidia::ace::animation_id::v1::AnimationIds* AnimationGraphRequest::release_animation_ids() {
  
  ::nvidia::ace::animation_id::v1::AnimationIds* temp = animation_ids_;
  animation_ids_ = nullptr;
  if (GetArena() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
  return temp;
}
inline ::nvidia::ace::animation_id::v1::AnimationIds* AnimationGraphRequest::unsafe_arena_release_animation_ids() {
  // @@protoc_insertion_point(field_release:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_ids)
  
  ::nvidia::ace::animation_id::v1::AnimationIds* temp = animation_ids_;
  animation_ids_ = nullptr;
  return temp;
}
inline ::nvidia::ace::animation_id::v1::AnimationIds* AnimationGraphRequest::_internal_mutable_animation_ids() {
  
  if (animation_ids_ == nullptr) {
    auto* p = CreateMaybeMessage<::nvidia::ace::animation_id::v1::AnimationIds>(GetArena());
    animation_ids_ = p;
  }
  return animation_ids_;
}
inline ::nvidia::ace::animation_id::v1::AnimationIds* AnimationGraphRequest::mutable_animation_ids() {
  // @@protoc_insertion_point(field_mutable:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_ids)
  return _internal_mutable_animation_ids();
}
inline void AnimationGraphRequest::set_allocated_animation_ids(::nvidia::ace::animation_id::v1::AnimationIds* animation_ids) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArena();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(animation_ids_);
  }
  if (animation_ids) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
      reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(animation_ids)->GetArena();
    if (message_arena != submessage_arena) {
      animation_ids = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, animation_ids, submessage_arena);
    }
    
  } else {
    
  }
  animation_ids_ = animation_ids;
  // @@protoc_insertion_point(field_set_allocated:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_ids)
}

// string animation_graph_variable_name = 2;
inline void AnimationGraphRequest::clear_animation_graph_variable_name() {
  animation_graph_variable_name_.ClearToEmpty();
}
inline const std::string& AnimationGraphRequest::animation_graph_variable_name() const {
  // @@protoc_insertion_point(field_get:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_name)
  return _internal_animation_graph_variable_name();
}
inline void AnimationGraphRequest::set_animation_graph_variable_name(const std::string& value) {
  _internal_set_animation_graph_variable_name(value);
  // @@protoc_insertion_point(field_set:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_name)
}
inline std::string* AnimationGraphRequest::mutable_animation_graph_variable_name() {
  // @@protoc_insertion_point(field_mutable:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_name)
  return _internal_mutable_animation_graph_variable_name();
}
inline const std::string& AnimationGraphRequest::_internal_animation_graph_variable_name() const {
  return animation_graph_variable_name_.Get();
}
inline void AnimationGraphRequest::_internal_set_animation_graph_variable_name(const std::string& value) {
  
  animation_graph_variable_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void AnimationGraphRequest::set_animation_graph_variable_name(std::string&& value) {
  
  animation_graph_variable_name_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_name)
}
inline void AnimationGraphRequest::set_animation_graph_variable_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  animation_graph_variable_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_name)
}
inline void AnimationGraphRequest::set_animation_graph_variable_name(const char* value,
    size_t size) {
  
  animation_graph_variable_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_name)
}
inline std::string* AnimationGraphRequest::_internal_mutable_animation_graph_variable_name() {
  
  return animation_graph_variable_name_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* AnimationGraphRequest::release_animation_graph_variable_name() {
  // @@protoc_insertion_point(field_release:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_name)
  return animation_graph_variable_name_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void AnimationGraphRequest::set_allocated_animation_graph_variable_name(std::string* animation_graph_variable_name) {
  if (animation_graph_variable_name != nullptr) {
    
  } else {
    
  }
  animation_graph_variable_name_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), animation_graph_variable_name,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_name)
}

// string animation_graph_variable_value = 3;
inline void AnimationGraphRequest::clear_animation_graph_variable_value() {
  animation_graph_variable_value_.ClearToEmpty();
}
inline const std::string& AnimationGraphRequest::animation_graph_variable_value() const {
  // @@protoc_insertion_point(field_get:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_value)
  return _internal_animation_graph_variable_value();
}
inline void AnimationGraphRequest::set_animation_graph_variable_value(const std::string& value) {
  _internal_set_animation_graph_variable_value(value);
  // @@protoc_insertion_point(field_set:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_value)
}
inline std::string* AnimationGraphRequest::mutable_animation_graph_variable_value() {
  // @@protoc_insertion_point(field_mutable:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_value)
  return _internal_mutable_animation_graph_variable_value();
}
inline const std::string& AnimationGraphRequest::_internal_animation_graph_variable_value() const {
  return animation_graph_variable_value_.Get();
}
inline void AnimationGraphRequest::_internal_set_animation_graph_variable_value(const std::string& value) {
  
  animation_graph_variable_value_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void AnimationGraphRequest::set_animation_graph_variable_value(std::string&& value) {
  
  animation_graph_variable_value_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_value)
}
inline void AnimationGraphRequest::set_animation_graph_variable_value(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  animation_graph_variable_value_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_value)
}
inline void AnimationGraphRequest::set_animation_graph_variable_value(const char* value,
    size_t size) {
  
  animation_graph_variable_value_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_value)
}
inline std::string* AnimationGraphRequest::_internal_mutable_animation_graph_variable_value() {
  
  return animation_graph_variable_value_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* AnimationGraphRequest::release_animation_graph_variable_value() {
  // @@protoc_insertion_point(field_release:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_value)
  return animation_graph_variable_value_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void AnimationGraphRequest::set_allocated_animation_graph_variable_value(std::string* animation_graph_variable_value) {
  if (animation_graph_variable_value != nullptr) {
    
  } else {
    
  }
  animation_graph_variable_value_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), animation_graph_variable_value,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:nvidia.ace.services.animation_controller.v1.AnimationGraphRequest.animation_graph_variable_value)
}

// -------------------------------------------------------------------

// AnimationIdsOrStatus

// .nvidia.ace.animation_id.v1.AnimationIds animation_ids = 1;
inline bool AnimationIdsOrStatus::_internal_has_animation_ids() const {
  return response_case() == kAnimationIds;
}
inline bool AnimationIdsOrStatus::has_animation_ids() const {
  return _internal_has_animation_ids();
}
inline void AnimationIdsOrStatus::set_has_animation_ids() {
  _oneof_case_[0] = kAnimationIds;
}
inline ::nvidia::ace::animation_id::v1::AnimationIds* AnimationIdsOrStatus::release_animation_ids() {
  // @@protoc_insertion_point(field_release:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus.animation_ids)
  if (_internal_has_animation_ids()) {
    clear_has_response();
      ::nvidia::ace::animation_id::v1::AnimationIds* temp = response_.animation_ids_;
    if (GetArena() != nullptr) {
      temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
    }
    response_.animation_ids_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline const ::nvidia::ace::animation_id::v1::AnimationIds& AnimationIdsOrStatus::_internal_animation_ids() const {
  return _internal_has_animation_ids()
      ? *response_.animation_ids_
      : reinterpret_cast< ::nvidia::ace::animation_id::v1::AnimationIds&>(::nvidia::ace::animation_id::v1::_AnimationIds_default_instance_);
}
inline const ::nvidia::ace::animation_id::v1::AnimationIds& AnimationIdsOrStatus::animation_ids() const {
  // @@protoc_insertion_point(field_get:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus.animation_ids)
  return _internal_animation_ids();
}
inline ::nvidia::ace::animation_id::v1::AnimationIds* AnimationIdsOrStatus::unsafe_arena_release_animation_ids() {
  // @@protoc_insertion_point(field_unsafe_arena_release:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus.animation_ids)
  if (_internal_has_animation_ids()) {
    clear_has_response();
    ::nvidia::ace::animation_id::v1::AnimationIds* temp = response_.animation_ids_;
    response_.animation_ids_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline void AnimationIdsOrStatus::unsafe_arena_set_allocated_animation_ids(::nvidia::ace::animation_id::v1::AnimationIds* animation_ids) {
  clear_response();
  if (animation_ids) {
    set_has_animation_ids();
    response_.animation_ids_ = animation_ids;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus.animation_ids)
}
inline ::nvidia::ace::animation_id::v1::AnimationIds* AnimationIdsOrStatus::_internal_mutable_animation_ids() {
  if (!_internal_has_animation_ids()) {
    clear_response();
    set_has_animation_ids();
    response_.animation_ids_ = CreateMaybeMessage< ::nvidia::ace::animation_id::v1::AnimationIds >(GetArena());
  }
  return response_.animation_ids_;
}
inline ::nvidia::ace::animation_id::v1::AnimationIds* AnimationIdsOrStatus::mutable_animation_ids() {
  // @@protoc_insertion_point(field_mutable:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus.animation_ids)
  return _internal_mutable_animation_ids();
}

// .nvidia.ace.status.v1.Status status = 2;
inline bool AnimationIdsOrStatus::_internal_has_status() const {
  return response_case() == kStatus;
}
inline bool AnimationIdsOrStatus::has_status() const {
  return _internal_has_status();
}
inline void AnimationIdsOrStatus::set_has_status() {
  _oneof_case_[0] = kStatus;
}
inline ::nvidia::ace::status::v1::Status* AnimationIdsOrStatus::release_status() {
  // @@protoc_insertion_point(field_release:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus.status)
  if (_internal_has_status()) {
    clear_has_response();
      ::nvidia::ace::status::v1::Status* temp = response_.status_;
    if (GetArena() != nullptr) {
      temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
    }
    response_.status_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline const ::nvidia::ace::status::v1::Status& AnimationIdsOrStatus::_internal_status() const {
  return _internal_has_status()
      ? *response_.status_
      : reinterpret_cast< ::nvidia::ace::status::v1::Status&>(::nvidia::ace::status::v1::_Status_default_instance_);
}
inline const ::nvidia::ace::status::v1::Status& AnimationIdsOrStatus::status() const {
  // @@protoc_insertion_point(field_get:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus.status)
  return _internal_status();
}
inline ::nvidia::ace::status::v1::Status* AnimationIdsOrStatus::unsafe_arena_release_status() {
  // @@protoc_insertion_point(field_unsafe_arena_release:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus.status)
  if (_internal_has_status()) {
    clear_has_response();
    ::nvidia::ace::status::v1::Status* temp = response_.status_;
    response_.status_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline void AnimationIdsOrStatus::unsafe_arena_set_allocated_status(::nvidia::ace::status::v1::Status* status) {
  clear_response();
  if (status) {
    set_has_status();
    response_.status_ = status;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus.status)
}
inline ::nvidia::ace::status::v1::Status* AnimationIdsOrStatus::_internal_mutable_status() {
  if (!_internal_has_status()) {
    clear_response();
    set_has_status();
    response_.status_ = CreateMaybeMessage< ::nvidia::ace::status::v1::Status >(GetArena());
  }
  return response_.status_;
}
inline ::nvidia::ace::status::v1::Status* AnimationIdsOrStatus::mutable_status() {
  // @@protoc_insertion_point(field_mutable:nvidia.ace.services.animation_controller.v1.AnimationIdsOrStatus.status)
  return _internal_mutable_status();
}

inline bool AnimationIdsOrStatus::has_response() const {
  return response_case() != RESPONSE_NOT_SET;
}
inline void AnimationIdsOrStatus::clear_has_response() {
  _oneof_case_[0] = RESPONSE_NOT_SET;
}
inline AnimationIdsOrStatus::ResponseCase AnimationIdsOrStatus::response_case() const {
  return AnimationIdsOrStatus::ResponseCase(_oneof_case_[0]);
}
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace animation_controller
}  // namespace services
}  // namespace ace
}  // namespace nvidia

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_nvidia_2fanimation_2dcontroller_2dinterface_2dproto_2fnvidia_2eace_2eservices_2eanimation_5fcontroller_2ev1_2eproto