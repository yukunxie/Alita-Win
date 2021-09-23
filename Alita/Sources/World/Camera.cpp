//
// Created by realxie on 2019-10-29.
//

#include "Camera.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <chrono>
#include <math.h>

NS_RX_BEGIN

#define PI 3.1415926f
#define ANGLE_TO_RAD(angle) ( (angle) * PI / 180.0f)

static TMat4x4 _ComposeViewMatrix(const TVector3& position, const TVector3& rotation)
{
	auto viewMatrix = TMat4x4(1.0f);
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.x), TVector3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), TVector3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), TVector3(0.0f, 0.0f, 1.0f));

	//translate to init position
	viewMatrix = glm::transpose(viewMatrix) * glm::translate(TMat4x4(1.0f), position * -1.f);
	return viewMatrix;
}

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::_UpdateViewMatrix()
{
	viewMatrix_ = _ComposeViewMatrix(transform_.Position(), transform_.Rotation());
}

void Camera::LookAt(const TVector3& from, const TVector3& center, const TVector3& up)
{
	auto rMaxtrix = glm::lookAtRH(from, center, up);

	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;

	bool ret = glm::decompose(rMaxtrix, scale, rotation, translation, skew, perspective);
	Assert(ret, "invalid");

	rotation = glm::conjugate(rotation);
	auto degrees = glm::degrees(glm::eulerAngles(rotation));

	SetPosition(from);
	SetRotation(degrees);

	_UpdateViewMatrix();
}

const float CAMERA_MOVE_SPEED = 15.0f;

void Camera::MoveForward(float speedScale)
{
	_UpdateViewMatrix();

	auto posInViewSpace = glm::vec4(0, 0, -CAMERA_MOVE_SPEED * 0.01f * speedScale, 1);

	transform_.Position() = glm::inverse(viewMatrix_) * posInViewSpace;

	_UpdateViewMatrix();
}

void Camera::MoveUp(float speedScale)
{
	_UpdateViewMatrix();

	auto posInViewSpace = glm::vec4(0, CAMERA_MOVE_SPEED * 0.01f * speedScale, 0, 1);

	transform_.Position() = glm::inverse(viewMatrix_) * posInViewSpace;

	_UpdateViewMatrix();
}

void Camera::MoveRight(float speedScale)
{
	_UpdateViewMatrix();

	auto posInViewSpace = glm::vec4(CAMERA_MOVE_SPEED * 0.01f * speedScale, 0, 0, 1);

	transform_.Position() = glm::inverse(viewMatrix_) * posInViewSpace;

	_UpdateViewMatrix();
}

OrthoCamera::OrthoCamera(float left, float right, float bottom, float top, float zNear, float zFar)
{
	projMatrix_ = glm::orthoRH(left, right, bottom, top, zNear, zFar);

	_UpdateViewMatrix();
}

void OrthoCamera::Tick(float dt)
{
	_UpdateViewMatrix();
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
	projMatrix_ = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
	projMatrix_[1][1] *= -1;
}

void PerspectiveCamera::Tick(float dt)
{
	_UpdateViewMatrix();
}

NS_RX_END