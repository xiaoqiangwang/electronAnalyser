// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: dgframe.proto

#ifndef PROTOBUF_dgframe_2eproto__INCLUDED
#define PROTOBUF_dgframe_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace dgframe {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_dgframe_2eproto();
void protobuf_AssignDesc_dgframe_2eproto();
void protobuf_ShutdownFile_dgframe_2eproto();

class Frame;

enum Code {
  UNKNOWN = 0,
  YUV444_UYV = 1,
  YUV422_UYVY = 2,
  YUV411_UYYVYY = 3,
  RGB_RGB = 4,
  RGBA_RGBA = 5,
  RGB_BGR = 6,
  RGBA_BGRA = 7,
  MONO8_Y = 8,
  MONO16Y_Y_HI_LO = 9,
  MONO12Y_Y_HI_LO = 10,
  INT = 11,
  UNSIGNED_INT = 12,
  FLOAT = 13,
  DOUBLE = 14,
  UNSIGNED_CHAR = 15,
  UNSIGNED_SHORT = 16,
  UNSIGNED_SHORT_12 = 17,
  SIGNED_SHORT = 18
};
bool Code_IsValid(int value);
const Code Code_MIN = UNKNOWN;
const Code Code_MAX = SIGNED_SHORT;
const int Code_ARRAYSIZE = Code_MAX + 1;

const ::google::protobuf::EnumDescriptor* Code_descriptor();
inline const ::std::string& Code_Name(Code value) {
  return ::google::protobuf::internal::NameOfEnum(
    Code_descriptor(), value);
}
inline bool Code_Parse(
    const ::std::string& name, Code* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Code>(
    Code_descriptor(), name, value);
}
// ===================================================================

class Frame : public ::google::protobuf::Message {
 public:
  Frame();
  virtual ~Frame();

  Frame(const Frame& from);

  inline Frame& operator=(const Frame& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Frame& default_instance();

  void Swap(Frame* other);

  // implements Message ----------------------------------------------

  Frame* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Frame& from);
  void MergeFrom(const Frame& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 width = 1;
  inline bool has_width() const;
  inline void clear_width();
  static const int kWidthFieldNumber = 1;
  inline ::google::protobuf::int32 width() const;
  inline void set_width(::google::protobuf::int32 value);

  // optional int32 height = 2;
  inline bool has_height() const;
  inline void clear_height();
  static const int kHeightFieldNumber = 2;
  inline ::google::protobuf::int32 height() const;
  inline void set_height(::google::protobuf::int32 value);

  // optional int32 length = 3;
  inline bool has_length() const;
  inline void clear_length();
  static const int kLengthFieldNumber = 3;
  inline ::google::protobuf::int32 length() const;
  inline void set_length(::google::protobuf::int32 value);

  // optional int32 code = 4;
  inline bool has_code() const;
  inline void clear_code();
  static const int kCodeFieldNumber = 4;
  inline ::google::protobuf::int32 code() const;
  inline void set_code(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:dgframe.Frame)
 private:
  inline void set_has_width();
  inline void clear_has_width();
  inline void set_has_height();
  inline void clear_has_height();
  inline void set_has_length();
  inline void clear_has_length();
  inline void set_has_code();
  inline void clear_has_code();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::int32 width_;
  ::google::protobuf::int32 height_;
  ::google::protobuf::int32 length_;
  ::google::protobuf::int32 code_;
  friend void  protobuf_AddDesc_dgframe_2eproto();
  friend void protobuf_AssignDesc_dgframe_2eproto();
  friend void protobuf_ShutdownFile_dgframe_2eproto();

  void InitAsDefaultInstance();
  static Frame* default_instance_;
};
// ===================================================================


// ===================================================================

// Frame

// optional int32 width = 1;
inline bool Frame::has_width() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Frame::set_has_width() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Frame::clear_has_width() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Frame::clear_width() {
  width_ = 0;
  clear_has_width();
}
inline ::google::protobuf::int32 Frame::width() const {
  // @@protoc_insertion_point(field_get:dgframe.Frame.width)
  return width_;
}
inline void Frame::set_width(::google::protobuf::int32 value) {
  set_has_width();
  width_ = value;
  // @@protoc_insertion_point(field_set:dgframe.Frame.width)
}

// optional int32 height = 2;
inline bool Frame::has_height() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Frame::set_has_height() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Frame::clear_has_height() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Frame::clear_height() {
  height_ = 0;
  clear_has_height();
}
inline ::google::protobuf::int32 Frame::height() const {
  // @@protoc_insertion_point(field_get:dgframe.Frame.height)
  return height_;
}
inline void Frame::set_height(::google::protobuf::int32 value) {
  set_has_height();
  height_ = value;
  // @@protoc_insertion_point(field_set:dgframe.Frame.height)
}

// optional int32 length = 3;
inline bool Frame::has_length() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Frame::set_has_length() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Frame::clear_has_length() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Frame::clear_length() {
  length_ = 0;
  clear_has_length();
}
inline ::google::protobuf::int32 Frame::length() const {
  // @@protoc_insertion_point(field_get:dgframe.Frame.length)
  return length_;
}
inline void Frame::set_length(::google::protobuf::int32 value) {
  set_has_length();
  length_ = value;
  // @@protoc_insertion_point(field_set:dgframe.Frame.length)
}

// optional int32 code = 4;
inline bool Frame::has_code() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Frame::set_has_code() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Frame::clear_has_code() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Frame::clear_code() {
  code_ = 0;
  clear_has_code();
}
inline ::google::protobuf::int32 Frame::code() const {
  // @@protoc_insertion_point(field_get:dgframe.Frame.code)
  return code_;
}
inline void Frame::set_code(::google::protobuf::int32 value) {
  set_has_code();
  code_ = value;
  // @@protoc_insertion_point(field_set:dgframe.Frame.code)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace dgframe

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::dgframe::Code> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::dgframe::Code>() {
  return ::dgframe::Code_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_dgframe_2eproto__INCLUDED
