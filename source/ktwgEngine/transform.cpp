#include "transform.h"

Transform::Transform()
  : pos_{}, rot_{}, scale_{ 1.f, 1.f, 1.f }
{
}

Matrix3 Transform::GetRotationMatrix33() const
{
  Matrix3 result = result = ConvertQuaternionToMat33(rot_);
  result.m_[0][0] *= scale_.x_;
  result.m_[0][1] *= scale_.x_;
  result.m_[0][2] *= scale_.x_;
  result.m_[1][0] *= scale_.y_;
  result.m_[1][1] *= scale_.y_;
  result.m_[1][2] *= scale_.y_;
  result.m_[2][0] *= scale_.z_;
  result.m_[2][1] *= scale_.z_;
  result.m_[2][2] *= scale_.z_;

  return result;
}

void Transform::LookAt(const Vec3& target, const Vec3& worldUp)
{
  SetRotation(LookRotation(target - pos_, worldUp));
}

Matrix4 Transform::GetMatrix() const
{
  Matrix4 result = ConvertQuaternionToMat44(rot_);
  result.m_[0][0] *= scale_.x_;
  result.m_[0][1] *= scale_.x_;
  result.m_[0][2] *= scale_.x_;
  result.m_[1][0] *= scale_.y_;
  result.m_[1][1] *= scale_.y_;
  result.m_[1][2] *= scale_.y_;
  result.m_[2][0] *= scale_.z_;
  result.m_[2][1] *= scale_.z_;
  result.m_[2][2] *= scale_.z_;
  result.m_[3][0] = pos_.x_;
  result.m_[3][1] = pos_.y_;
  result.m_[3][2] = pos_.z_;
  result.m_[3][3] = 1.f;
  return result;
}

Vec3 Transform::Forward() const
{
  Vec3 v = VECTOR_Z;
  rot_.Rotate(v);
  return v;
}
