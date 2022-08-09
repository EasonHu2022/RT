#include "ImageView.hpp"
namespace Vulkan {
	ImageView::ImageView(const Device& _device, VkImage _image, VkFormat _format, VkImageAspectFlags aspectFlags)
		:device(_device), image(_image), format(_format)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = aspectFlags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		Check(vkCreateImageView(device.Handle(),&createInfo,nullptr, &imageView),"create image view");
	}
	ImageView::~ImageView()
	{
		if (imageView != nullptr)
		{
			vkDestroyImageView(device.Handle(),imageView,nullptr);
			imageView = nullptr;
		}
	}
}
