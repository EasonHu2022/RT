#include "DepthBuffer.hpp"
#include "Utils.hpp"
#include "CommandPool.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
namespace Vulkan
{
	DepthBuffer::DepthBuffer(CommandPool& commandPool, VkExtent2D extent) : format(FindSupportedFormat(
		commandPool.get_device(),
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	))
	{
		const auto& device = commandPool.get_device();
		image.reset(new Image(device, extent, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
		imageView.reset(new class ImageView(device, image->Handle(), format, VK_IMAGE_ASPECT_DEPTH_BIT));
	}
	DepthBuffer::~DepthBuffer()
	{
	}
};