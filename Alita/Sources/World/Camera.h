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
		return viewMatrix_ * projMatrix_;
	}

	void MoveForward();

	void MoveBack();

	void MoveRight();

	void MoveLeft();

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
