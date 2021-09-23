
#include "Windows.h"
#include <glfw\glfw3.h>

#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <fstream>

#include "RHI/RHI.h"
#include "Backend/Vulkan/VKDevice.h"

NS_RHI_BEGIN

//Device* CreateDeviceHelper(DeviceType deviceType, void* data)
//{
//    GLFWwindow* window = static_cast<GLFWwindow*>(data);
//    return new VKDevice(window);
//}


NS_RHI_END

void _OutputLog(LogLevel level, const char* format, ...)
{
    va_list vaList;//equal to Format + sizeof(FOrmat)
    char szBuff[10240] = { 0 };
    va_start(vaList, format);
    vsnprintf(szBuff, sizeof(szBuff), format, vaList);
    va_end(vaList);

    std::stringstream logger;

    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    logger << std::put_time(std::localtime(&t), "%Y-%m-%d %X ") << szBuff << std::endl;

    static std::ofstream _FileLogger;
    if (!_FileLogger.is_open())
    {
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "%Y%m%d-%H%M%S");
        std::string dir = "Log/";
        std::string filename = dir + ss.str() + ".log";

#if WIN32
        std::wstring wdir;
        {
            //获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
            int len = MultiByteToWideChar(CP_ACP, 0, dir.c_str(), dir.size(), NULL, 0);
            TCHAR* buffer = new TCHAR[len + 1];
            //多字节编码转换成宽字节编码  
            MultiByteToWideChar(CP_ACP, 0, dir.c_str(), dir.size(), buffer, len);
            buffer[len] = '\0';
            //删除缓冲区并返回值  
            wdir.append(buffer);
            delete[] buffer;
        }

        auto flag = GetFileAttributesA(dir.c_str());

        if (INVALID_FILE_ATTRIBUTES == flag || (!(flag & FILE_ATTRIBUTE_DIRECTORY))) {
            bool flag = CreateDirectory(wdir.c_str(), NULL);
            Assert(flag, "create 'Log/' dir fail.");
        }
#else
        Assert(false, "");
#endif
        _FileLogger.open(filename, std::ios::out);
    }

    std::cout << logger.str();
    _FileLogger << logger.str();
    _FileLogger.flush();
}

void AssertImpl(const char* filename, const char* function, int lineno, const char* format, ...)
{
    if (true);
}