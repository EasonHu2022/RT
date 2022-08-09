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
		const VkExtent2D& get_extent() const { return extent; }
		VkFormat get_format() const { return format; }
		VkPresentModeKHR get_presentMode() const { return presentMode; }
	private:
		const VkPhysicalDevice physicalDevice;
		//LD	
		const class Device& device;
		uint32_t minImageCount;
		std::vector<VkImage> images;
		std::vector<std::unique_ptr<ImageView>> imageViews;
		VkFormat format;
		VkPresentModeKHR presentMode;
		VkExtent2D extent;
		VULKAN_HANDLES(VkSwapchainKHR,swapChain)
	};

}

