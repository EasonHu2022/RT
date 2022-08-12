#include "DepthBuffer.hpp"
#include "Utils.hpp"
#include "CommandPool.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
namespace Vulkan
{
	//********************************************************************************************//
	inline VkFormat FindSupportedFormat(const Device& device, const std::vector<VkFormat>& candidates, const VkImageTiling tiling, const VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(device.get_physicalDevice(), format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}

			if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}
		std::cerr << "failed to find supported format" << std::endl;
		return VK_FORMAT_UNDEFINED;
	}


	DepthBuffer::DepthBuffer(Allocator const& allocator, CommandPool& commandPool, VkExtent2D extent) : format(FindSupportedFormat(
		commandPool.get_device(),
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	))
	{
		const auto& device = commandPool.get_device();
		image.reset(new Image(device, allocator, extent, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
		imageView.reset(new class ImageView(device, image->Handle(), format, VK_IMAGE_ASPECT_DEPTH_BIT));
	}
	DepthBuffer::~DepthBuffer()
	{
	}
};