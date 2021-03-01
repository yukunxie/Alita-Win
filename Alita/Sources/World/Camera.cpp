//
// Created by realxie on 2019-10-29.
//

#include "Camera.h"

#include <glm/gtx/matrix_decompose.hpp>
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
	transform_.Position() = TVector3{ 0.0f, 15.0f, 15.f };
	transform_.Rotation() = TVector3{ 45.0f, 0.0f, 0.0f };

	/*viewMatrix_ = glm::lookAtRH(glm::vec3(.0f, 0.0f, -8.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));*/

	projMatrix_ = glm::perspectiveRH(glm::radians(45.0f), 1280.0f / 800.0f, 0.1f, 100.f);
	projMatrix_[1][1] *= -1;

	
	_UpdateViewMatrix();
}

TMat4x4 _ComposeViewMatrix(const TVector3& position, const TVector3& rotation)
{
	auto viewMatrix = TMat4x4(1.0f);
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.x), TVector3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), TVector3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), TVector3(0.0f, 0.0f, 1.0f));

	//translate to init position
	viewMatrix = glm::translate(viewMatrix, position * -1.f);
	return viewMatrix;
}

void PerspectiveCamera::_UpdateViewMatrix()
{
	viewMatrix_ = _ComposeViewMatrix(transform_.Position(), transform_.Rotation());

	auto xxx = glm::vec3(viewMatrix_[3]);

	if (xxx.length()) {}

	//// rotate
	//viewMatrix_ = TMat4x4(1.0f);
	//viewMatrix_ = glm::rotate(viewMatrix_, glm::radians(transform_.Rotation().x), TVector3(1.0f, 0.0f, 0.0f));
	//viewMatrix_ = glm::rotate(viewMatrix_, glm::radians(transform_.Rotation().y), TVector3(0.0f, 1.0f, 0.0f));
	//viewMatrix_ = glm::rotate(viewMatrix_, glm::radians(transform_.Rotation().z), TVector3(0.0f, 0.0f, 1.0f));

	////translate to init position
	//viewMatrix_ = glm::translate(viewMatrix_, transform_.Position() * -1.f);
}

void PerspectiveCamera::Tick(float dt)
{
	return;

	// rotate camera

	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(
		currentTime - startTime).count();

	_UpdateViewMatrix();

	TMat4x4 mat(1);
	mat = glm::rotate(mat, time * glm::radians(90.0f) * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
	
	viewMatrix_ = viewMatrix_ * mat;

	/*{
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(viewMatrix_, scale, rotation, translation, skew, perspective);

		glm::vec3 eulerRotation = glm::degrees(glm::eulerAngles(rotation));

		auto tmp = TMat4x4(1.0f);
		tmp = glm::rotate(tmp, -1.f * glm::radians(eulerRotation.x), TVector3(1.0f, 0.0f, 0.0f));
		tmp = glm::rotate(tmp, -1.f * glm::radians(eulerRotation.y), TVector3(0.0f, 1.0f, 0.0f));
		tmp = glm::rotate(tmp, -1.f * glm::radians(eulerRotation.z), TVector3(0.0f, 0.0f, 1.0f));

		auto k = tmp * viewMatrix_;

		auto x = k[0][0];
		auto y = k[1][0];
		auto z = k[2][0];
		auto w = k[3][3];

		auto xxx = glm::vec3(viewMatrix_[3]);

		rotation = glm::conjugate(rotation);
	}*/
	

}

NS_RX_END