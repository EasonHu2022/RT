#pragma once

#include "Wrapper.hpp"

namespace Vulkan
{
	class ImageView;
	class RenderPass;

	class FrameBuffer 
	{
	public:

		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer& operator = (const FrameBuffer&) = delete;
		FrameBuffer& operator = (FrameBuffer&&) = delete;

		explicit FrameBuffer(const ImageView& imageView, const RenderPass& renderPass);
		FrameBuffer(FrameBuffer&& other) noexcept;
		~FrameBuffer();

		const class ImageView& ImageView() const { return imageView; }
		const class RenderPass& RenderPass() const { return renderPass; }

	private:

		const class ImageView& imageView;
		const class RenderPass& renderPass;

		VULKAN_HANDLES(VkFramebuffer, framebuffer)
	};

}
