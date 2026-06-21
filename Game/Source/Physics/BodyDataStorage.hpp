#pragma once

#include <Core/Common.hpp>
#include <typeinfo>
#include <functional>
#include <Game/ActionSystem.hpp>
#include <Scene/Entity.hpp>

namespace Hyper
{
    enum class BodyDataStorageType : uint8
    {
        None,
        ActionProvider,
        Entity,
        Custom
    };

    class BodyDataStorage
    {
    private:
        void* m_Data = nullptr;
        std::function<void*(const std::type_info&)> m_TypeCaster = nullptr;

    public:
        BodyDataStorageType Type = BodyDataStorageType::None;

        // Default constructor
        BodyDataStorage() = default;

        template<typename T>
        BodyDataStorage(T* ptr, BodyDataStorageType type = BodyDataStorageType::Custom)
            : m_Data(ptr), Type(type)
        {
            SetupTypeCasting<T>();
        }

        template<typename T>
        static BodyDataStorage* CreateActionProvider(T* ptr)
        {
            static_assert(std::is_base_of_v<IActionProvider, T>, "T must inherit from IActionProvider");
            return new BodyDataStorage(ptr, BodyDataStorageType::ActionProvider);
        }

        template<typename T>
        T* As()
        {
            if (!m_Data) return nullptr;

            if (m_TypeCaster) {
                void* result = m_TypeCaster(typeid(T));
                return static_cast<T*>(result);
            }

            return reinterpret_cast<T*>(m_Data);
        }

        template<typename T>
        bool Is() const
        {
            if (!m_Data || !m_TypeCaster) return false;
            return m_TypeCaster(typeid(T)) != nullptr;
        }

        void* GetRawData() const { return m_Data; }

        bool IsValid() const { return m_Data != nullptr; }

        void Clear()
        {
            m_Data = nullptr;
            m_TypeCaster = nullptr;
            Type = BodyDataStorageType::None;
        }

    private:
        template<typename ConcreteType>
        void SetupTypeCasting()
        {
            m_TypeCaster = [this](const std::type_info& targetType) -> void* {
                if (!m_Data) return nullptr;

                ConcreteType* concretePtr = static_cast<ConcreteType*>(m_Data);

                if (targetType == typeid(ConcreteType)) {
                    return concretePtr;
                }

                if (targetType == typeid(IActionProvider)) {
                    if constexpr (std::is_base_of_v<IActionProvider, ConcreteType>) {
                        return static_cast<IActionProvider*>(concretePtr);
                    }
                }

                if (targetType == typeid(Entity)) {
                    if constexpr (std::is_base_of_v<Entity, ConcreteType>) {
                        return static_cast<Entity*>(concretePtr);
                    }
                }

                return nullptr;
            };
        }
    };
}
