#include "EventSystem.h"
#include "Engine/Engine.h"
#include "World/World.h"
#include "World/Camera.h"


#include <GLFW/glfw3.h>

#include <iostream>

#if WIN32
#include "../Third-Party/RenderDoc/renderdoc_app.h"
extern RENDERDOC_API_1_4_0* gRenderDocAPI;
#endif


NS_RX_BEGIN

EventSystem* EventSystem::Instance_ = nullptr;


void EventSystem::_SetupEventRegisterToWindow(void* windowHandler)
{
	Window_ = windowHandler;
	GLFWwindow* window = (GLFWwindow*)windowHandler;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwSetCursorPosCallback(window, EventSystem::_EventMouseCallback);
	glfwSetScrollCallback(window, EventSystem::_EventScrollCallback);
	glfwSetKeyCallback(window, EventSystem::_EventKeyboardCallback);
	glfwSetMouseButtonCallback(window, EventSystem::_EventMouseButtonCallbak);
	glfwSetWindowSizeCallback(window, EventSystem::_EventOnWindowResized);
}

void EventSystem::_EventMouseHandler(float xpos, float ypos)
{
	auto now = std::chrono::high_resolution_clock::now();
	using namespace std::literals;

	TVector2 offset = glm::abs(TVector2{ xpos, ypos } - LastMousePosition_);

	if ((now - MousePressDownTimestamp_ > 500ms) || (offset.x + offset.y >= 20))
	{
		if (LeftMouseBtnPressing_)
		{
			TVector2 diff = (TVector2{ xpos, ypos } - LastMousePosition_) * 0.1f;
			Engine::GetWorld()->GetCamera()->RotateAroundPoint(diff.x, -diff.y);
		}
		else if (RightMouseBtnPressing_)
		{
			TVector2 diff = (TVector2{ xpos, ypos } - LastMousePosition_) * 0.1f;
			Engine::GetWorld()->GetCamera()->YawPitch(diff.x, diff.y);
		}
		IsMouseMoving_ = true;
	}
	LastMousePosition_ = { xpos, ypos };
}

void EventSystem::_EventScrollHandler(float xoffset, float yoffset)
{
	Engine::GetWorld()->GetCamera()->MoveForward(yoffset * 5.0f);
}

void EventSystem::_EventKeyboardHandler(int key, int scancode, int action, int mods)
{
	if (!LeftMouseBtnPressing_ && !RightMouseBtnPressing_)
	{
		if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D)
			Engine::GetWorld()->GetCamera()->MoveRight(1.0f);
		else if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A)
			Engine::GetWorld()->GetCamera()->MoveRight(-1.0f);
		else if (key == GLFW_KEY_UP || key == GLFW_KEY_W)
			Engine::GetWorld()->GetCamera()->MoveForward(1.0f);
		else if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S)
			Engine::GetWorld()->GetCamera()->MoveForward(-1.0f);
		else
		{
#if WIN32
			if (gRenderDocAPI && key == GLFW_KEY_F12 && mods == GLFW_MOD_CONTROL && action == GLFW_PRESS)
			{
				gRenderDocAPI->TriggerCapture();
				if (!gRenderDocAPI->IsRemoteAccessConnected())
				{
					gRenderDocAPI->LaunchReplayUI(true, NULL);
				}
			}
#endif
		}
	}


}

void EventSystem::_EventMouseButtonHandler(int button, int action, int mods, double xpos, double ypos)
{
	if (action == GLFW_PRESS)
	{
		MousePressDownTimestamp_ = std::chrono::high_resolution_clock::now();
		IsMouseMoving_ = false;
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			LeftMouseBtnPressing_ = true;
			RightMouseBtnPressing_ = false;
			LastMousePosition_ = { xpos, ypos };
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			LeftMouseBtnPressing_ = false;
			RightMouseBtnPressing_ = true;
			LastMousePosition_ = { xpos, ypos };
			break;
		default:
			return;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			LeftMouseBtnPressing_ = false;
			if (!IsMouseMoving_)
			{
				_EventClickMouseButton(GLFW_MOUSE_BUTTON_LEFT, xpos, ypos);
			}
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			RightMouseBtnPressing_ = false;
			break;
		default:
			return;
		}
	}
	else if (action == GLFW_REPEAT)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			break;
		default:
			return;
		}
	}
}

void EventSystem::_EventClickMouseButton(int button, double xpos, double ypos)
{
	if (GLFW_MOUSE_BUTTON_LEFT != button)
	{
		return;
	}

	TExtent2D size = Engine::GetEngine()->GetWindowSize();
	if (size.width == 0 || size.height == 0)
	{
		return;
	}

	auto VPMatrix = Engine::GetWorld()->GetCamera()->GetViewProjectionMatrix();

	float u = glm::clamp((float(xpos) / size.width) * 2 - 1.0f, -1.0f, 1.0f);
	float v = glm::clamp((float(ypos) / size.height) * 2 - 1.0f, -1.0f, 1.0f);

	TVector3 ray = glm::inverse(VPMatrix)* TVector4(u, v, 1.0, 1.0);

	Physics::GetInstance().Pick(Engine::GetWorld()->GetCamera()->GetPosition(), glm::normalize(ray));

}

void EventSystem::_EventWindowResizedHandler(int width, int height)
{
	Engine::GetEngine()->SetWindowResized(width, height);
}

void EventSystem::_EventMouseButtonCallbak(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	Instance_->_EventMouseButtonHandler(button, action, mods, (float)xpos, (float)ypos);
}

void EventSystem::_EventMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Instance_->_EventMouseHandler(xpos, ypos);
}

void EventSystem::_EventScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Instance_->_EventScrollHandler((float)xoffset, (float)yoffset);
}

void EventSystem::_EventKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Instance_->_EventKeyboardHandler(key, scancode, action, mods);
}

void EventSystem::_EventOnWindowResized(GLFWwindow* window, int width, int height)
{
	Instance_->_EventWindowResizedHandler(width, height);
}

NS_RX_END