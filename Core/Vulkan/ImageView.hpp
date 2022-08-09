#pragma once
#include "Wrapper.hpp"
#include "Utils.hpp"
#include "Device.hpp"
namespace Vulkan
{
	class ImageView
	{
		
	public :
		VULKAN_OBJECT(ImageView)
		ImageView(const Device& device, VkImage image,VkFormat format, VkImageAspectFlags aspectFlags);
		~ImageView();
		const class Device& get_device() const { return device; }

	private:
		const class Device& device;
		const VkImage image;
		const VkFormat format;
		VULKAN_HANDLES(VkImageView,imageView)
	};

}

