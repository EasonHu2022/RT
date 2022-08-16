#include "Buffer.hpp"
#include "Device.hpp"
#include "SingleTimeCommands.hpp"
namespace Vulkan
{
	Buffer::Buffer(const Device& _device, size_t size, VkBufferUsageFlags usage)
		:device(_device)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		Check(vkCreateBuffer(device.Handle(), &bufferInfo, nullptr, &buffer),
			"create buffer");
	}

	Buffer::~Buffer()
	{
		if (buffer != nullptr)
		{
			vkDestroyBuffer(device.Handle(), buffer, nullptr);
			buffer = nullptr;
		}
	}

	DeviceMemory Buffer::allocate_memory(VkMemoryPropertyFlags propertyFlags)
	{
		return allocate_memory(0, propertyFlags);
	}

	DeviceMemory Buffer::allocate_memory(VkMemoryAllocateFlags allocateFlags, VkMemoryPropertyFlags propertyFlags)
	{
		const auto requirements = get_memory_requirements();
		DeviceMemory memory(device, requirements.size, requirements.memoryTypeBits, allocateFlags, propertyFlags);

		Check(vkBindBufferMemory(device.Handle(), buffer, memory.Handle(), 0),
			"bind buffer memory");

		return memory;
	}

	VkMemoryRequirements Buffer::get_memory_requirements() const
	{
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device.Handle(), buffer, &requirements);
		return requirements;
	}

	VkDeviceAddress Buffer::get_device_address() const
	{
		VkBufferDeviceAddressInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		info.pNext = nullptr;
		info.buffer = Handle();

		return vkGetBufferDeviceAddress(device.Handle(), &info);
	}


	void Buffer::copy(CommandPool& commandPool, const Buffer& src, VkDeviceSize size)
	{
		SingleTimeCommands::Submit(commandPool, [&](VkCommandBuffer commandBuffer)
			{
				VkBufferCopy copyRegion = {};
				copyRegion.srcOffset = 0; // Optional
				copyRegion.dstOffset = 0; // Optional
				copyRegion.size = size;

				vkCmdCopyBuffer(commandBuffer, src.Handle(), Handle(), 1, &copyRegion);
			});

	}

};
