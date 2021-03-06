// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: dgframe.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "dgframe.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace dgframe {

namespace {

const ::google::protobuf::Descriptor* Frame_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Frame_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* Code_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_dgframe_2eproto() {
  protobuf_AddDesc_dgframe_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "dgframe.proto");
  GOOGLE_CHECK(file != NULL);
  Frame_descriptor_ = file->message_type(0);
  static const int Frame_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Frame, width_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Frame, height_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Frame, length_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Frame, code_),
  };
  Frame_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Frame_descriptor_,
      Frame::default_instance_,
      Frame_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Frame, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Frame, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Frame));
  Code_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_dgframe_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Frame_descriptor_, &Frame::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_dgframe_2eproto() {
  delete Frame::default_instance_;
  delete Frame_reflection_;
}

void protobuf_AddDesc_dgframe_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\rdgframe.proto\022\007dgframe\"D\n\005Frame\022\r\n\005wid"
    "th\030\001 \001(\005\022\016\n\006height\030\002 \001(\005\022\016\n\006length\030\003 \001(\005"
    "\022\014\n\004code\030\004 \001(\005*\270\002\n\004Code\022\013\n\007UNKNOWN\020\000\022\016\n\n"
    "YUV444_UYV\020\001\022\017\n\013YUV422_UYVY\020\002\022\021\n\rYUV411_"
    "UYYVYY\020\003\022\013\n\007RGB_RGB\020\004\022\r\n\tRGBA_RGBA\020\005\022\013\n\007"
    "RGB_BGR\020\006\022\r\n\tRGBA_BGRA\020\007\022\013\n\007MONO8_Y\020\010\022\023\n"
    "\017MONO16Y_Y_HI_LO\020\t\022\023\n\017MONO12Y_Y_HI_LO\020\n\022"
    "\007\n\003INT\020\013\022\020\n\014UNSIGNED_INT\020\014\022\t\n\005FLOAT\020\r\022\n\n"
    "\006DOUBLE\020\016\022\021\n\rUNSIGNED_CHAR\020\017\022\022\n\016UNSIGNED"
    "_SHORT\020\020\022\025\n\021UNSIGNED_SHORT_12\020\021\022\020\n\014SIGNE"
    "D_SHORT\020\022", 409);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "dgframe.proto", &protobuf_RegisterTypes);
  Frame::default_instance_ = new Frame();
  Frame::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_dgframe_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_dgframe_2eproto {
  StaticDescriptorInitializer_dgframe_2eproto() {
    protobuf_AddDesc_dgframe_2eproto();
  }
} static_descriptor_initializer_dgframe_2eproto_;
const ::google::protobuf::EnumDescriptor* Code_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Code_descriptor_;
}
bool Code_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
      return true;
    default:
      return false;
  }
}


// ===================================================================

#ifndef _MSC_VER
const int Frame::kWidthFieldNumber;
const int Frame::kHeightFieldNumber;
const int Frame::kLengthFieldNumber;
const int Frame::kCodeFieldNumber;
#endif  // !_MSC_VER

Frame::Frame()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:dgframe.Frame)
}

void Frame::InitAsDefaultInstance() {
}

Frame::Frame(const Frame& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:dgframe.Frame)
}

void Frame::SharedCtor() {
  _cached_size_ = 0;
  width_ = 0;
  height_ = 0;
  length_ = 0;
  code_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Frame::~Frame() {
  // @@protoc_insertion_point(destructor:dgframe.Frame)
  SharedDtor();
}

void Frame::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Frame::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Frame::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Frame_descriptor_;
}

const Frame& Frame::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_dgframe_2eproto();
  return *default_instance_;
}

Frame* Frame::default_instance_ = NULL;

Frame* Frame::New() const {
  return new Frame;
}

void Frame::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<Frame*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  ZR_(width_, code_);

#undef OFFSET_OF_FIELD_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Frame::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:dgframe.Frame)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 width = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &width_)));
          set_has_width();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_height;
        break;
      }

      // optional int32 height = 2;
      case 2: {
        if (tag == 16) {
         parse_height:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &height_)));
          set_has_height();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(24)) goto parse_length;
        break;
      }

      // optional int32 length = 3;
      case 3: {
        if (tag == 24) {
         parse_length:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &length_)));
          set_has_length();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(32)) goto parse_code;
        break;
      }

      // optional int32 code = 4;
      case 4: {
        if (tag == 32) {
         parse_code:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &code_)));
          set_has_code();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:dgframe.Frame)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:dgframe.Frame)
  return false;
#undef DO_
}

void Frame::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:dgframe.Frame)
  // optional int32 width = 1;
  if (has_width()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->width(), output);
  }

  // optional int32 height = 2;
  if (has_height()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->height(), output);
  }

  // optional int32 length = 3;
  if (has_length()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->length(), output);
  }

  // optional int32 code = 4;
  if (has_code()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(4, this->code(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:dgframe.Frame)
}

::google::protobuf::uint8* Frame::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:dgframe.Frame)
  // optional int32 width = 1;
  if (has_width()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->width(), target);
  }

  // optional int32 height = 2;
  if (has_height()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->height(), target);
  }

  // optional int32 length = 3;
  if (has_length()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->length(), target);
  }

  // optional int32 code = 4;
  if (has_code()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(4, this->code(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:dgframe.Frame)
  return target;
}

int Frame::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 width = 1;
    if (has_width()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->width());
    }

    // optional int32 height = 2;
    if (has_height()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->height());
    }

    // optional int32 length = 3;
    if (has_length()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->length());
    }

    // optional int32 code = 4;
    if (has_code()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->code());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Frame::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Frame* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Frame*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Frame::MergeFrom(const Frame& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_width()) {
      set_width(from.width());
    }
    if (from.has_height()) {
      set_height(from.height());
    }
    if (from.has_length()) {
      set_length(from.length());
    }
    if (from.has_code()) {
      set_code(from.code());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Frame::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Frame::CopyFrom(const Frame& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Frame::IsInitialized() const {

  return true;
}

void Frame::Swap(Frame* other) {
  if (other != this) {
    std::swap(width_, other->width_);
    std::swap(height_, other->height_);
    std::swap(length_, other->length_);
    std::swap(code_, other->code_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Frame::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Frame_descriptor_;
  metadata.reflection = Frame_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace dgframe

// @@protoc_insertion_point(global_scope)
