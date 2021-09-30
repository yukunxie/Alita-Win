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
protected:
	Camera();

	void _UpdateViewMatrix();

public:
	virtual ~Camera();

public:
	TMat4x4 GetViewMatrix()
	{
		return viewMatrix_;
	}

	TMat4x4 GetProjectionMatrix()
	{
		return projMatrix_;
	}
	
	TMat4x4 GetViewProjectionMatrix()
	{
		return  projMatrix_ * viewMatrix_;
	}

	void Setup(const TVector3& position, float yaw, float pitch, const TVector3& up = { 0, 1, 0 });

	void LookAt(const TVector3& from, const TVector3& center, const TVector3& up = { 0, 1, 0 });

	void MoveForward(float speedScale = 1.0f);

	void MoveUp(float speedScale = 1.0f);

	void MoveRight(float speedScale = 1.0f);

	void RotateAroundPoint(float x, float y);
	
	void YawPitch(float raw = 0.0f, float pitch = 0.0);

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
	TMat4x4 viewMatrix_;
	TMat4x4 projMatrix_;
	TColor4 backgroudColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };

	float Yaw_ = 0.0f;
	float Pitch_ = 0.0f;
	TVector3 WorldUp_ = { 0, 1, 0 };
	TVector3 Up_;
	TVector3 Front_;
};

class OrthoCamera final : public Camera
{
public:
	OrthoCamera(float left, float right, float bottom, float top, float zNear, float zFar);

	virtual void Tick(float dt) override;
};

class PerspectiveCamera final: public Camera
{
public:
	PerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane);

public:
	virtual void Tick(float dt) override;

private:
	float fov_;
	float aspect_;
	float nearPlane_;
	float farPlane_;
};

NS_RX_END
