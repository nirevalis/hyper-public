#include "EventDispatcher.hpp"

namespace Hyper
{
    void EventDispatcher::Connect(EventType eventType, Hyper::Delegate<Event &>::Callback &&callback)
    {
        m_EventHandlers[eventType].Connect(std::move(callback));
    }

    void EventDispatcher::DispatchEvent(Event &event)
    {
        m_EventHandlers[event.GetEventType()].Fire(event);
    }
}