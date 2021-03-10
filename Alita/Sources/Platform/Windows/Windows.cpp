
#include "Windows.h"
#include <glfw\glfw3.h>

#include <stdarg.h>
#include<iostream>

#include "RHI.h"
#include "Backend/Vulkan/VKDevice.h"

NS_RHI_BEGIN

Device* CreateDeviceHelper(DeviceType deviceType, void* data)
{
    GLFWwindow* window = static_cast<GLFWwindow*>(data);
    return new VKDevice(window);
}

NS_RHI_END

void _OutputLog(LogLevel level, const char* format, ...)
{
    va_list vaList;//equal to Format + sizeof(FOrmat)
    char szBuff[10240] = { 0 };
    va_start(vaList, format);
    vsnprintf(szBuff, sizeof(szBuff), format, vaList);
    va_end(vaList);

    std::cout << "LOG: " << szBuff << std::endl;
}

void AssertImpl(const char* filename, const char* function, int lineno, const char* format, ...)
{
    if (true);
}