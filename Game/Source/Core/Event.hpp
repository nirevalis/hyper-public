#pragma once

#include <Core/Common.hpp>
#include <mutex>
#include <shared_mutex>
#include <iostream>
#include <functional>
namespace Hyper
{
	template <typename... Args>
	class Delegate
	{
	private:
		struct CallbackData;

		struct DelegateState
		{
			mutable std::shared_mutex _mutex;
			std::vector<CallbackData> _callbacks;
			mutable std::atomic<int32> _fireCount { 0 };
		};

		struct CallbackData
		{
			using Callback = std::function<void(Args...)>;
			Callback callback;

			CallbackData(Callback&& cb)
				: callback(std::move(cb))
			{
			}
		};

	public:
		using Callback = std::function<void(Args...)>;

		Delegate() : _state(std::make_shared<DelegateState>())
		{
		}

		Delegate(const Delegate& other) = default;
		Delegate& operator=(const Delegate& other) = default;
		Delegate(Delegate&& other) noexcept = default;
		Delegate& operator=(Delegate&& other) noexcept = default;

		void Connect(Callback&& callback)
		{
			if (!_state) return;

			_state->_callbacks.emplace_back(std::move(callback));
		}

		void Fire(Args... args) const
		{
			if (!_state) return;

			std::vector<Callback> callbacksToFire;
			{
				std::shared_lock lock(_state->_mutex);
				callbacksToFire.reserve(_state->_callbacks.size());
				for (const auto& cbData : _state->_callbacks)
				{
					callbacksToFire.push_back(cbData.callback);
				}
			}

			for (const auto& cb : callbacksToFire)
			{
				try
				{
					cb(args...);
				}
				catch (const std::exception& e)
				{
					std::cerr << "Exception caught in callback: " << e.what() << std::endl;
				}
			}

			if (++_state->_fireCount > 10)
			{
				_state->_fireCount = 0;
				Cleanup();
			}
		}
	private:
		void Cleanup() const
		{
			if (!_state) return;
		}

		std::shared_ptr<DelegateState> _state;
	};

}