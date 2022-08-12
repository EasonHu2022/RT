#include "PipelineLayout.hpp"
#include "DescriptorSetLayout.hpp"
#include "Device.hpp"

namespace Vulkan {

PipelineLayout::PipelineLayout(const Device & _device, const DescriptorSetLayout& descriptorSetLayout) :
	device(_device)
{
	VkDescriptorSetLayout descriptorSetLayouts[] = { descriptorSetLayout.Handle() };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	Check(vkCreatePipelineLayout(device.Handle(), &pipelineLayoutInfo, nullptr, &pipelineLayout),
		"create pipeline layout");
}

PipelineLayout::~PipelineLayout()
{
	if (pipelineLayout != nullptr)
	{
		vkDestroyPipelineLayout(device.Handle(), pipelineLayout, nullptr);
		pipelineLayout = nullptr;
	}
}

}
