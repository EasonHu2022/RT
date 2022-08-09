#pragma once
#include "Wrapper.hpp"
#include "Utils.hpp"
#include "Instance.hpp"
namespace Vulkan
{
	class Instance;
	class Window;
	class Surface
	{
	public:
		VULKAN_OBJECT(Surface)


		explicit Surface(const Instance& instance);
		~Surface();
		const class Instance& get_instance() const { return instance; }
	private:
		const class Instance& instance;
		VULKAN_HANDLES(VkSurfaceKHR,surface)
	};

}

