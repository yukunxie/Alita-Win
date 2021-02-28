// Alita.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/vec4.hpp>
//#include <glm/mat4x4.hpp>

#include "Engine/Engine.h"

#include <iostream>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "vulkan-1.lib")
#pragma comment(lib, "VkLayer_utils.lib")

#include <Windows.h>

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

   /* uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported" << std::endl;*/

    //glm::mat4 matrix;
    //glm::vec4 vec;
    //auto test = matrix * vec/*;*/

    while (!glfwWindowShouldClose(window)) {
        pEngine->Update(0.016f);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
