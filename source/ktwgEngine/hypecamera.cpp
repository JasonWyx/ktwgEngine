#include "hypecamera.h"

void Camera::UpdateFrustum()
{
  float yScale = 1.f / tanf(verticalFov_ / 2.0f);
  float xScale = yScale / aspectRatio_;

  cachedProjectionTransform_.m_[0][0] = xScale;
  cachedProjectionTransform_.m_[1][1] = yScale;
  cachedProjectionTransform_.m_[2][2] = farDistance_ / (farDistance_ - nearDistance_);
  cachedProjectionTransform_.m_[2][3] = 1;
  cachedProjectionTransform_.m_[3][2] = -(nearDistance_ * farDistance_ / (farDistance_ - nearDistance_));

  shouldUpdateProjection_ = false;
}

Camera::Camera(Entity& owner)
  : owner_{&owner}, cachedProjectionTransform_{}, verticalFov_{ PI / 2 }, aspectRatio_{ 16.f / 9.f }, nearDistance_{ 3.f }, farDistance_{ 500.f }, shouldUpdateProjection_{ true }
{
}

Camera::Camera(Entity& owner, float verticalFov, float aspectRatio, float nearDistance, float farDistance)
  : owner_{&owner}, cachedProjectionTransform_{}, verticalFov_{ verticalFov }, aspectRatio_{ aspectRatio }, nearDistance_{ nearDistance }, farDistance_{ farDistance }, shouldUpdateProjection_{ true }
{
}

void Camera::Update()
{
  if (shouldUpdateProjection_)
    UpdateFrustum();
}

void Camera::SetFarDistance(float farDist)
{
  farDistance_ = farDist;
  shouldUpdateProjection_ = true;
}

void Camera::SetNearDistance(float NearDist)
{
  nearDistance_ = NearDist;
  shouldUpdateProjection_ = true;
}

void Camera::SetVerticalFov(float verticalFOV)
{
  verticalFov_ = verticalFOV;
  shouldUpdateProjection_ = true;
}

void Camera::SetAspectRatio(float aspectRatio)
{
  aspectRatio_ = aspectRatio;
  shouldUpdateProjection_ = true;

}

const Vec3 & Camera::GetPosition() const
{
  return owner_->GetTransform().GetPosition();
}

const Vec3 & Camera::GetLookAt() const
{
  static Vec3 lookAt = Vec3{};
  // Reset look at to be the standard forward vector
  lookAt = Vec3{ 0.f, 0.f, 1.f };
  owner_->GetTransform().GetRotation().Rotate(lookAt);
  return lookAt;
}

const Quaternion & Camera::GetRotation() const
{
  return owner_->GetTransform().GetRotation();
}

Matrix4 Camera::GetViewMatrix() const
{
  auto z = GetLookAt();
  auto x = Cross(Vec3{ 0.f, 1.f, 0.f }, z);
  x.Normalize();
  auto y = Cross(z, x);
  auto pos = GetPosition();

  return
    Matrix4{ x.x_, y.x_, z.x_, 0.f,
            x.y_, y.y_, z.y_, 0.f,
            x.z_, y.z_, z.z_, 0.f,
           -Dot(x, pos), -Dot(y, pos), -Dot(z, pos), 1.f };
}
