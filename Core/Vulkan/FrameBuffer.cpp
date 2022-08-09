#include "FrameBuffer.hpp"

#include "Device.hpp"
#include "ImageView.hpp"
#include "RenderPass.hpp"
#include "SwapChain.hpp"
#include "DepthBuffer.hpp"
#include <array>

namespace Vulkan {

FrameBuffer::FrameBuffer(const class ImageView& _imageView, const class RenderPass& _renderPass) :
	imageView(_imageView),
	renderPass(_renderPass)
{
	std::array<VkImageView, 2> attachments =
	{
		imageView.Handle(),
		renderPass.get_depthBuffer().get_imageView().Handle()
	};

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass.Handle();
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = renderPass.get_swapChain().get_extent().width;
	framebufferInfo.height = renderPass.get_swapChain().get_extent().height;
	framebufferInfo.layers = 1;

	Check(vkCreateFramebuffer(imageView.get_device().Handle(), &framebufferInfo, nullptr, &framebuffer),
		"create framebuffer");
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept :
	imageView(other.imageView),
	renderPass(other.renderPass),
	framebuffer(other.framebuffer)
{
	other.framebuffer = nullptr;
}

FrameBuffer::~FrameBuffer()
{
	if (framebuffer != nullptr)
	{
		vkDestroyFramebuffer(imageView.get_device().Handle(), framebuffer, nullptr);
		framebuffer = nullptr;
	}
}

}
