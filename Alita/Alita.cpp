// Alita.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/vec4.hpp>
//#include <glm/mat4x4.hpp>

#include "Engine/Engine.h"
#include "Third-Party/RenderDoc/renderdoc_app.h"

#include <iostream>
#include <chrono>

#include "boost/asio.hpp""

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "vulkan-1.lib")
#pragma comment(lib, "VkLayer_utils.lib")

// --- PhysX lib
#pragma comment(lib, "LowLevelAABB_static_64.lib")
#pragma comment(lib, "LowLevelDynamics_static_64.lib")
#pragma comment(lib, "LowLevel_static_64.lib")
#pragma comment(lib, "PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysXTask_static_64.lib")
#pragma comment(lib, "PhysXVehicle_static_64.lib")
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "SampleBase_static_64.lib")
#pragma comment(lib, "SampleFramework_static_64.lib")
#pragma comment(lib, "SamplePlatform_static_64.lib")
#pragma comment(lib, "SampleRenderer_static_64.lib")
#pragma comment(lib, "SamplesToolkit_static_64.lib")
#pragma comment(lib, "Samples_64.lib")
#pragma comment(lib, "SceneQuery_static_64.lib")
#pragma comment(lib, "SimulationController_static_64.lib")
#pragma comment(lib, "SnippetRender_static_64.lib")
// --- PhysX lib end.

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

#if WIN32

RENDERDOC_API_1_4_0* gRenderDocAPI = NULL;

using WString = std::basic_string<wchar_t, std::char_traits<wchar_t>>;

WString QueryRenderdocFromRegister()
{
    const HKEY InKey = HKEY_LOCAL_MACHINE;
    const TCHAR* InSubKey = TEXT("SOFTWARE\\Classes\\RenderDoc.RDCCapture.1\\DefaultIcon\\");

    for (int RegistryIndex = 0; RegistryIndex < 2; ++RegistryIndex)
    {
        HKEY Key = 0;
        const int RegFlags = (RegistryIndex == 0) ? KEY_WOW64_32KEY : KEY_WOW64_64KEY;
        if (RegOpenKeyEx(InKey, InSubKey, 0, KEY_READ | RegFlags, &Key) == ERROR_SUCCESS)
        {
            DWORD Size = 256;
            TCHAR Buffer[256] = { 0 };
            if (RegQueryValueEx(Key, NULL, NULL, NULL, (LPBYTE)Buffer, &Size) == ERROR_SUCCESS)
            {
                while (Buffer[Size - 1] == '\0')
                    Size--;
                while (Buffer[Size - 1] != '\\' && Buffer[Size - 1] != '/')
                    Size--;
                WString OutBuffer(Buffer, Size);
                return OutBuffer;
            }
            RegCloseKey(Key);
        }
    }
    return TEXT("");
}

#endif

int main() {

    auto io_service(new boost::asio::io_service);

#if WIN32 && 1
    WString RenderdocPath = QueryRenderdocFromRegister();
    RenderdocPath += TEXT("renderdoc.dll");
    HMODULE mod = LoadLibrary(RenderdocPath.c_str());
    if (mod != NULL)
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI =
            (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_4_0, (void**)&gRenderDocAPI);
        if (ret == 1)
        {
            LOGI("Load RenderDoc Success.")
            gRenderDocAPI->MaskOverlayBits(eRENDERDOC_Overlay_FrameRate, eRENDERDOC_Overlay_FrameRate);
            gRenderDocAPI->UnloadCrashHandler();
        }
        else
        {
            gRenderDocAPI = NULL;
            LOGE("Load RenderDoc Fail.")
        }
    }
#endif


    // Console windows
    {
        HWND hwnd = GetConsoleWindow();
        ShowWindow(hwnd, 1);
    }

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 800, "Vulkan window", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    NS_RX::Engine* pEngine = NS_RX::Engine::CreateEngine(window);

    bool firstFrame = true;

    auto nowTime = std::chrono::high_resolution_clock::now();
    /*long duration = static_cast<long>(std::chrono::duration_cast<std::chrono::microseconds>(
        nowTime - startTime_).count());
    LOGI("@@@@TimeProfiler: %s : %.03fms", tag_.c_str(), duration / 1000.0f);*/

    const float frameInterval = 1 / 60.0f;

    while (!glfwWindowShouldClose(window)) {
        if (!firstFrame)
        {
            glfwPollEvents();
        }
        firstFrame = false;
        auto startTime = std::chrono::high_resolution_clock::now();
        pEngine->Update(frameInterval);
        while (true)
        {
            auto nowTime = std::chrono::high_resolution_clock::now();
            long duration = static_cast<long>(std::chrono::duration_cast<std::chrono::microseconds>(nowTime - startTime).count());
            if (duration / (1000.0f * 1000.0f) >= frameInterval)
            {
                break;
            }
            using namespace std::literals;
            std::this_thread::sleep_for(0ms);
        }
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
