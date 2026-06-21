#pragma once

#include "LayerStack.hpp"

namespace Hyper
{
	LayerStack::LayerStack()
	{
		m_LayerInsert = begin();
	}

	LayerStack::~LayerStack()
	{

	}

	void LayerStack::PushLayer(ILayer* layer)
	{
		m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer);
	}

	void LayerStack::PushOverlay(ILayer* layer)
	{
		m_Layers.emplace_back(layer);
	}

	void LayerStack::PopLayer(ILayer* layer)
	{
		LayerList::iterator it = std::find(begin(), end(), layer);
		if (it != end())
		{
			m_Layers.erase(it);
			m_LayerInsert--;
		}
	}

	void LayerStack::PophOverlay(ILayer* layer)
	{
		LayerList::iterator it = std::find(begin(), end(), layer);
		if (it != end())
			m_Layers.erase(it);
	}
}