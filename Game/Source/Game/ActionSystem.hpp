#pragma once

#include <Core/Common.hpp>
#include <functional>

namespace Hyper
{
    class Scene;

    struct Action
    {
        int32 Keybind;
        String Text;
        std::function<void()> Callback;

        Action() : Keybind(0), Text("")
        {
        }

        Action(int32 keybind, const String& text, const std::function<void()>& callback)
            : Keybind(keybind), Text(text), Callback(callback)
        {

        }
    };

    class IActionProvider
    {
    public:
        String ObjectName;

        IActionProvider() = default;
        virtual ~IActionProvider() = default;

        [[nodiscard]] virtual std::vector<Action> Provide() = 0;
    };

    class ActionSystem
    {
    private:
        std::unordered_map<String, std::vector<Action>> m_Actions;
        std::unordered_map<uint32, Action*> m_KeyToAction;

        void onKeyPress(int32 key);
    public:
        ActionSystem();

        void Update(Scene* scene);
        std::unordered_map<String, std::vector<Action>>& GetActions();
    };
}
