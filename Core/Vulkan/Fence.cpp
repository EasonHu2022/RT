#include "Fence.hpp"
#include "Device.hpp"

namespace Vulkan {

Fence::Fence(const class Device& _device, const bool signaled) :
	device(_device)
{
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	Check(vkCreateFence(device.Handle(), &fenceInfo, nullptr, &fence),
		"create fence");
}

Fence::Fence(Fence&& other) noexcept :
	device(other.device),
	fence(other.fence)
{
	other.fence = nullptr;
}

Fence::~Fence()
{
	if (fence != nullptr)
	{
		vkDestroyFence(device.Handle(), fence, nullptr);
		fence = nullptr;
	}
}

void Fence::Reset()
{
	Check(vkResetFences(device.Handle(), 1, &fence),
		"reset fence");
}

void Fence::Wait(const uint64_t timeout) const
{
	Check(vkWaitForFences(device.Handle(), 1, &fence, VK_TRUE, timeout),
		"wait for fence");
}

}
