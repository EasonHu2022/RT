#pragma once
#include "Wrapper.hpp"
#include "Allocator.hpp"
namespace Vulkan
{
	class CommandPool;
	class Device;
	class Buffer
	{
	public:
		VULKAN_OBJECT(Buffer)
		Buffer(const Device& device, Allocator const& allocator, size_t size, VkBufferUsageFlags usage, VmaMemoryUsage aMemoryUsage);
		~Buffer();
		const class Device& get_device() const { return device; }

		//copy from
		void copy(CommandPool& commandPool, const Buffer& src, VkDeviceSize size);
		VmaAllocation allocation = VK_NULL_HANDLE;
	private:

		const class Device& device;
		const class Allocator& allocator;
		VULKAN_HANDLES(VkBuffer, buffer)
	};

}

