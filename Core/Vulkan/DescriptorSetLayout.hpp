#pragma once

#include "DescriptorBinding.hpp"
#include <vector>
namespace Vulkan
{
	class Device;
	class DescriptorSetLayout final
	{
	public:
		VULKAN_OBJECT(DescriptorSetLayout)
		DescriptorSetLayout(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings);
		~DescriptorSetLayout();
	private:
		const Device& device;
		VULKAN_HANDLES(VkDescriptorSetLayout, layout)
	};

}
