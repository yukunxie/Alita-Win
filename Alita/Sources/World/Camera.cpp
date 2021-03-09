//
// Created by realxie on 2019-10-29.
//

#include "Camera.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <chrono>

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
	viewMatrix = glm::translate(viewMatrix, position * -1.f);
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
	TMat4x4 mat = glm::lookAtRH(from, center, up);
	auto k = mat * TVector4{ 0, 0, 0, 1 };
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	bool ret = glm::decompose(mat, scale, rotation, translation, skew, perspective);

	rotation = glm::conjugate(rotation);
	auto e = glm::eulerAngles(rotation);
	auto angles = glm::degrees(e);

	if (true);
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
	transform_.Position() = TVector3{ 0.0f, 15.0f, 15.f };
	transform_.Rotation() = TVector3{ 45.0f, 0.0f, 0.0f };

	/*viewMatrix_ = glm::lookAtRH(glm::vec3(.0f, 0.0f, -8.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));*/

	projMatrix_ = glm::perspectiveRH(glm::radians(45.0f), 1280.0f / 800.0f, 0.1f, 100.f);
	projMatrix_[1][1] *= -1;

	
	_UpdateViewMatrix();
}

void PerspectiveCamera::Tick(float dt)
{
	_UpdateViewMatrix();
	//return;

	//// rotate camera

	//static auto startTime = std::chrono::high_resolution_clock::now();
	//auto currentTime = std::chrono::high_resolution_clock::now();
	//float time = std::chrono::duration<float, std::chrono::seconds::period>(
	//	currentTime - startTime).count();

	//_UpdateViewMatrix();

	//TMat4x4 mat(1);
	//mat = glm::rotate(mat, time * glm::radians(90.0f) * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
	//
	//viewMatrix_ = viewMatrix_ * mat;
}

NS_RX_END