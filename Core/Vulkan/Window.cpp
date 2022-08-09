#include "Window.hpp"
#include <iostream>
namespace Vulkan
{
	namespace callbacks
	{
		void GlfwErrorCallback(const int error, const char* const description)
		{
			std::cerr << "ERROR: GLFW: " << description << " (code: " << error << ")" << std::endl;
		}
	};


	Window::Window(const WindowConfig& config) : _config(config)
	{
		glfwSetErrorCallback(callbacks::GlfwErrorCallback);
		if (!glfwInit())
		{
			std::cerr << "glfwInit() failed!" << std::endl;
		}

		if (!glfwVulkanSupported())
		{
			std::cerr << "glfwVulkanSupported() failed!" << std::endl;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE,  GLFW_FALSE);
		glfwWindow = glfwCreateWindow(config.width, config.height, config.windowTitle.c_str(), nullptr, nullptr);
	}

	Window::~Window()
	{
		if (glfwWindow != nullptr)
		{
			glfwDestroyWindow(glfwWindow);
			glfwWindow = nullptr;
		}

		glfwTerminate();
		glfwSetErrorCallback(nullptr);

	}
	float Window::get_content_scale() const
	{
		float xscale;
		float yscale;
		glfwGetWindowContentScale(glfwWindow, &xscale, &yscale);
		return xscale;
	}

	VkExtent2D Window::get_framebuffer_size() const
	{
		int width, height;
		glfwGetFramebufferSize(glfwWindow, &width, &height);
		return VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
	}

	std::vector<const char*> Window::get_required_extensions() const
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	}

	VkExtent2D Window::get_window_size() const
	{
		int width, height;
		glfwGetWindowSize(glfwWindow, &width, &height);
		return VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
	}

	double Window::get_time() const
	{
		return glfwGetTime();
	}

	void Window::Close()
	{
		glfwSetWindowShouldClose(glfwWindow,1);
	}

	void Window::Start()
	{
		glfwSetTime(0.0);

		while (!glfwWindowShouldClose(glfwWindow))
		{
			glfwPollEvents();

			if (Loops)
			{
				Loops();
			}
		}
	}
};