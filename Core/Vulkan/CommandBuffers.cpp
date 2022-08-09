#include "CommandBuffers.hpp"
#include "CommandPool.hpp"
#include "Device.hpp"

namespace Vulkan {

CommandBuffers::CommandBuffers(CommandPool& _commandPool, const uint32_t size) :
	commandPool(_commandPool)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = _commandPool.Handle();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = size;

	commandBuffers.resize(size);

	Check(vkAllocateCommandBuffers(commandPool.get_device().Handle(), &allocInfo, commandBuffers.data()),
		"allocate command buffers");
}

CommandBuffers::~CommandBuffers()
{
	if (!commandBuffers.empty())
	{
		vkFreeCommandBuffers(commandPool.get_device().Handle(), commandPool.Handle(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}
}

VkCommandBuffer CommandBuffers::Begin(const size_t i)
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional

	Check(vkBeginCommandBuffer(commandBuffers[i], &beginInfo),
		"begin recording command buffer");

	return commandBuffers[i];
}

void CommandBuffers::End(const size_t i)
{
	Check(vkEndCommandBuffer(commandBuffers[i]),
		"record command buffer");
}

}
