#pragma once

#include "Wrapper.hpp"
#include <map>
#include <vector>

namespace Vulkan
{
	class Buffer;
	class DescriptorPool;
	class DescriptorSetLayout;
	class ImageView;

	class DescriptorSets
	{
	public:

		VULKAN_OBJECT(DescriptorSets)

		DescriptorSets(
			const DescriptorPool& descriptorPool, 
			const DescriptorSetLayout& layout,
		    std::map<uint32_t, VkDescriptorType> bindingTypes, 
			size_t size);

		~DescriptorSets();

		VkDescriptorSet Handle(uint32_t index) const { return descriptorSets[index]; }

		VkWriteDescriptorSet Bind(uint32_t index, uint32_t binding, const VkDescriptorBufferInfo& bufferInfo, uint32_t count = 1) const;
		VkWriteDescriptorSet Bind(uint32_t index, uint32_t binding, const VkDescriptorImageInfo& imageInfo, uint32_t count = 1) const;
		VkWriteDescriptorSet Bind(uint32_t index, uint32_t binding, const VkWriteDescriptorSetAccelerationStructureKHR& structureInfo, uint32_t count = 1) const;
		void update_descriptors(uint32_t index, const std::vector<VkWriteDescriptorSet>& descriptorWrites);

	private:

		VkDescriptorType get_binding_type(uint32_t binding) const;

		const DescriptorPool& descriptorPool;
		const std::map<uint32_t, VkDescriptorType> bindingTypes;
		
		std::vector<VkDescriptorSet> descriptorSets;
	};

}