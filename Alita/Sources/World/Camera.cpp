//
// Created by realxie on 2019-10-29.
//

#include "Camera.h"

NS_RX_BEGIN

Camera::Camera()
{

}

Camera::~Camera()
{

}

Camera* Camera::CreatePerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane)
{
	return new PerspectiveCamera(fov, aspect, nearPlane, farPlane);
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane)
	: fov_(fov)
	, aspect_(aspect)
	, nearPlane_(nearPlane)
	, farPlane_(farPlane)
{
}

TMat4x4 PerspectiveCamera::GetViewMatrix()
{
	TMat4x4 mat4(1.0f);
	return glm::translate(mat4, transform_.Position() * -1.0f);
}

TMat4x4 PerspectiveCamera::GetProjectMatrix()
{
	return glm::perspectiveRH(glm::radians(fov_), aspect_, nearPlane_, farPlane_);
}

TMat4x4 PerspectiveCamera::GetMVP()
{
	return GetProjectMatrix() * GetViewMatrix();
}

NS_RX_END