#include "DescriptorSetLayout.hpp"
#include "Device.hpp"

namespace Vulkan {

DescriptorSetLayout::DescriptorSetLayout(const Device& _device, const std::vector<DescriptorBinding>& descriptorBindings) :
	device(_device)
{
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	for (const auto& binding : descriptorBindings)
	{
		VkDescriptorSetLayoutBinding b = {};
		b.binding = binding.Binding;
		b.descriptorCount = binding.DescriptorCount;
		b.descriptorType = binding.Type;
		b.stageFlags = binding.Stage;
		layoutBindings.push_back(b);
	}
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	layoutInfo.pBindings = layoutBindings.data();
	Check(vkCreateDescriptorSetLayout(device.Handle(), &layoutInfo, nullptr, &layout),
		"create descriptor set layout");
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	if (layout != nullptr)
	{
		vkDestroyDescriptorSetLayout(device.Handle(), layout, nullptr);
		layout = nullptr;
	}
}

}
