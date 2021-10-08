//
// Created by realxie on 2019-10-29.
//

#include "Camera.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/vector_angle.hpp>
#include < glm/gtx/euler_angles.hpp>
#include <chrono>
#include <math.h>

NS_RX_BEGIN

#define PI 3.1415926f
#define ANGLE_TO_RAD(angle) ( (angle) * PI / 180.0f)

// https://www.3dgep.com/understanding-the-view-matrix/#The_View_Matrix
static TMat4x4 _ComposeViewMatrix(const TVector3& eye, const TVector3& rotation)
{
	float yaw = glm::radians(rotation.y);
	float pitch = glm::radians(rotation.x);
	
	// I assume the values are already converted to radians.
	float cosPitch = cos(pitch);
	float sinPitch = sin(pitch);
	float cosYaw = cos(yaw);
	float sinYaw = sin(yaw);

	TVector3 xaxis = { cosYaw, 0, -sinYaw };
	TVector3 yaxis = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
	TVector3 zaxis = { sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
	glm::mat4 viewMatrix = {
		TVector4(xaxis.x,            yaxis.x,            zaxis.x,      0),
		TVector4(xaxis.y,            yaxis.y,            zaxis.y,      0),
		TVector4(xaxis.z,            yaxis.z,            zaxis.z,      0),
		TVector4(-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1)
	};

	auto a = viewMatrix * TVector4(1, 1, 0, 1);

	return viewMatrix;

	////auto viewMatrix = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	//auto viewMatrix = TMat4x4(1.0f);
	//viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.x), TVector3(1.0f, 0.0f, 0.0f));
	//viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), TVector3(0.0f, 1.0f, 0.0f));
	//viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), TVector3(0.0f, 0.0f, 1.0f));

	////translate to init position
	//viewMatrix = glm::transpose(viewMatrix) * glm::translate(TMat4x4(1.0f), position);

	//auto a = viewMatrix * TVector4(1, 1, 0, 1);
	//return viewMatrix;
}

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::_UpdateViewMatrix()
{
	if (isTransformDirty_)
	{
		viewMatrix_ = _ComposeViewMatrix(transform_.Position(), transform_.Rotation());
	}
	isTransformDirty_ = false;
}

TVector3 CalcFrontDirectionWithYawPitch(float yaw, float pitch)
{
	glm::vec3 front;
	front.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	return front;
}


void Camera::Setup(const TVector3& position, float yaw, float pitch, const TVector3& up)
{
	TVector3 WorldUp_ = up;

	Yaw_ = yaw;
	Pitch_ = pitch;

	if (Pitch_ > 89.0f)
		Pitch_ = 89.0f;
	if (Pitch_ < -89.0f)
		Pitch_ = -89.0f;

	Front_ = CalcFrontDirectionWithYawPitch(Yaw_, Pitch_);
	glm::vec3 right = glm::normalize(glm::cross(Front_, WorldUp_));

	Up_ = glm::normalize(glm::cross(right, Front_));

	LookAt(position, position + Front_, Up_);
}

#define IS_ZERO(f) (fabs(f) < 1e-5)

template <typename T> T sgn(T val) {
	return float((T(0) < val) - (val < T(0)));
}

void Camera::LookAt(const TVector3& from, const TVector3& center, const TVector3& up_)
{
	TVector3 front = glm::normalize(center - from);
	TVector3 up = up_;
	//if (glm::dot(front, up) > 0.99f)
	//{
	//	up =  TVector3(0, 0, -1);
	//}

	viewMatrix_ = glm::lookAtRH(from, center, up);

	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;

	bool ret = glm::decompose(viewMatrix_, scale, rotation, translation, skew, perspective);
	Assert(ret, "invalid");

	rotation = glm::conjugate(rotation);
	auto degrees = glm::degrees(glm::eulerAngles(rotation));

	auto a = viewMatrix_ * TVector4(1, 1, 0, 1);


	SetPosition(from);
	SetRotation(degrees);

	isTransformDirty_ = false;
}

const float CAMERA_MOVE_SPEED = 15.0f;

void Camera::MoveForward(float speedScale)
{
	_UpdateViewMatrix();

	auto posInViewSpace = glm::vec4(0, 0, -CAMERA_MOVE_SPEED * 0.01f * speedScale, 1);

	SetPosition( glm::inverse(viewMatrix_) * posInViewSpace);

	_UpdateViewMatrix();
}

void Camera::MoveUp(float speedScale)
{
	_UpdateViewMatrix();

	auto posInViewSpace = glm::vec4(0, CAMERA_MOVE_SPEED * 0.01f * speedScale, 0, 1);

	SetPosition(glm::inverse(viewMatrix_) * posInViewSpace);

	_UpdateViewMatrix();
}

void Camera::MoveRight(float speedScale)
{
	_UpdateViewMatrix();

	auto posInViewSpace = glm::vec4(CAMERA_MOVE_SPEED * 0.01f * speedScale, 0, 0, 1);

	SetPosition(glm::inverse(viewMatrix_) * posInViewSpace);

	_UpdateViewMatrix();
}

void Camera::RotateAroundPoint(float x, float y)
{
	glm::vec4 position(GetPosition(), 1);
	glm::vec4 pivot(TVector3{ 0, 0, 0 }, 1);
	float xAngle = x;
	float yAngle = y;

	TVector3 right = TVector3(glm::transpose(viewMatrix_)[0]);
	TVector3 up = TVector3(glm::transpose(viewMatrix_)[1]);
	TVector3 front = -TVector3(glm::transpose(viewMatrix_)[2]);


	// Extra step to handle the problem when the camera direction is the same as the up vector
	float cosAngle = glm::dot(front, up);
	if (cosAngle * sgn(yAngle) > 0.99f)
		yAngle = 0;

	// step 2: Rotate the camera around the pivot point on the first axis.
	glm::mat4x4 rotationMatrixX(1.0f);
	rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, up);
	position = (rotationMatrixX * (position - pivot)) + pivot;

	// step 3: Rotate the camera around the pivot point on the second axis.
	glm::mat4x4 rotationMatrixY(1.0f);
	rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, right);
	glm::vec3 finalPosition = (rotationMatrixY * (position - pivot)) + pivot;

	LookAt(finalPosition, pivot, up);
}

void Camera::RotateAroundAxis(float angle, const TVector3& axis)
{
	// TODO
}

void Camera::YawPitch(float yaw, float pitch)
{
	TVector3 target = glm::rotate(TVector3{ 0, 0, -1 }, glm::radians(pitch), TVector3(1, 0, 0));
	target = glm::rotate(target, glm::radians(yaw), TVector3(0, 1, 0));
	target = glm::inverse(viewMatrix_)* TVector4(target, 1.0);

	auto direction = glm::normalize(target - GetPosition());
	if (fabs(glm::dot(direction, TVector3(0, -1, 0))) > 0.99f)
	{
		return;
	}
	LookAt(GetPosition(), target);
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

void PerspectiveCamera::OnWindowResized(uint32 width, uint32 height)
{
	if (width == 0 || height == 0)
		return;
	projMatrix_ = glm::perspective(glm::radians(fov_), float(width)/float(height), nearPlane_, farPlane_);
	projMatrix_[1][1] *= -1;
}

void PerspectiveCamera::Tick(float dt)
{
	_UpdateViewMatrix();
}

NS_RX_END