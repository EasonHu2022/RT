#pragma once

#include "DescriptorBinding.hpp"
#include <vector>

namespace Vulkan
{
	class Device;

	class DescriptorPool final
	{
	public:
		VULKAN_OBJECT(DescriptorPool)
		DescriptorPool(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings, size_t maxSets);
		~DescriptorPool();
		const class Device& get_device() const { return device; }
	private:
		const class Device& device;
		VULKAN_HANDLES(VkDescriptorPool, descriptorPool)
	};

}
