#pragma once

#include <unordered_map>
#include <Core/Event.hpp>
#include "Event.hpp"

namespace Hyper
{
    class EventDispatcher
    {
    private:
        std::unordered_map<EventType, Hyper::Delegate<Event&>> m_EventHandlers;
    public:
        void DispatchEvent(Event& event);
        void Connect(EventType eventType, Hyper::Delegate<Event&>::Callback&& callback);
    };
}
