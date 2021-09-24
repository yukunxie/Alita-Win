#include "EventSystem.h"
#include "Engine/Engine.h"
#include "World/World.h"
#include "World/Camera.h"

#include <GLFW/glfw3.h>

#include <iostream>

#if WIN32
#include "../Third-Party/RenderDoc/renderdoc_app.h"
extern RENDERDOC_API_1_4_0* rdoc_api;
#endif


NS_RX_BEGIN

EventSystem* EventSystem::instance_ = nullptr;

void EventSystem::_SetupEventRegisterToWindow(void* windowHandler)
{
	GLFWwindow* window = (GLFWwindow*)windowHandler;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwSetCursorPosCallback(window, EventSystem::_EventMouseCallback);
	glfwSetScrollCallback(window, EventSystem::_EventScrollCallback);
	glfwSetKeyCallback(window, EventSystem::_EventKeyboardCallback);
	glfwSetMouseButtonCallback(window, EventSystem::_EventMouseButtonCallbak);
}

void EventSystem::_EventMouseHandler(float xpos, float ypos)
{
	if (bLeftMouseBtnPressing_)
	{
		TVector2 diff = (TVector2{ xpos, ypos } - lastMousePosition_) * 0.1f;
		Engine::GetWorld()->GetCamera()->MoveRight(-diff.x);
		Engine::GetWorld()->GetCamera()->MoveUp(diff.y);
	}

	lastMousePosition_ = { xpos, ypos };
}

void EventSystem::_EventScrollHandler(float xoffset, float yoffset)
{
	Engine::GetWorld()->GetCamera()->MoveForward(yoffset * 5.0f);
}

void EventSystem::_EventKeyboardHandler(int key, int scancode, int action, int mods)
{
	if (!bLeftMouseBtnPressing_)
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
			if (rdoc_api && key == GLFW_KEY_F12 && mods == GLFW_MOD_CONTROL && action == GLFW_PRESS)
			{
				rdoc_api->TriggerCapture();
				if (!rdoc_api->IsRemoteAccessConnected())
				{
					rdoc_api->LaunchReplayUI(true, NULL);
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
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			bLeftMouseBtnPressing_ = true;
			lastMousePosition_ = { xpos, ypos };
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
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
			bLeftMouseBtnPressing_ = false;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
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

void EventSystem::_EventMouseButtonCallbak(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	instance_->_EventMouseButtonHandler(button, action, mods, (float)xpos, (float)ypos);
}

void EventSystem::_EventMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	instance_->_EventMouseHandler(xpos, ypos);
}

void EventSystem::_EventScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	instance_->_EventScrollHandler((float)xoffset, (float)yoffset);
}

void EventSystem::_EventKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	instance_->_EventKeyboardHandler(key, scancode, action, mods);
}

NS_RX_END