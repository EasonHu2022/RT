#pragma once
#include "Wrapper.hpp"
#include "Utils.hpp"
#include <vector>
#include <memory>
#include "ImageView.hpp"
namespace Vulkan
{
	class Device;
	class ImageView;
	class Window;
	class SwapChain
	{
	public:
		VULKAN_OBJECT(SwapChain)
		SwapChain(const Device& device, VkPresentModeKHR presentMode);
		~SwapChain();
		VkPhysicalDevice get_physicalDevice() const { return physicalDevice; }
		const class Device& get_device() const { return device; }
		uint32_t get_min_imageCount() const { return minImageCount; }
		const std::vector<VkImage>& Images() const { return images; }
		const std::vector<std::unique_ptr<ImageView>>& get_imageViews() const { return imageViews; }
		const VkExtent2D& get_extent() const { return extent_; }
		VkFormat get_format() const { return format; }
		VkPresentModeKHR get_presentMode() const { return presentMode; }
	private:
		struct SupportDetails
		{
			VkSurfaceCapabilitiesKHR Capabilities{};
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
		};

		static SupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, VkPresentModeKHR presentMode);
		static VkExtent2D ChooseSwapExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities);
		static uint32_t ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

		const VkPhysicalDevice physicalDevice;
		const class Device& device;

		VULKAN_HANDLES(VkSwapchainKHR, swapChain)

			uint32_t minImageCount;
		VkPresentModeKHR presentMode;
		VkFormat format;
		VkExtent2D extent_{};
		std::vector<VkImage> images;
		std::vector<std::unique_ptr<ImageView>> imageViews;
	};

}

