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
        Instance_ = this;
    }

    static EventSystem* GetInstance()
    {
        return Instance_;
    }

protected:
    void _SetupEventRegisterToWindow(void* windowHandler);

    void _EventMouseHandler(float xpos, float ypos);

    void _EventScrollHandler(float xoffset, float yoffset);

    void _EventKeyboardHandler(int key, int scancode, int action, int mods);

    void _EventMouseButtonHandler(int button, int action, int mods, double xpos, double ypos);

    void _EventClickMouseButton(int button, double xpos, double ypos);

    void _EventWindowResizedHandler(int width, int height);

    static void _EventMouseButtonCallbak(GLFWwindow* window, int button, int action, int mods);

    static void _EventMouseCallback(GLFWwindow* window, double xpos, double ypos);

    static void _EventScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static void _EventKeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void _EventOnWindowResized(GLFWwindow* window, int width, int height);

private:
    static EventSystem* Instance_;

    void* Window_ = nullptr;

    bool LeftMouseBtnPressing_ = false;
    bool RightMouseBtnPressing_ = false;
    bool IsMouseMoving_ = false;
    TVector2 LastMousePosition_ = TVector2(0);

    std::chrono::high_resolution_clock::time_point MousePressDownTimestamp_;
};
NS_RX_END

