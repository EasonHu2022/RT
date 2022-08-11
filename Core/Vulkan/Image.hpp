#pragma once
#include "Wrapper.hpp"
#include "Allocator.hpp"
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
		Image(const Image&) = delete;
		Image& operator = (const Image&) = delete;
		Image& operator = (Image&&) = delete;

		//Image(const Device& device, VkExtent2D extent, VkFormat format);
		Image(const Device& device, Allocator const& allocator, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage aMemoryUsage);
		Image(Image&& other) noexcept;
		~Image();
		const class Device& get_device() const { return device; }
		VkExtent2D get_extent() const { return extent; }
		VkFormat get_format() const { return format; }
		const class Allocator& get_allocator() const { return allocator; }
		void trans_image_layout(CommandPool& commandPool, VkImageLayout newLayout);
		void Copy(CommandPool& commandPool, const Buffer& buffer);
		VmaAllocation allocation = VK_NULL_HANDLE;
	private:

		const class Device& device;
		const class Allocator& allocator;
		const VkExtent2D extent;
		const VkFormat format;
		VkImageLayout imageLayout;

		

		VULKAN_HANDLES(VkImage, image)
	};

}

