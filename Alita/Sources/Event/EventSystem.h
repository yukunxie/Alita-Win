#pragma once

#include "Base/ObjectBase.h"

NS_RX_BEGIN

class EventSystem : public ObjectBase
{
public:
    EventSystem(void* windowHandler)
    {
        _SetupEventRegisterToWindow(windowHandler);
        instance_ = this;
    }

    static EventSystem* GetInstance()
    {
        return instance_;
    }

protected:
    void _SetupEventRegisterToWindow(void* windowHandler);

    void _EventMouseHandler(double xpos, double ypos);

    void _EventScrollHandler(double xoffset, double yoffset);

    void _EventKeyboardHandler(int key, int scancode, int action, int mods);

    static void _EventMouseCallback(GLFWwindow* window, double xpos, double ypos);

    static void _EventScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static void _EventKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    static EventSystem* instance_;
};
NS_RX_END

