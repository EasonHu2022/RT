#include "Buffer.hpp"
#include "Device.hpp"
#include "SingleTimeCommands.hpp"
namespace Vulkan
{
	Buffer::Buffer(const Device& _device, Allocator const&  _allocator, size_t size, VkBufferUsageFlags usage, VmaMemoryUsage aMemoryUsage)
		:device(_device),allocator(_allocator)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = aMemoryUsage;

		Check(vmaCreateBuffer(allocator.Handle(), &bufferInfo, &allocInfo, &buffer, &allocation, nullptr),
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
