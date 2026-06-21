#pragma once

#include <Core/Common.hpp>
#include "Layer.hpp"
#include <vector>

namespace Hyper
{
	class HYPER_API LayerStack
	{
	private:
		using LayerList = std::vector<ILayer*>;

		LayerList m_Layers;
		LayerList::iterator m_LayerInsert;

	public:
		LayerStack();
		~LayerStack();

		void PushLayer(ILayer* layer);
		void PushOverlay(ILayer* layer);
		void PopLayer(ILayer* layer);
		void PophOverlay(ILayer* layer);

		LayerList::iterator begin()
		{
			return m_Layers.begin();
		}

		LayerList::iterator end()
		{
			return m_Layers.end();
		}
	};

	class HYPER_API LayerStackEmitter
	{
	public:
		LayerStackEmitter() = delete;
		~LayerStackEmitter() = delete;

		static void EmitStart(Hyper::LayerStack& stack);
		static void EmitExit(Hyper::LayerStack& stack);
		static void EmitEvent(Hyper::LayerStack& stack, Hyper::Event& event);
	};
}