#include "Allocator.hpp"
#include "Device.hpp"
#include "Instance.hpp"
namespace Vulkan
{
	

	Allocator::~Allocator()
	{
		if (VK_NULL_HANDLE != allocator)
		{
			vmaDestroyAllocator(allocator);
		}
	}

	Allocator::Allocator(VmaAllocator vma, Device device, Instance instance)
	{
		VkPhysicalDeviceProperties props{};
		vkGetPhysicalDeviceProperties(device.get_physicalDevice(), &props);

		VmaVulkanFunctions functions{};
		functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocInfo{};
		allocInfo.vulkanApiVersion = props.apiVersion;
		allocInfo.physicalDevice = device.get_physicalDevice();
		allocInfo.device = device.Handle();
		allocInfo.instance = instance.Handle();
		allocInfo.pVulkanFunctions = &functions;

		if (auto const res = vmaCreateAllocator(&allocInfo, &allocator); VK_SUCCESS != res)
		{

			std::cerr << "Unable to create allocator\n vmaCreateAllocator() returned false " << std::endl;
		}
	}


	Allocator::Allocator(Allocator && aOther) noexcept
		: allocator(std::exchange(aOther.allocator, VK_NULL_HANDLE))
	{}
	Allocator& Allocator::operator=(Allocator && aOther) noexcept
	{
		std::swap(allocator, aOther.allocator);
		return *this;
	}
};