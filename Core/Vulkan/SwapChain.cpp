#include "SwapChain.hpp"
#include "Device.hpp"
#include "Surface.hpp"
#include "Window.hpp"
namespace Vulkan
{
	SwapChain::SwapChain(const Device& _device, VkPresentModeKHR _presentMode):device(_device),physicalDevice(_device.get_physicalDevice())
	{
		const auto details = QuerySwapChainSupport(device.get_physicalDevice(), device.get_surface().Handle());
		if (details.Formats.empty() || details.PresentModes.empty())
		{
			throw std::runtime_error("empty swap chain support");
		}

		const auto& surface = device.get_surface();
		const auto& window = surface.get_instance().get_window();

		const auto surfaceFormat = ChooseSwapSurfaceFormat(details.Formats);
		const auto actualPresentMode = ChooseSwapPresentMode(details.PresentModes, _presentMode);
		const auto extent = ChooseSwapExtent(window, details.Capabilities);
		const auto imageCount = ChooseImageCount(details.Capabilities);

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface.Handle();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		createInfo.preTransform = details.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = actualPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = nullptr;

		if (device.get_graphicsFamiltyInde() != device.get_presentFamilyIndex())
		{
			uint32_t queueFamilyIndices[] = { device.get_graphicsFamiltyInde(), device.get_presentFamilyIndex() };

			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		Check(vkCreateSwapchainKHR(device.Handle(), &createInfo, nullptr, &swapChain),
			"create swap chain!");

		minImageCount = std::max(2u, details.Capabilities.minImageCount);
		presentMode = actualPresentMode;
		format = surfaceFormat.format;
		extent_ = extent;
		images = get_vkEnumerate(device.Handle(), swapChain, vkGetSwapchainImagesKHR);
		imageViews.reserve(images.size());

		for (const auto image : images)
		{
			imageViews.push_back(std::make_unique<ImageView>(device, image, format, VK_IMAGE_ASPECT_COLOR_BIT));
		}
	}
	SwapChain::~SwapChain()
	{
		imageViews.clear();

		if (swapChain != nullptr)
		{
			vkDestroySwapchainKHR(device.Handle(), swapChain, nullptr);
			swapChain = nullptr;
		}
	}
	SwapChain::SupportDetails SwapChain::QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
	{
		SupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.Capabilities);
		details.Formats = get_vkEnumerate(physicalDevice, surface, vkGetPhysicalDeviceSurfaceFormatsKHR);
		details.PresentModes = get_vkEnumerate(physicalDevice, surface, vkGetPhysicalDeviceSurfacePresentModesKHR);

		return details;
	}
	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		{
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto& format : formats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}
		std::cerr << "found no suitable suface format" << std::endl;
		//Throw(std::runtime_error("found no suitable surface format"));
	}
	VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, VkPresentModeKHR presentMode)
	{
		// VK_PRESENT_MODE_IMMEDIATE_KHR specifies that the presentation engine does not wait for a vertical blanking period
	// to update the current image, meaning this mode may result in visible tearing. No internal queuing of presentation
	// requests is needed, as the requests are applied immediately.

	// VK_PRESENT_MODE_MAILBOX_KHR specifies that the presentation engine waits for the next vertical blanking period to
	// update the current image. Tearing cannot be observed. An internal single-entry queue is used to hold pending
	// presentation requests. If the queue is full when a new presentation request is received, the new request replaces
	// the existing entry, and any images associated with the prior entry become available for re-use by the application.
	// One request is removed from the queue and processed during each vertical blanking period in which the queue is non-empty.

	// VK_PRESENT_MODE_FIFO_KHR specifies that the presentation engine waits for the next vertical blanking period to update
	// the current image. Tearing cannot be observed. An internal queue is used to hold pending presentation requests.
	// New requests are appended to the end of the queue, and one request is removed from the beginning of the queue and
	// processed during each vertical blanking period in which the queue is non-empty. This is the only value of presentMode
	// that is required to be supported.

	// VK_PRESENT_MODE_FIFO_RELAXED_KHR specifies that the presentation engine generally waits for the next vertical blanking
	// period to update the current image. If a vertical blanking period has already passed since the last update of the current
	// image then the presentation engine does not wait for another vertical blanking period for the update, meaning this mode
	// may result in visible tearing in this case. This mode is useful for reducing visual stutter with an application that will
	// mostly present a new image before the next vertical blanking period, but may occasionally be late, and present a new
	// image just after the next vertical blanking period. An internal queue is used to hold pending presentation requests.
	// New requests are appended to the end of the queue, and one request is removed from the beginning of the queue and
	// processed during or after each vertical blanking period in which the queue is non-empty.


		switch (presentMode)
		{
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
		case VK_PRESENT_MODE_MAILBOX_KHR:
		case VK_PRESENT_MODE_FIFO_KHR:
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR:

			if (std::find(presentModes.begin(), presentModes.end(), presentMode) != presentModes.end())
			{
				return presentMode;
			}

			break;

		default:
			std::cerr << "unknown present mode "<< std::endl;
			//Throw(std::out_of_range("unknown present mode"));
		}

		// Fallback
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D SwapChain::ChooseSwapExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities)
	{
		// Vulkan tells us to match the resolution of the window by setting the width and height in the currentExtent member.
	// However, some window managers do allow us to differ here and this is indicated by setting the width and height in
	// currentExtent to a special value: the maximum value of uint32_t. In that case we'll pick the resolution that best 
	// matches the window within the minImageExtent and maxImageExtent bounds.
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}

		auto actualExtent = window.get_framebuffer_size();

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
	uint32_t SwapChain::ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		// The implementation specifies the minimum amount of images to function properly
	// and we'll try to have one more than that to properly implement triple buffering.
	// (tanguyf: or not, we can just rely on VK_PRESENT_MODE_MAILBOX_KHR with two buffers)
		uint32_t imageCount = std::max(2u, capabilities.minImageCount);// +1; 

		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		{
			imageCount = capabilities.maxImageCount;
		}

		return imageCount;
	}
};