#include "Surface.hpp"
#include "Window.hpp"

namespace Vulkan
{
	Surface::Surface(const Instance& _instance):instance(_instance)
	{
		Check(glfwCreateWindowSurface(instance.Handle(),instance.get_window().get_handle(), nullptr, &surface), "create window surface");
	}

	Surface::~Surface()
	{
		if (surface != nullptr)
		{
			vkDestroySurfaceKHR(instance.Handle(),surface,nullptr);
			surface = nullptr;
		}
	}
};