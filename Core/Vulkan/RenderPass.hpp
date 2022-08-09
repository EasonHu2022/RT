#pragma once

#include "Wrapper.hpp"

namespace Vulkan
{
	class DepthBuffer;
	class SwapChain;

	class RenderPass 
	{
	public:

		VULKAN_OBJECT(RenderPass)
		RenderPass(const SwapChain& swapChain, const DepthBuffer& depthBuffer,
				VkAttachmentLoadOp colorBufferLoadOp, VkAttachmentLoadOp depthBufferLoadOp);
		~RenderPass();
		const class SwapChain& get_swapChain() const { return swapChain; }
		const class DepthBuffer& get_depthBuffer() const { return depthBuffer; }

	private:

		const class SwapChain& swapChain;
		const class DepthBuffer& depthBuffer;

		VULKAN_HANDLES(VkRenderPass, renderPass)
	};

}
