#include "GameWindow.hpp"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <Asset/AssetRegistry.hpp>
#include <GLFW/glfw3native.h>
#include <Renderer/Assets/TextureAsset.hpp>
#include <Renderer/Async/TextureDownloadTask.hpp>
#include <Threading/JobManager.hpp>
namespace Hyper
{
    bool GameInput::IsKeyDown(int32 key)
    {
        return m_KeyStates[key];
    }

    bool GameInput::IsMouseDown(int32 button)
    {
        return m_MouseStates[button];
    }

    GameInput::MouseData GameInput::GetMouseData() const
    {
        return m_MouseData;
    }

    void GameInput::BeginFrame()
    {
    }

    void GameInput::EndFrame()
    {

    }

    void GameWindow::ErrorCallback(int error, const char *description)
    {

    }

    void GameWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        GameWindow* gameWindow = static_cast<GameWindow*>(glfwGetWindowUserPointer(window));
        GameInput& input = gameWindow->Input;

        if (!glfwGetWindowAttrib(window, GLFW_FOCUSED))
            return;

        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            input.m_KeyStates[key] = true;
        else
            input.m_KeyStates[key] = false;

        if (action == GLFW_PRESS)
            gameWindow->OnKeyPress.Fire(key);
    }

    void GameWindow::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        GameWindow* gameWindow = static_cast<GameWindow*>(glfwGetWindowUserPointer(window));
        GameInput& input = gameWindow->Input;

        if (!glfwGetWindowAttrib(window, GLFW_FOCUSED))
            return;

        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            input.m_MouseStates[button] = true;
        else
            input.m_MouseStates[button] = false;
    }


    void GameWindow::CharCallback(GLFWwindow* window, unsigned int codepoint)
    {

    }

    void GameWindow::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        /*GameWindow* gameWindow = static_cast<GameWindow*>(glfwGetWindowUserPointer(window));
        GameInput& input = gameWindow->Input;
        GameInput::MouseData& mouse = input.m_MouseData;

        mouse.LastMousePosition = mouse.MousePosition;
        mouse.MousePosition = Float2(xpos,ypos);

        INFO("updating");*/
    }

    void GameWindow::WindowCloseCallback(GLFWwindow* window)
    {

    }

    void GameWindow::ResizeCallback(GLFWwindow* window, int32 width, int32 height)
    {
        GameWindow* gameWindow = static_cast<GameWindow*>(glfwGetWindowUserPointer(window));
        gameWindow->Size = {static_cast<float>(width),static_cast<float>(height)};
        gameWindow->OnResize.Fire(width, height);
    }

    GameWindow::GameWindow(const String& title, const Float2& size, bool resizable)
    {
        glfwInit();

        glfwSetErrorCallback(ErrorCallback);

        glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(
            static_cast<int32>(size.X),
            static_cast<int32>(size.Y),
            title.Get(),
            nullptr, nullptr
        );

        glfwSetWindowUserPointer(m_Window, this);

        glfwSetKeyCallback(m_Window, KeyCallback);
        glfwSetCharCallback(m_Window, CharCallback);
        glfwSetCursorPosCallback(m_Window, CursorPosCallback);
        glfwSetWindowSizeCallback(m_Window, ResizeCallback);
        glfwSetFramebufferSizeCallback(m_Window, ResizeCallback);
        glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);

        Size = Float2{size.X, size.Y};
    }

    GameWindow::~GameWindow()
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;

        glfwTerminate();
    }

    void *GameWindow::GetNativePointer() const
    {
#ifdef _WIN32
        return glfwGetWin32Window(m_Window);
#endif

        return nullptr;
    }

    void GameWindow::BeginFrame()
    {
        glfwPollEvents();

        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        GameInput::MouseData& mouse = Input.m_MouseData;

        mouse.LastMousePosition = mouse.MousePosition;

        if (glfwGetWindowAttrib(m_Window, GLFW_FOCUSED))
        {
            double xpos = 0, ypos = 0;
            glfwGetCursorPos(m_Window, &xpos, &ypos);
            mouse.MousePosition = Float2(xpos,ypos);
        }

        Input.BeginFrame();
    }

    void GameWindow::EndFrame()
    {
        Input.EndFrame();
    }

    bool GameWindow::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }
}
