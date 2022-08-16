#include "Application.hpp"
#include <array>
#include "RenderPass.hpp"
#include "Scene.hpp"
#include "Buffer.hpp"
#include "PipelineLayout.hpp"
#include "Model.hpp"
namespace Vulkan
{
	Application::~Application()
	{
		//Application::DeleteSwapChain();

		commandPool.reset();
		device.reset();
		surface.reset();
		//debugUtilsMessenger.reset();
		instance.reset();
		window.reset();
	}
	const std::vector<VkExtensionProperties>& Application::get_extensions() const
	{
		return instance->get_extensions();
	}
	const std::vector<VkLayerProperties>& Application::get_layers() const
	{
		return instance->get_layers();
	}
	const std::vector<VkPhysicalDevice>& Application::get_physicalDevices() const
	{
		return instance->get_physicalDivces();
	}
	Application::Application(const WindowConfig& windowConfig, VkPresentModeKHR _presentMode, bool enableValidationLayers): presentMode(_presentMode)
	{
		const auto validationLayers = enableValidationLayers ? std::vector<const char*>{"VK_LAYER_KHRONOS_validation"} : std::vector<const char*>();
		window.reset(new class Window(windowConfig));
		instance.reset(new Instance(*window,validationLayers,VK_API_VERSION_1_2));
		surface.reset(new Surface(*instance));
	}
	void Application::set_physical_device(VkPhysicalDevice physicalDevice)
	{
		if (device)
		{
			std::cerr << "physical device has already been set" << std::endl;
			
		}

		std::vector<const char*> requiredExtensions =
		{
			// VK_KHR_swapchain
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkPhysicalDeviceFeatures deviceFeatures = {};

		set_physical_device(physicalDevice, requiredExtensions, deviceFeatures, nullptr);
		on_device_set();

		// Create swap chain and command buffers.
		create_swapChain();
	}
	void Application::Start()
	{
		window->Start();
	}

	void Application::set_physical_device(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions, VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures)
	{
		device.reset(new class Device(physicalDevice, *surface, requiredExtensions, deviceFeatures, nextDeviceFeatures));
		commandPool.reset(new class CommandPool(*device, device->get_graphicsFamiltyInde(), true));
	}

	void Application::update_uniformBuffer(uint32_t imageIndex)
	{
		uniformBuffers[imageIndex].SetValue(get_uniformBufferObjects(swapChain->get_extent()));
	}

	void Application::recreate_swapChain()
	{
		device->WaitIdle();
		delete_swapChain();
		create_swapChain();
	}

	void Application::on_device_set()
	{
	}

	void Application::create_swapChain()
	{

		// Wait until the window is visible.
		while (window->IsMinimized())
		{
			window->WaitForEvents();
		}

		swapChain.reset(new class SwapChain(*device, presentMode));
		depthBuffer.reset(new class DepthBuffer(*commandPool, swapChain->get_extent()));

		for (size_t i = 0; i != swapChain->get_imageViews().size(); ++i)
		{
			imageAvailableSemaphores.emplace_back(*device);
			renderFinishedSemaphores.emplace_back(*device);
			inFlightFences.emplace_back(*device, true);
			uniformBuffers.emplace_back(*device);
		}

		graphicsPipeline.reset(new class GraphicsPipeline(*swapChain, *depthBuffer, uniformBuffers, get_scene(), isWireFrame_));

		for (const auto& imageView : swapChain->get_imageViews())
		{
			swapChainFramebuffers.emplace_back(*imageView, graphicsPipeline->RenderPass());
		}

		commandBuffers.reset(new CommandBuffers(*commandPool, static_cast<uint32_t>(swapChainFramebuffers.size())));
	}

	void Application::delete_swapChain()
	{
		commandBuffers.reset();
		swapChainFramebuffers.clear();
		graphicsPipeline.reset();
		uniformBuffers.clear();
		inFlightFences.clear();
		renderFinishedSemaphores.clear();
		imageAvailableSemaphores.clear();
		depthBuffer.reset();
		swapChain.reset();
	}

	void Application::draw_frame()
	{
		const auto noTimeout = std::numeric_limits<uint64_t>::max();

		auto& inFlightFence = inFlightFences[currentFrame];
		const auto imageAvailableSemaphore = imageAvailableSemaphores[currentFrame].Handle();
		const auto renderFinishedSemaphore = renderFinishedSemaphores[currentFrame].Handle();

		inFlightFence.Wait(noTimeout);

		uint32_t imageIndex;
		auto result = vkAcquireNextImageKHR(device->Handle(), swapChain->Handle(), noTimeout, imageAvailableSemaphore, nullptr, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || isWireFrame_ != graphicsPipeline->IsWireFrame())
		{
			recreate_swapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			std::cerr << "failed to acquire next image (" + std::string(ToString(result)) + ")" << std::endl;
			
		}

		const auto commandBuffer = commandBuffers->Begin(imageIndex);
		render(commandBuffer, imageIndex);
		commandBuffers->End(imageIndex);

		update_uniformBuffer(imageIndex);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkCommandBuffer commandBuffers[]{ commandBuffer };
		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		inFlightFence.Reset();

		Check(vkQueueSubmit(device->get_graphicsQueue(), 1, &submitInfo, inFlightFence.Handle()),
			"submit draw command buffer");

		VkSwapchainKHR swapChains[] = { swapChain->Handle() };
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(device->get_presentQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			recreate_swapChain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			std::cerr << std::string("failed to present next image (") + ToString(result) + ")" << std::endl;
			
		}

		currentFrame = (currentFrame + 1) % inFlightFences.size();
	}

	void Application::render(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = graphicsPipeline->RenderPass().Handle();
		renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex].Handle();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->get_extent();
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			const auto& scene = get_scene();

			VkDescriptorSet descriptorSets[] = { graphicsPipeline->DescriptorSet(imageIndex) };
			VkBuffer vertexBuffers[] = { scene.VertexBuffer().Handle() };
			const VkBuffer indexBuffer = scene.IndexBuffer().Handle();
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->Handle());
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->PipelineLayout().Handle(), 0, 1, descriptorSets, 0, nullptr);
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

			uint32_t vertexOffset = 0;
			uint32_t indexOffset = 0;

			for (const auto& model : scene.Models())
			{
				const auto vertexCount = static_cast<uint32_t>(model.NumberOfVertices());
				const auto indexCount = static_cast<uint32_t>(model.NumberOfIndices());

				vkCmdDrawIndexed(commandBuffer, indexCount, 1, indexOffset, vertexOffset, 0);

				vertexOffset += vertexCount;
				indexOffset += indexCount;
			}
		}
		vkCmdEndRenderPass(commandBuffer);
	}

}