#pragma once

#include "../Vulkan/Wrapper.hpp"
#include <memory>
#include <vector>

#include "../Vulkan/Buffer.hpp"

namespace Vulkan
{
	class Buffer;
	class Device;
	class DeviceMemory;
}

namespace Vulkan::RayTracing
{
	class DeviceProcedures;
	class RayTracingPipeline;
	class RayTracingProperties;
	
	class ShaderBindingTable final
	{
	public:

		struct Entry
		{
			uint32_t GroupIndex;
			std::vector<unsigned char> InlineData;
		};

		VULKAN_OBJECT(ShaderBindingTable)

		ShaderBindingTable(
			const DeviceProcedures& deviceProcedures,
			const RayTracingPipeline& rayTracingPipeline,
			const RayTracingProperties& rayTracingProperties,
			const std::vector<Entry>& rayGenPrograms,
			const std::vector<Entry>& missPrograms,
			const std::vector<Entry>& hitGroups);

		~ShaderBindingTable();

		const class Buffer& Buffer() const { return *buffer_; }

		VkDeviceAddress RayGenDeviceAddress() const { return Buffer().get_device_address() + RayGenOffset(); }
		VkDeviceAddress MissDeviceAddress() const { return Buffer().get_device_address() + MissOffset(); }
		VkDeviceAddress HitGroupDeviceAddress() const { return Buffer().get_device_address() + HitGroupOffset(); }

		size_t RayGenOffset() const { return rayGenOffset_; }
		size_t MissOffset() const { return missOffset_; }
		size_t HitGroupOffset() const { return hitGroupOffset_; }

		size_t RayGenSize() const { return rayGenSize_; }
		size_t MissSize() const { return missSize_; }
		size_t HitGroupSize() const { return hitGroupSize_; }

		size_t RayGenEntrySize() const { return rayGenEntrySize_; }
		size_t MissEntrySize() const { return missEntrySize_; }
		size_t HitGroupEntrySize() const { return hitGroupEntrySize_; }

	private:

		const size_t rayGenEntrySize_;
		const size_t missEntrySize_;
		const size_t hitGroupEntrySize_;

		const size_t rayGenOffset_;
		const size_t missOffset_;
		const size_t hitGroupOffset_;

		const size_t rayGenSize_;
		const size_t missSize_;
		const size_t hitGroupSize_;

		std::unique_ptr<class Buffer> buffer_;
		std::unique_ptr<DeviceMemory> bufferMemory_;
	};

}
