#include "UniformBuffer.hpp"
#include "Buffer.hpp"
#include "Allocator.hpp"
#include <cstring>

namespace Assets {

UniformBuffer::UniformBuffer(const Vulkan::Device& device, Vulkan::Allocator const& allocator)
{
	const auto bufferSize = sizeof(UniformBufferObject);

	buffer_.reset(new Vulkan::Buffer(device, allocator, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY));
	//memory_.reset(new Vulkan::DeviceMemory(buffer_->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)));
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept :
	buffer_(other.buffer_.release())

{
}

UniformBuffer::~UniformBuffer()
{
	buffer_.reset();

}

void UniformBuffer::SetValue(const UniformBufferObject& ubo)
{
	/*const auto data = memory_->Map(0, sizeof(UniformBufferObject));
	std::memcpy(data, &ubo, sizeof(ubo));
	memory_->Unmap();*/
}

}
