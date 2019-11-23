#pragma once

#include "entity.h"
#include "matrix4.h"

///////////////////////////////////////////////////////////
// Class that represents a Camera                        //
///////////////////////////////////////////////////////////
class Camera
{
public:

  /////////////////////////////////
  // Constructors                //
  /////////////////////////////////
  Camera(Entity& owner);
  Camera(Entity& owner, float verticalFov, float aspectRatio, float nearDistance, float farDistance);

  //////////////////////////////////
  // Frustum Functions            //
  //////////////////////////////////
  void                SetFarDistance(float farDist);
  void                SetNearDistance(float NearDist);
  void                SetVerticalFov(float verticalFOV);
  void                SetAspectRatio(float aspectRatio);
  void                SetActive(bool active) { isActive_ = active; }

  //////////////////////////////////
  // Get Functions                //
  //////////////////////////////////
  virtual       float               GetVerticalFov()      const { return verticalFov_; }
  virtual       float               GetAspectRatio()      const { return aspectRatio_; }
  virtual       float               GetNearDistance()      const { return nearDistance_; }
  virtual       float               GetFarDistance()      const { return farDistance_; }
  virtual const Vec3&               GetPosition()      const;
  virtual const Vec3&               GetLookAt()      const;
  virtual const Quaternion&         GetRotation()      const;
  virtual       Matrix4             GetViewMatrix()      const;
  virtual const Matrix4&            GetProjectionMatrix()      const { return cachedProjectionTransform_; }
  bool          GetActive()      const { return isActive_; }

  void          Update();

protected:
  void          UpdateFrustum();

  Entity*       owner_;
  Matrix4       cachedProjectionTransform_; // View transform is computed every loop
  float         verticalFov_;               // Vertical FOV used
  float         aspectRatio_;               // Aspect ratio width : height
  float         nearDistance_;              // Distance to near plane
  float         farDistance_;               // Distance to far plane
  // RenderTarget* renderTarget_;              // Where to render the result of this camera
  bool          shouldUpdateProjection_;    // Does the projection matrix need to be updated
  bool          isActive_;
};