#pragma once

#include "glm/glm.hpp"
#include <memory>

namespace Vulkan
{
	class Buffer;
	class Device;
	class DeviceMemory;
	class Allocator;
}

namespace Assets
{
	class UniformBufferObject
	{
	public:

		glm::mat4 ModelView;
		glm::mat4 Projection;
		glm::mat4 ModelViewInverse;
		glm::mat4 ProjectionInverse;
		float Aperture;
		float FocusDistance;
		float HeatmapScale;
		uint32_t TotalNumberOfSamples;
		uint32_t NumberOfSamples;
		uint32_t NumberOfBounces;
		uint32_t RandomSeed;
		uint32_t HasSky; // bool
		uint32_t ShowHeatmap; // bool
	};

	class UniformBuffer
	{
	public:

		UniformBuffer(const UniformBuffer&) = delete;
		UniformBuffer& operator = (const UniformBuffer&) = delete;
		UniformBuffer& operator = (UniformBuffer&&) = delete;

		explicit UniformBuffer(const Vulkan::Device& device, Vulkan::Allocator const& allocator);
		UniformBuffer(UniformBuffer&& other) noexcept;
		~UniformBuffer();

		const Vulkan::Buffer& Buffer() const { return *buffer_; }

		void SetValue(const UniformBufferObject& ubo);

	private:

		std::unique_ptr<Vulkan::Buffer> buffer_;
		
	};

}