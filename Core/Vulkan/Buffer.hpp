#pragma once
#include "Wrapper.hpp"
#include "DeviceMemory.hpp"
namespace Vulkan
{
	class CommandPool;
	class Device;
	class Buffer
	{
	public:
		VULKAN_OBJECT(Buffer)
		Buffer(const Device& device, size_t size, VkBufferUsageFlags usage);
		~Buffer();
		const class Device& get_device() const { return device; }
		DeviceMemory allocate_memory(VkMemoryPropertyFlags propertyFlags);
		DeviceMemory allocate_memory(VkMemoryAllocateFlags allocateFlags, VkMemoryPropertyFlags propertyFlags);
		VkMemoryRequirements get_memory_requirements() const;
		VkDeviceAddress get_device_address() const;
		void copy(CommandPool& commandPool, const Buffer& src, VkDeviceSize size);
	private:

		const class Device& device;
		VULKAN_HANDLES(VkBuffer, buffer)
	};

}

