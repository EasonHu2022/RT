#pragma once

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

		Image(const Device& device, VkExtent2D extent, VkFormat format);
		Image(const Device& device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
		Image(Image&& other) noexcept;
		~Image();
	
	};

}

