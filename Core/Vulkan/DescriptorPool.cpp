#include "DescriptorPool.hpp"
#include "Device.hpp"

namespace Vulkan {

DescriptorPool::DescriptorPool(const Vulkan::Device& _device, const std::vector<DescriptorBinding>& descriptorBindings, const size_t maxSets) :
	device(_device)
{
	std::vector<VkDescriptorPoolSize> poolSizes;
	for (const auto& binding : descriptorBindings)
	{
		poolSizes.push_back(VkDescriptorPoolSize{ binding.Type, static_cast<uint32_t>(binding.DescriptorCount*maxSets )});
	}
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(maxSets);
	Check(vkCreateDescriptorPool(device.Handle(), &poolInfo, nullptr, &descriptorPool),
		"create descriptor pool");
}

DescriptorPool::~DescriptorPool()
{
	if (descriptorPool != nullptr)
	{
		vkDestroyDescriptorPool(device.Handle(), descriptorPool, nullptr);
		descriptorPool = nullptr;
	}
}

}
