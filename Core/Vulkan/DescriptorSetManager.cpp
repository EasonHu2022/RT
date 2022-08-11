#include "DescriptorSetManager.hpp"
#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"
#include "DescriptorSets.hpp"
#include "Device.hpp"
#include <set>

namespace Vulkan {

DescriptorSetManager::DescriptorSetManager(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings, const size_t maxSets)
{
	// Sanity check to avoid binding different resources to the same binding point.
	std::map<uint32_t, VkDescriptorType> bindingTypes;

	for (const auto& binding : descriptorBindings)
	{
		if (!bindingTypes.insert(std::make_pair(binding.Binding, binding.Type)).second)
		{
			std::cerr << "binding collision" << std::endl;
		}
	}

	descriptorPool.reset(new DescriptorPool(device, descriptorBindings, maxSets));
	descriptorSetLayout.reset(new class DescriptorSetLayout(device, descriptorBindings));
	descriptorSets.reset(new class DescriptorSets(*descriptorPool, *descriptorSetLayout, bindingTypes, maxSets));
}

DescriptorSetManager::~DescriptorSetManager()
{
	descriptorSets.reset();
	descriptorSetLayout.reset();
	descriptorPool.reset();
}

}
