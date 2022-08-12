#pragma once

#include "Wrapper.hpp"

namespace Vulkan
{
	class DescriptorSetLayout;
	class Device;

	class PipelineLayout
	{
	public:

		VULKAN_OBJECT(PipelineLayout)

		PipelineLayout(const Device& device, const DescriptorSetLayout& descriptorSetLayout);
		~PipelineLayout();

	private:

		const Device& device;

		VULKAN_HANDLES(VkPipelineLayout, pipelineLayout)
	};

}
