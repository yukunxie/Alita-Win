//
// Created by realxie on 2019-10-29.
//

#pragma once


#include "Base/ObjectBase.h"
#include "Base/Entity.h"

NS_RX_BEGIN

// Right hand coordiate system.

class Camera : public Entity
{
public:
	Camera();
	virtual ~Camera();

public:
	virtual TMat4x4 GetViewMatrix() = 0;
	virtual TMat4x4 GetProjectMatrix() = 0;
	virtual TMat4x4 GetMVP() = 0;

public:
	TColor4 GetBackgroundColor() const
	{
		return backgroudColor_;
	}

	void SetBackgroundColor(const TColor4& color4)
	{
		backgroudColor_ = color4;
	}

public:
	static Camera* CreatePerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane);

protected:
	TColor4 backgroudColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };
};

class PerspectiveCamera final: public Camera
{
public:
	PerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane);

	virtual TMat4x4 GetViewMatrix() override;
	virtual TMat4x4 GetProjectMatrix() override;
	virtual TMat4x4 GetMVP() override;

private:
	float fov_;
	float aspect_;
	float nearPlane_;
	float farPlane_;
	/*TVector3 up_;
	TVector3 forward_;*/
};

NS_RX_END
