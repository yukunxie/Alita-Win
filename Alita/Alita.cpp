﻿// Alita.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/vec4.hpp>
//#include <glm/mat4x4.hpp>

#include "Engine/Engine.h"

#include <iostream>
#include <chrono>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "vulkan-1.lib")
#pragma comment(lib, "VkLayer_utils.lib")

#include <Windows.h>

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (true) {}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (true) {}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
    }
}

int main() {

    // Console windows
    {
        HWND hwnd = GetConsoleWindow();
        ShowWindow(hwnd, 1);
    }

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 800, "Vulkan window", nullptr, nullptr);

    NS_RX::Engine* pEngine = NS_RX::Engine::CreateEngine(window);

    bool firstFrame = true;

    while (!glfwWindowShouldClose(window)) {
        if (!firstFrame)
        {
            glfwPollEvents();
        }
        firstFrame = false;
        pEngine->Update(0.016f);
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
