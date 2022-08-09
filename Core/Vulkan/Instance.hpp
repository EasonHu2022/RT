#pragma once
#include "Wrapper.hpp"
#include <vector>

namespace Vulkan
{
	class Window;
	class Instance
	{
	public:
		VULKAN_OBJECT(Instance)
		Instance(const Window& window, const std::vector<const char*>& validationLayers, uint32_t vulkanVersion);
		~Instance();

		const class Window& get_window() const { return window; };	
		const std::vector<VkExtensionProperties>& get_extensions() const { return extensions; }
		const std::vector<VkLayerProperties>& get_layers() const { return layers; }
		const std::vector<VkPhysicalDevice>& get_physicalDivces() const { return physicalDevices; }
		const std::vector<const char*>& get_validationLayers() const { return validationLayers; }


	//handle
		VULKAN_HANDLES(VkInstance, instance)
	private:
		const Window& window;
		std::vector<VkExtensionProperties> extensions;
		std::vector<VkLayerProperties> layers;
		std::vector<VkPhysicalDevice> physicalDevices;
		const std::vector<const char*> validationLayers;
	};

}

