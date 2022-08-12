#include "Semaphore.hpp"
#include "Device.hpp"

namespace Vulkan {

Semaphore::Semaphore(const class Device& _device) :
	device(_device)
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	Check(vkCreateSemaphore(device.Handle(), &semaphoreInfo, nullptr, &semaphore),
		"create semaphores");
}

Semaphore::Semaphore(Semaphore&& other) noexcept :
	device(other.device),
	semaphore(other.semaphore)
{
	other.semaphore = nullptr;
}

Semaphore::~Semaphore()
{
	if (semaphore != nullptr)
	{
		vkDestroySemaphore(device.Handle(), semaphore, nullptr);
		semaphore = nullptr;
	}
}

}
