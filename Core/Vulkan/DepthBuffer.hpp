#pragma once

#include "Wrapper.hpp"

namespace Vulkan
{
	class CommandPool;
	class ImageView;
	class Image;
	class DepthBuffer 
	{
	public:
		VULKAN_OBJECT(DepthBuffer)
		DepthBuffer(CommandPool& commandPool, VkExtent2D extent);
		~DepthBuffer();
		VkFormat get_format() const { return format; }
		const class ImageView& get_imageView() const { return *imageView; }
		static bool bHas_stencil_component(const VkFormat format)
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}
	private:
		const VkFormat format;
		std::unique_ptr<class ImageView> imageView;
		std::unique_ptr<Image> image;
	};

}
