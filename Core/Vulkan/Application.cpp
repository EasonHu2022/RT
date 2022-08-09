#include "Application.hpp"
#include "Instance.hpp"
#include "Surface.hpp"
namespace Vulkan
{
	Application::~Application()
	{
		surface.reset();
		instance.reset();
		window.reset();
	}
	const std::vector<VkExtensionProperties>& Application::get_extensions() const
	{
		// TODO: 在此处插入 return 语句
		return {};
	}
	const std::vector<VkLayerProperties>& Application::get_layers() const
	{
		// TODO: 在此处插入 return 语句
		return {};
	}
	const std::vector<VkPhysicalDevice>& Application::get_physicalDevices() const
	{
		// TODO: 在此处插入 return 语句
		return {};
	}
	Application::Application(const WindowConfig& windowConfig, VkPresentModeKHR _presentMode, bool enableValidationLayers): presentMode(_presentMode)
	{
		const auto validationLayers = enableValidationLayers ? std::vector<const char*>{"VK_LAYER_KHRONOS_validation"} : std::vector<const char*>();
		window.reset(new class Window(windowConfig));
		instance.reset(new Instance(*window,validationLayers,VK_API_VERSION_1_2));
		surface.reset(new Surface(*instance));
	}
	void Application::set_physical_device(VkPhysicalDevice physicalDevice)
	{
	}
	void Application::Start()
	{
		window->Start();
	}

}