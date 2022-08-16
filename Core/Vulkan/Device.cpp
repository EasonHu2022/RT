#include "Device.hpp"
#include "Surface.hpp"
#include <algorithm>
#include <set>
#include "Surface.hpp"
namespace Vulkan 
{

	Device::Device(VkPhysicalDevice _physicalDevice, const Surface& _surface, const std::vector<const char*>& requiredExtensions, const VkPhysicalDeviceFeatures& deviceFeatures, const void* nextDeviceFeatures)
		: physicalDevice(_physicalDevice), surface(_surface)
	{

		const auto queueFamilies = get_vkEnumerate(physicalDevice,vkGetPhysicalDeviceQueueFamilyProperties);

		//find graphics queues
		const auto graphicsFamily = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& queueF)
			{
				return
					queueF.queueCount > 0 && queueF.queueFlags & VK_QUEUE_GRAPHICS_BIT && !(queueF.queueFlags & 0);
			});
		if (graphicsFamily == queueFamilies.end())
		{
			std::cerr << "found no matching -- graphics family -- queue" << std::endl;
		}

		const auto computeFamily = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& queueF)
			{
				return
					queueF.queueCount > 0 && queueF.queueFlags & VK_QUEUE_COMPUTE_BIT && !(queueF.queueFlags & VK_QUEUE_GRAPHICS_BIT);
			});
		if (computeFamily == queueFamilies.end())
		{
			std::cerr << "found no matching -- compute family -- queue" << std::endl;
		}

		const auto presentFamily = std::find_if(queueFamilies.begin(), queueFamilies.end(), [&](const VkQueueFamilyProperties& queueF) {
			VkBool32 presentSupport = false;
			const uint32_t t = static_cast<uint32_t>(&*queueFamilies.cbegin() - &queueF);
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,t,surface.Handle(), &presentSupport);
			return queueF.queueCount > 0 && presentSupport;
			});

		if (presentFamily == queueFamilies.end())
		{
			std::cerr << "found no present queue" << std::endl;
		}

		graphicsFamilyIndex = static_cast<uint32_t>(graphicsFamily - queueFamilies.begin());
		computeFamilyIndex = static_cast<uint32_t>(computeFamily - queueFamilies.begin());
		presentFamilyIndex = static_cast<uint32_t>(presentFamily - queueFamilies.begin());

		const std::set<uint32_t> uniqueQueueFamilies =
		{
			graphicsFamilyIndex,computeFamilyIndex,presentFamilyIndex,
		};

		float priority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &priority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		//create device
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = nextDeviceFeatures;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledLayerCount = static_cast<uint32_t>(surface.get_instance().get_validationLayers().size());
		createInfo.ppEnabledLayerNames = surface.get_instance().get_validationLayers().data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();
		Check(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device),
			"create logical device");
		//set
		vkGetDeviceQueue(device, graphicsFamilyIndex, 0, &graphicsQueue);
		vkGetDeviceQueue(device, computeFamilyIndex, 0, &computeQueue);
		vkGetDeviceQueue(device, presentFamilyIndex, 0, &presentQueue);
	}
	Device::~Device()
	{
		if (device != nullptr)
		{
			vkDestroyDevice(device, nullptr);
			device = nullptr;
		}
	}
	void Device::WaitIdle() const
	{
		Check(vkDeviceWaitIdle(device),
			"wait for device idle");
	}
	void Device::check_required_extensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) const
	{
		const auto availableExtensions = get_vkEnumerate(physicalDevice, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties);
		std::set<std::string> required(requiredExtensions.begin(), requiredExtensions.end());
		for (const auto& extension : availableExtensions)
		{
			required.erase(extension.extensionName);
		}
		if (!required.empty())
		{
			bool first = true;
			std::string extensions;
			for (const auto& extension : required)
			{
				if (!first)
				{
					extensions += ", ";
				}

				extensions += extension;
				first = false;
			}
			std::cerr << "missing required extensions: " + extensions << std::endl;
		}
	}
};