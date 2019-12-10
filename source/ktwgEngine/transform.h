#pragma once

#include "vector3.h"
#include "quaternion.h"
#include "bitstream.h"
#include "componentids.h"

class Transform
{
public:

  Transform();

  /* Reference getters */
  Vec3&       GetPosition() { return pos_; }
  Vec3&       GetScale()    { return scale_; }
  Quaternion& GetRotation() { return rot_; }

  /* Getters */
  const Vec3&       GetPosition() const { return pos_; }
  const Vec3&       GetScale()    const { return scale_; }
  const Quaternion& GetRotation() const { return rot_; }

  /* Called to display inspector only */
  const Vec3& GetRotationAxes() const { return ToEuler(rot_); }

  Matrix3 GetRotationMatrix33() const;

  bool operator==(const Transform& rhs) const { return pos_ == rhs.pos_ && rot_ == rhs.rot_ && scale_ == rhs.scale_; }
  bool operator!=(const Transform& rhs) const { return !(*this == rhs); }

  friend Vec3      Multiply(const Transform& t, const Vec3& v);
  friend Vec3      MultiplyNS(const Transform& t, const Vec3& v);
  friend Transform Multiply(const Transform& t0, const Transform& t1);
  friend Transform MultiplyNS(const Transform& t0, const Transform& t1);
  
  // ---- WORLD POSITION ----------------------------
  void SetPosition(const Vec3& pos)
  {
    pos_ = pos;
  }

  void SetRotationAxis(const Vec3& axis)
  {
    rot_ = ConvertEulerToQuaternion(axis);
    GetMatrix();
  }

  void SetRotation(const Quaternion& quat)
  {
    rot_ = quat;
  }

  void SetScale(const Vec3& scale)
  {
    scale_.x_ = scale.x_;
    scale_.y_ = scale.y_;
    scale_.z_ = scale.z_;
  }
  // ---- END WORLD POSITION ----------------------------

  void SetIdentity()
  {
    pos_.SetZero();
    rot_.SetIdentity();
    scale_.Set(1, 1, 1);
  }

  void LookAt(const Vec3& target, const Vec3& worldUp = VECTOR_Y);

  Matrix4 GetMatrix() const;

  Vec3 Forward() const;

protected:
  Vec3       pos_;   // Stores the position of the transform
  Quaternion rot_;   // Stores the rotation of the transform in quaternion
  Vec3       scale_; // Stores the scale of the transform
};

inline Vec3 Multiply(const Transform& t, const Vec3& v)
{
  // Apply transform on a vector
  // 1st : Apply scale
  // 2nd : Apply rotation
  // 3rd : Apply translation
  auto a = t.scale_ * v;
  t.rot_.Rotate(a);
  return a + t.pos_;
}

inline Vec3 MultiplyNS(const Transform& t, const Vec3& v)
{
  // Apply transform on a vector
  // 2nd : Apply rotation
  // 3rd : Apply translation
  auto a = v;
  t.rot_.Rotate(a);
  return a + t.pos_;
}

inline Transform Multiply(const Transform& lhs, const Transform& rhs)
{
  // Concat two transform
  Transform result;

  Vec3 v = rhs.pos_;
  result.rot_ = lhs.rot_ * rhs.rot_;
  lhs.rot_.Rotate(v);
  result.pos_ = v + lhs.pos_;
  result.scale_ = lhs.scale_ * rhs.scale_;
  return result;
}

inline Transform MultiplyNS(const Transform& lhs, const Transform& rhs)
{
  // Concat two transform ignoring scaling
  Transform result;
  Vec3 v = rhs.pos_;
  result.rot_ = lhs.rot_ * rhs.rot_;
  lhs.rot_.Rotate(v);
  result.pos_ = v + lhs.pos_;
  return result;
}

inline Vec3 MultiplyT(const Transform& lhs, const Vec3& rhs)
{
  Matrix3 inverse = Inverse(lhs.GetRotationMatrix33());
  return Multiply(inverse, rhs - lhs.GetPosition());
}

inline BitStream& operator<<(BitStream& stream, const Transform& tfm)
{
  stream << CI_Transform;
  const Vec3& pos = tfm.GetPosition();
  stream << pos.x_ << pos.y_ << pos.z_;
  const Quaternion& rot = tfm.GetRotation();
  stream << rot.x_ << rot.y_ << rot.z_ << rot.w_;
  const Vec3& scale = tfm.GetScale();
  stream << scale.x_ << scale.y_ << scale.z_;

  return stream;
}

inline BitStream& operator>>(BitStream& stream, Transform& tfm)
{
  ComponentID dummy;
  stream >> dummy;
  Vec3 pos;
  stream >> pos.x_ >> pos.y_ >> pos.z_;
  Quaternion rot;
  stream >> rot.x_ >> rot.y_ >> rot.z_ >> rot.w_;
  Vec3 scale;
  stream >> scale.x_ >> scale.y_ >> scale.z_;

  tfm.SetPosition(pos);
  tfm.SetRotation(rot);
  tfm.SetScale(scale);

  return stream;
}

inline BitStream& ReplicateFromStream(BitStream& stream, Transform& tfm)
{
  Vec3 pos;
  stream >> pos.x_ >> pos.y_ >> pos.z_;
  Quaternion rot;
  stream >> rot.x_ >> rot.y_ >> rot.z_ >> rot.w_;
  Vec3 scale;
  stream >> scale.x_ >> scale.y_ >> scale.z_;

  tfm.SetPosition(pos);
  tfm.SetRotation(rot);
  tfm.SetScale(scale);

  return stream;

}