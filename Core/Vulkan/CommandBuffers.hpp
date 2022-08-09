#pragma once

#include "wrapper.hpp"
#include <vector>

namespace Vulkan
{
	class CommandPool;

	class CommandBuffers 
	{
	public:

		VULKAN_OBJECT(CommandBuffers)

		CommandBuffers(CommandPool& commandPool, uint32_t size);
		~CommandBuffers();

		uint32_t get_size() const { return static_cast<uint32_t>(commandBuffers.size()); }
		VkCommandBuffer& operator [] (const size_t i) { return commandBuffers[i]; }

		VkCommandBuffer Begin(size_t i);
		void End(size_t);

	private:

		const CommandPool& commandPool;

		std::vector<VkCommandBuffer> commandBuffers;
	};

}
