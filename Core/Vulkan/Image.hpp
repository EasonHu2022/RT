#pragma once
#include "Wrapper.hpp"
#include "DeviceMemory.hpp"
namespace Vulkan
{

	class Buffer;
	class CommandPool;
	class Device;
	/// <summary>
	/// thought from rt weekend and rt in vulkan
	/// </summary>
	class Image
	{
	public:
		Image(const Image&) = delete;
		Image& operator = (const Image&) = delete;
		Image& operator = (Image&&) = delete;

		//Image(const Device& device, VkExtent2D extent, VkFormat format);
		Image(const Device& device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
		Image(Image&& other) noexcept;
		~Image();
		const class Device& get_device() const { return device; }
		VkExtent2D get_extent() const { return extent; }
		VkFormat get_format() const { return format; }

		DeviceMemory allocate_memory(VkMemoryPropertyFlags properties) const;
		VkMemoryRequirements get_memory_requirements() const;

		void trans_image_layout(CommandPool& commandPool, VkImageLayout newLayout);
		void copy(CommandPool& commandPool, const Buffer& buffer);
		
	private:
		VULKAN_HANDLES(VkImage, image)






		const class Device& device;
		const VkExtent2D extent;
		const VkFormat format;
		VkImageLayout imageLayout;

		

		
	};

}

