#include "Window.hpp"
#include <iostream>
#include "stb_image.h"
#include "Exception.hpp"
namespace Vulkan
{
	namespace
	{
		void GlfwErrorCallback(const int error, const char* const description)
		{
			std::cerr << "ERROR: GLFW: " << description << " (code: " << error << ")" << std::endl;
		}

		void GlfwKeyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
		{
			auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (this_->OnKey)
			{
				this_->OnKey(key, scancode, action, mods);
			}
		}

		void GlfwCursorPositionCallback(GLFWwindow* window, const double xpos, const double ypos)
		{
			auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (this_->OnCursorPosition)
			{
				this_->OnCursorPosition(xpos, ypos);
			}
		}

		void GlfwMouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
		{
			auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (this_->OnMouseButton)
			{
				this_->OnMouseButton(button, action, mods);
			}
		}

		void GlfwScrollCallback(GLFWwindow* window, const double xoffset, const double yoffset)
		{
			auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (this_->OnScroll)
			{
				this_->OnScroll(xoffset, yoffset);
			}
		}
	};


	Window::Window(const WindowConfig& config) : _config(config)
	{
		glfwSetErrorCallback(GlfwErrorCallback);
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
		if (glfwWindow == nullptr)
		{
			Throw(std::runtime_error("failed to create window"));
		}


		if (config.CursorDisabled)
		{
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		glfwSetWindowUserPointer(glfwWindow, this);
		glfwSetKeyCallback(glfwWindow, GlfwKeyCallback);
		glfwSetCursorPosCallback(glfwWindow, GlfwCursorPositionCallback);
		glfwSetMouseButtonCallback(glfwWindow, GlfwMouseButtonCallback);
		glfwSetScrollCallback(glfwWindow, GlfwScrollCallback);
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

	const char* Window::get_key_name(int key, int scancode) const
	{
		return glfwGetKeyName(key, scancode);
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

			if (DrawFrame)
			{
				DrawFrame();
			}
		}
	}
	bool Window::IsMinimized() const
	{
		const auto size = get_framebuffer_size();
		return size.height == 0 && size.width == 0;
	}
	void Window::WaitForEvents() const
	{
		glfwWaitEvents();
	}
};