#pragma once

#include <Core/Common.hpp>
#include <Core/Event.hpp>
#include <Core/Math/Float2.hpp>
#include <GLFW/glfw3.h>

namespace Hyper
{
    class GameInput
    {
    public:
        struct MouseData
        {
            Float2 LastMousePosition = Float2::Zero;
            Float2 MousePosition = Float2::Zero;
        };

    private:
        friend class GameWindow;
        std::unordered_map<int32, bool> m_KeyStates;
        std::unordered_map<int32, bool> m_MouseStates;
        MouseData m_MouseData;
        bool m_FirstMouse = true;
        float m_MouseSensitivity = 1.0f;

    public:
        bool IsKeyDown(int32 key);
        bool IsMouseDown(int32 button);

        void BeginFrame();
        void EndFrame();

        MouseData GetMouseData() const;
    };

    class GameWindow
    {
    private:
        GLFWwindow* m_Window;

        static void ErrorCallback(int error, const char* description);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void CharCallback(GLFWwindow* window, unsigned int codepoint);
        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void WindowCloseCallback(GLFWwindow* window);
        static void ResizeCallback(GLFWwindow* window, int width, int height);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    public:
        GameInput Input;

        Delegate<int32, int32> OnResize;
        Delegate<int32> OnKeyPress;

        Float2 Size;

        GameWindow(const String& title, const Float2& size, bool resizable);
        ~GameWindow();

        void BeginFrame();
        void EndFrame();

        bool ShouldClose() const;
        void* GetNativePointer() const;
    };
}
