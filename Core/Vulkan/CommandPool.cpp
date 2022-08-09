#include "CommandPool.hpp"
#include "Device.hpp"

namespace Vulkan {

CommandPool::CommandPool(const class Device& _device, const uint32_t queueFamilyIndex, const bool allowReset) :
	device(_device)
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndex;
	poolInfo.flags = allowReset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0; //from rt weekend
	Check(vkCreateCommandPool(device.Handle(), &poolInfo, nullptr, &commandPool),
		"create command pool");
}

CommandPool::~CommandPool()
{
	if (commandPool != nullptr)
	{
		vkDestroyCommandPool(device.Handle(), commandPool, nullptr);
		commandPool = nullptr;
	}
}

}
