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

static TMat4x4 _ComposeViewMatrix(const TVector3& position, const TVector3& rotation)
{
	//auto viewMatrix = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	auto viewMatrix = TMat4x4(1.0f);
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.x), TVector3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), TVector3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), TVector3(0.0f, 0.0f, 1.0f));

	//translate to init position
	viewMatrix = glm::transpose(viewMatrix) * glm::translate(TMat4x4(1.0f), position);

	auto a = viewMatrix * TVector4(0, 0, 0, 1);
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
	if (IS_ZERO(fabs(front.y) - 1))
	{
		up =  TVector3(0, 0.0f, -1);
	}

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

	auto a = viewMatrix_ * TVector4(0, 0, 0, 1);


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

void Camera::RotateAroundPoint(float x, float y)
{
	auto rotation = GetRotation();
	rotation.y += y/10;
	rotation.x += x/10;
	SetRotation(rotation);
	//float distance = glm::length(GetPosition());
	//TVector3 center = distance * glm::normalize(Front_) + GetPosition();
	//TVector3 relPos = GetPosition() - center;

	////1、初始化一个单位矩阵
	//glm::mat4 trans = glm::mat4(1.0f);
	////2、用glm::radians将角度转化为弧度，glm::vec3(0.0, 0.0, 1.0)表示沿Z轴旋转
	//trans = glm::rotate(trans, glm::radians(x), glm::vec3(1.0, 0.0, 0.0));
	//trans = glm::rotate(trans, glm::radians(y), glm::vec3(0.0, 1.0, 0.0));

	//auto newPosition = glm::vec3(glm::vec4((GetPosition() - center), 1.0) * trans) + center;
	//LookAt(newPosition, center, { 0, 1, 0 });


	//TVector3 right = glm::cross(Front_, WorldUp_);

	//point = glm::rotate(point, x / 10, right);
	////point = glm::rotate(point, y / 10, WorldUp_);

	///*Front_ = glm::normalize(point);
	//WorldUp_ = glm::cross(Front_, right);*/

	//SetPosition(point + center);

	//LookAt(GetPosition(), center);
}

void Camera::YawPitch(float yaw, float pitch)
{
	auto rotation = GetRotation();
	rotation.y += yaw;
	rotation.x += pitch;
	SetRotation(rotation);

	/*Yaw_ += yaw;

	Front_ = CalcFrontDirectionWithYawPitch(Yaw_, Pitch_);
	glm::vec3 right = glm::normalize(glm::cross(Front_, WorldUp_));

	Up_ = glm::normalize(glm::cross(right, Front_));
	LookAt(GetPosition(), GetPosition() + Front_, Up_);*/
}

//void Camera::Pitch(float pitch)
//{
//	Pitch_ += pitch;
//
//	if (Pitch_ > 89.0f)
//		Pitch_ = 89.0f;
//	if (Pitch_ < -89.0f)
//		Pitch_ = -89.0f;
//
//	Front_ = CalcFrontDirectionWithYawPitch(Yaw_, Pitch_);
//	glm::vec3 right = glm::normalize(glm::cross(Front_, WorldUp_));
//
//	Up_ = glm::normalize(glm::cross(right, Front_));
//	LookAt(GetPosition(), GetPosition() + Front_, Up_);
//}

OrthoCamera::OrthoCamera(float left, float right, float bottom, float top, float zNear, float zFar)
{
	projMatrix_ = glm::orthoRH(left, right, bottom, top, zNear, zFar);

	_UpdateViewMatrix();
}

void OrthoCamera::Tick(float dt)
{
	//_UpdateViewMatrix();
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
	//_UpdateViewMatrix();
}

NS_RX_END