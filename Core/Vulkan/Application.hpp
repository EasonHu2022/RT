#pragma once
#include "Wrapper.hpp"
#include "Utils.hpp"
#include  <vector>
#include "Window.hpp"
#include <memory>
#include "SwapChain.hpp"
#include "UniformBuffer.hpp"
#include "Fence.hpp"
#include "Semaphore.hpp"
#include "CommandPool.hpp"
#include "CommandBuffers.hpp"
#include "FrameBuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "DepthBuffer.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "Device.hpp"
#include "UniformBuffer.hpp"
namespace Vulkan
{
	class Application
	{
	public:
		VULKAN_OBJECT(Application)

		virtual ~Application();

		const std::vector<VkExtensionProperties>& get_extensions() const;
		const std::vector<VkLayerProperties>& get_layers() const;
		const std::vector<VkPhysicalDevice>& get_physicalDevices() const;
		const class SwapChain& get_swapChain() const { return *swapChain; }

		class  Window& get_window() const  { return *window; }
		Application(const WindowConfig& windowConfig, VkPresentModeKHR presentMode, bool enableValidationLayers);

		void set_physical_device(VkPhysicalDevice physicalDevice);
		
		bool HasSwapChain() const { return swapChain.operator bool(); }

		void Start();
	protected:
		virtual void set_physical_device(
			VkPhysicalDevice physicalDevice,
			std::vector<const char*>& requiredExtensions,
			VkPhysicalDeviceFeatures& deviceFeatures,
			void* nextDeviceFeatures);

		const class Device& get_device() const { return *device; }
		class CommandPool& get_commandPool() { return *commandPool; }
		const class DepthBuffer& get_depthBuffer() const { return *depthBuffer; }
		const std::vector<Assets::UniformBuffer>& get_uniformBuffer() const { return uniformBuffers; }
		const class GraphicsPipeline& get_graphicsPipline () const { return *graphicsPipeline; }
		const class FrameBuffer& get_swapChainFrameBuffer(const size_t i) const { return swapChainFramebuffers[i]; }
		virtual const Assets::Scene& get_scene() const = 0;
		virtual Assets::UniformBufferObject get_uniformBufferObjects(VkExtent2D extent) const = 0;

		virtual void on_device_set();
		virtual void create_swapChain();
		virtual void delete_swapChain();
		virtual void draw_frame();
		virtual void render(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		virtual void on_key(int key, int scancode, int action, int mods) { }
		virtual void on_cursorPosition(double xpos, double ypos) { }
		virtual void on_mouseButton(int button, int action, int mods) { }
		virtual void on_scroll(double xoffset, double yoffset) { }

		bool isWireFrame_{};
	private:

		void update_uniformBuffer(uint32_t imageIndex);
		void recreate_swapChain();

		const VkPresentModeKHR presentMode;
		std::unique_ptr<class Window> window;
		std::unique_ptr<class Instance> instance;
		//std::unique_ptr<class DebugUtilsMessenger> debugUtilsMessenger;
		std::unique_ptr<class Surface> surface;
		std::unique_ptr<class Device> device;
		std::unique_ptr<class SwapChain> swapChain;
		std::vector<Assets::UniformBuffer> uniformBuffers;
		std::unique_ptr<class DepthBuffer> depthBuffer;
		std::unique_ptr<class GraphicsPipeline> graphicsPipeline;
		std::vector<class FrameBuffer> swapChainFramebuffers;
		std::unique_ptr<class CommandPool> commandPool;
		std::unique_ptr<class CommandBuffers> commandBuffers;
		std::vector<class Semaphore> imageAvailableSemaphores;
		std::vector<class Semaphore> renderFinishedSemaphores;
		std::vector<class Fence> inFlightFences;

		size_t currentFrame{};
	};

}

