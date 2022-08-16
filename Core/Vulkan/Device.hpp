#pragma once
#include "Wrapper.hpp"
#include "Utils.hpp"
#include <vector>
namespace Vulkan
{
	class Surface;
	class Device
	{
	public:
		VULKAN_OBJECT(Device)
		Device(VkPhysicalDevice physicalDevice,
			const Surface& surface,
			const std::vector<const char*>& requiredExtensions,
			const VkPhysicalDeviceFeatures& deviceFeatures, 
			const void* nextDeviceFeatures);
		~Device();
		VkPhysicalDevice get_physicalDevice() const { return physicalDevice; }
		const class Surface& get_surface() const { return surface; }
		uint32_t get_graphicsFamiltyInde() const { return graphicsFamilyIndex; }
		uint32_t get_computeFamilyIndex() const { return computeFamilyIndex; }
		uint32_t get_presentFamilyIndex() const { return presentFamilyIndex; }
		VkQueue get_graphicsQueue() const { return graphicsQueue; }
		VkQueue get_computeQueue() const { return computeQueue; }
		VkQueue get_presentQueue() const { return presentQueue; }
		void WaitIdle() const;
	private:
		void check_required_extensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) const;

		VkPhysicalDevice physicalDevice;
		const class Surface& surface;

		uint32_t graphicsFamilyIndex;
		uint32_t computeFamilyIndex;
		uint32_t presentFamilyIndex;
		VkQueue graphicsQueue;
		VkQueue computeQueue;
		VkQueue presentQueue;

		VULKAN_HANDLES(VkDevice,device)
	};

}

