#pragma once

#include "Base/ObjectBase.h"
#include "Types/Types.h"

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

    void _EventMouseHandler(float xpos, float ypos);

    void _EventScrollHandler(float xoffset, float yoffset);

    void _EventKeyboardHandler(int key, int scancode, int action, int mods);

    void _EventMouseButtonHandler(int button, int action, int mods, double xpos, double ypos);

    static void _EventMouseButtonCallbak(GLFWwindow* window, int button, int action, int mods);

    static void _EventMouseCallback(GLFWwindow* window, double xpos, double ypos);

    static void _EventScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static void _EventKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    static EventSystem* instance_;

    bool bLeftMouseBtnPressing_ = false;
    TVector2 lastMousePosition_ = TVector2(0);
};
NS_RX_END

