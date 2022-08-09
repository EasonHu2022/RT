#include "SwapChain.hpp"
#include "Device.hpp"
namespace Vulkan
{
	SwapChain::SwapChain(const Device& _device, VkPresentModeKHR presentMode):device(_device),physicalDevice(_device.get_physicalDevice())
	{
	}
	SwapChain::~SwapChain()
	{
	}
};