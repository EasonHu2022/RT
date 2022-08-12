#include "Image.hpp"
#include "Device.hpp"
#include "SingleTimeCommands.hpp"
#include "DepthBuffer.hpp"
#include "Buffer.hpp"
namespace Vulkan {

	Image::Image(const Device& _device, Allocator const& _allocator, 
		VkExtent2D _extent, VkFormat _format, VkImageTiling tiling, 
		VkImageUsageFlags usage)
		:allocator(_allocator),device(_device),extent(_extent),format(_format)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = extent.width;
		imageInfo.extent.height = extent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = imageLayout;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	
		Check(vmaCreateImage(allocator.Handle(), &imageInfo, &allocInfo, &image, &allocation, nullptr),
			"create image");
	}

	Image::~Image()
	{
		if (image != nullptr)
		{
			vkDestroyImage(device.Handle(), image, nullptr);
			image = nullptr;
		}
	}

	Image::Image(Image&& other) noexcept :
		device(other.device),
		extent(other.extent),
		format(other.format),
		imageLayout(other.imageLayout),
		image(other.image),
		allocator(other.allocator)
	{
		other.image = nullptr;
	}

	void Image::trans_image_layout(CommandPool& commandPool, VkImageLayout newLayout)
	{
		SingleTimeCommands::Submit(commandPool, [&](VkCommandBuffer commandBuffer)
			{
				VkImageMemoryBarrier barrier = {};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.oldLayout = imageLayout;
				barrier.newLayout = newLayout;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.image = image;
				barrier.subresourceRange.baseMipLevel = 0;
				barrier.subresourceRange.levelCount = 1;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.layerCount = 1;

				if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
				{
					barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

					if (DepthBuffer::bHas_stencil_component(format))
					{
						barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
					}
				}
				else
				{
					barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				}

				VkPipelineStageFlags sourceStage;
				VkPipelineStageFlags destinationStage;

				if (imageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
				{
					barrier.srcAccessMask = 0;
					barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

					sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				}
				else if (imageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
				{
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

					sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
					destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				}
				else if (imageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
				{
					barrier.srcAccessMask = 0;
					barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

					sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				}
				else
				{
					std::cerr << "unsupported layout transition" << std::endl;
					
				}

				vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
			});

		imageLayout = newLayout;
	}

	void Image::Copy(CommandPool& commandPool, const Buffer& buffer)
	{

		SingleTimeCommands::Submit(commandPool, [&](VkCommandBuffer commandBuffer)
			{
				VkBufferImageCopy region = {};
				region.bufferOffset = 0;
				region.bufferRowLength = 0;
				region.bufferImageHeight = 0;
				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.mipLevel = 0;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = 1;
				region.imageOffset = { 0, 0, 0 };
				region.imageExtent = { extent.width, extent.height, 1 };

				vkCmdCopyBufferToImage(commandBuffer, buffer.Handle(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
			});
	}

};