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
		// GLFW instance properties (i.e. not bound to a window handler).
		const char* get_key_name(int key, int scancode) const;

		double get_time() const;
		void Close();
		void Start();


		bool IsMinimized() const;
		void WaitForEvents() const;
		//callbacks
		std::function<void()> Loops;


		// Window instance properties.
		const WindowConfig& get_config() const { return _config; }



	private:

		const WindowConfig _config;
		GLFWwindow* glfwWindow;
	};

}

