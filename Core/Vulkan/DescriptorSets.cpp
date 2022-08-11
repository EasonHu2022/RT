#include "DescriptorSets.hpp"
#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"
#include "Device.hpp"
#include <array>
#include <utility>

namespace Vulkan {

DescriptorSets::DescriptorSets(
	const DescriptorPool& _descriptorPool, 
	const DescriptorSetLayout& layout,
	std::map<uint32_t, VkDescriptorType> _bindingTypes,
	const size_t size) :
	descriptorPool(_descriptorPool),
	bindingTypes(std::move(_bindingTypes))
{
	std::vector<VkDescriptorSetLayout> layouts(size, layout.Handle());

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool.Handle();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(size);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(size);

	Check(vkAllocateDescriptorSets(descriptorPool.get_device().Handle(), &allocInfo, descriptorSets.data()),
		"allocate descriptor sets");
}

DescriptorSets::~DescriptorSets()
{
	//if (!descriptorSets_.empty())
	//{
	//	vkFreeDescriptorSets(
	//		descriptorPool_.Device().Handle(),
	//		descriptorPool_.Handle(),
	//		static_cast<uint32_t>(descriptorSets_.size()),
	//		descriptorSets_.data());

	//	descriptorSets_.clear();
	//}
}

VkWriteDescriptorSet DescriptorSets::Bind(const uint32_t index, const uint32_t binding, const VkDescriptorBufferInfo& bufferInfo, const uint32_t count) const
{
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSets[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = get_binding_type(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pBufferInfo = &bufferInfo;

	return descriptorWrite;
}

VkWriteDescriptorSet DescriptorSets::Bind(const uint32_t index, const uint32_t binding, const VkDescriptorImageInfo& imageInfo, const uint32_t count) const
{
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSets[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = get_binding_type(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pImageInfo = &imageInfo;
	return descriptorWrite;
}

VkWriteDescriptorSet DescriptorSets::Bind(uint32_t index, uint32_t binding, const VkWriteDescriptorSetAccelerationStructureKHR& structureInfo, const uint32_t count) const
{
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSets[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = get_binding_type(binding);
	descriptorWrite.descriptorCount = count;
	descriptorWrite.pNext = &structureInfo;

	return descriptorWrite;
}

void DescriptorSets::update_descriptors(uint32_t index, const std::vector<VkWriteDescriptorSet>& descriptorWrites)
{
	vkUpdateDescriptorSets(
		descriptorPool.get_device().Handle(),
		static_cast<uint32_t>(descriptorWrites.size()),
		descriptorWrites.data(), 0, nullptr);
}

VkDescriptorType DescriptorSets::get_binding_type(uint32_t binding) const
{
	const auto it = bindingTypes.find(binding);
	if (it == bindingTypes.end())
	{
		std::cerr << "binding not found" << std::endl;
	}
	return it->second;
}

}
