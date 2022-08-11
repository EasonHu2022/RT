#pragma once

#include "Wrapper.hpp"
#include <memory>
#include <vector>

namespace Assets
{
	class Scene;
	class UniformBuffer;
}

namespace Vulkan
{
	class DepthBuffer;
	class PipelineLayout;
	class RenderPass;
	class SwapChain;

	class GraphicsPipeline 
	{
	public:

		VULKAN_OBJECT(GraphicsPipeline)

		GraphicsPipeline(
			const SwapChain& swapChain, 
			const DepthBuffer& depthBuffer,
			const std::vector<Assets::UniformBuffer>& uniformBuffers,
			const Assets::Scene& scene,
			bool isWireFrame);
		~GraphicsPipeline();

		VkDescriptorSet DescriptorSet(uint32_t index) const;
		bool IsWireFrame() const { return isWireFrame; }
		const class PipelineLayout& PipelineLayout() const { return *pipelineLayout; }
		const class RenderPass& RenderPass() const { return *renderPass; }

	private:

		const SwapChain& swapChain;
		const bool isWireFrame;

		VULKAN_HANDLES(VkPipeline, pipeline)

		std::unique_ptr<class DescriptorSetManager> descriptorSetManager;
		std::unique_ptr<class PipelineLayout> pipelineLayout;
		std::unique_ptr<class RenderPass> renderPass;
	};

}
