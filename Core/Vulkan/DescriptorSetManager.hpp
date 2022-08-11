#pragma once

#include "DescriptorBinding.hpp"
#include <memory>
#include <vector>

namespace Vulkan
{
	class Device;
	class DescriptorPool;
	class DescriptorSetLayout;
	class DescriptorSets;

	class DescriptorSetManager
	{
	public:
		VULKAN_OBJECT(DescriptorSetManager)

		explicit DescriptorSetManager(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings, size_t maxSets);
		~DescriptorSetManager();
		const class DescriptorSetLayout& get_descriptorSet_layout() const { return *descriptorSetLayout; }
		class DescriptorSets& get_descriptorSets() { return *descriptorSets; }

	private:

		std::unique_ptr<DescriptorPool> descriptorPool;
		std::unique_ptr<class DescriptorSetLayout> descriptorSetLayout;
		std::unique_ptr<class DescriptorSets> descriptorSets;
	};

}
