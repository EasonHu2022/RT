#pragma once
#include "Wrapper.hpp"
#include "WindowConfig.hpp"
#include <functional>
namespace Vulkan
{
	class Window 
	{
	public:
		VULKAN_OBJECT(Window)
		Window(const WindowConfig& config);
		~Window();
		GLFWwindow* get_handle() const { return glfwWindow; }
		float get_content_scale() const;
		
		VkExtent2D get_window_size() const;

		VkExtent2D get_framebuffer_size() const;

		std::vector<const char*> get_required_extensions() const;


		double get_time() const;
		void Close();
		void Start();


		//callbacks
		std::function<void()> Loops;






	private:

		const WindowConfig _config;
		GLFWwindow* glfwWindow;
	};

}

