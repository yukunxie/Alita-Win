#include "EventSystem.h"
#include "Engine/Engine.h"
#include "World/World.h"
#include "World/Camera.h"

#include <GLFW/glfw3.h>


NS_RX_BEGIN

EventSystem* EventSystem::instance_ = nullptr;


void EventSystem::_SetupEventRegisterToWindow(void* windowHandler)
{
	GLFWwindow* window = (GLFWwindow*)windowHandler;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwSetCursorPosCallback(window, EventSystem::_EventMouseCallback);
	glfwSetScrollCallback(window, EventSystem::_EventScrollCallback);
	glfwSetKeyCallback(window, EventSystem::_EventKeyboardCallback);
}

void EventSystem::_EventMouseHandler(double xpos, double ypos)
{

}

void EventSystem::_EventScrollHandler(double xoffset, double yoffset)
{

}

void EventSystem::_EventKeyboardHandler(int key, int scancode, int action, int mods)
{
	//if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D)
			Engine::GetWorld()->GetCamera()->MoveRight();
		else if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A)
			Engine::GetWorld()->GetCamera()->MoveLeft();
		else if (key == GLFW_KEY_UP || key == GLFW_KEY_W)
			Engine::GetWorld()->GetCamera()->MoveForward();
		else if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S)
			Engine::GetWorld()->GetCamera()->MoveBack();
	}
	
}

void EventSystem::_EventMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	instance_->_EventMouseHandler(xpos, ypos);
}

void EventSystem::_EventScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	instance_->_EventScrollHandler(xoffset, yoffset);
}

void EventSystem::_EventKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	instance_->_EventKeyboardHandler(key, scancode, action, mods);
}

NS_RX_END