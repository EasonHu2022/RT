#pragma once
#include "Wrapper.hpp"
#include "Utils.hpp"
#include  <vector>
#include "Window.hpp"
#include <memory>
#include "SwapChain.hpp"
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

		class Window& get_window() { return *window; }
		Application(const WindowConfig& windowConfig, VkPresentModeKHR presentMode, bool enableValidationLayers);

		void set_physical_device(VkPhysicalDevice physicalDevice);
		


		void Start();
	private:
		std::unique_ptr<class Window> window;
		std::unique_ptr<class SwapChain> swapChain;
		const VkPresentModeKHR presentMode;
		std::unique_ptr<class Instance> instance;
		std::unique_ptr<class Surface> surface;
	};

}

