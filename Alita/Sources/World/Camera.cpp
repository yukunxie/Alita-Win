//
// Created by realxie on 2019-10-29.
//

#include "Camera.h"

#include <chrono>

NS_RX_BEGIN

#define PI 3.1415926f
#define ANGLE_TO_RAD(angle) ( (angle) * PI / 180.0f)

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
	transform_.Position() = TVector3{ 0.0f, 8.0f, 8.0f };
	transform_.Rotation() = TVector3{ 45.0f, 0.0f, 0.0f };

	/*viewMatrix_ = glm::lookAtRH(glm::vec3(.0f, 0.0f, -8.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));*/

	projMatrix_ = glm::perspectiveRH(glm::radians(45.0f), 1280.0f / 800.0f, 0.1f, 100.f);
	projMatrix_[1][1] *= -1;

	// rotate
	viewMatrix_ = TMat4x4(1.0f);
	viewMatrix_ = glm::rotate(viewMatrix_, glm::radians(transform_.Rotation().x), TVector3(1.0f, 0.0f, 0.0f));
	viewMatrix_ = glm::rotate(viewMatrix_, glm::radians(transform_.Rotation().y), TVector3(0.0f, 1.0f, 0.0f));
	viewMatrix_ = glm::rotate(viewMatrix_, glm::radians(transform_.Rotation().z), TVector3(0.0f, 0.0f, 1.0f));

	//translate to init position
	viewMatrix_ = glm::translate(viewMatrix_, transform_.Position() * -1.f);
}

void PerspectiveCamera::Tick(float dt)
{
	//static auto startTime = std::chrono::high_resolution_clock::now();
	//auto currentTime = std::chrono::high_resolution_clock::now();
	//float time = std::chrono::duration<float, std::chrono::seconds::period>(
	//	currentTime - startTime).count();

	//TMat4x4 mat(1);
	//mat = glm::rotate(mat, time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//TVector4 pos = TVector4(.0f, 0.0f, -8.0f, 1.0) * mat;

	//viewMatrix_ = glm::lookAtRH(TVector3(pos), glm::vec3(0.0f, 0.0f, 0.0f),
	//	glm::vec3(0.0f, 1.0f, 0.0f));
}

NS_RX_END