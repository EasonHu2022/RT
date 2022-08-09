#pragma once

#include "Wrapper.hpp"

namespace Vulkan
{
	class Device;
	class CommandPool 
	{
	public:
		VULKAN_OBJECT(CommandPool)
		CommandPool(const Device& device, uint32_t queueFamilyIndex, bool allowReset);
		~CommandPool();
		const class Device& get_device() const { return device; }
	private:
		VULKAN_HANDLES(VkCommandPool, commandPool)
		const class Device& device;	
	};

}
