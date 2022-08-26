#include "Vulkan/Application.hpp"
#include "UserSettings.hpp"
#include "Options.hpp"
#include "RayTracer.hpp"
#include "Version.hpp"
#include <Wrapper.hpp>
#include <Exception.hpp>
namespace
{
	UserSettings CreateUserSettings(const Options& options);
	void PrintVulkanSdkInformation();
	void PrintVulkanInstanceInformation(const Vulkan::Application& application, bool benchmark);
	void PrintVulkanLayersInformation(const Vulkan::Application& application, bool benchmark);
	void PrintVulkanDevices(const Vulkan::Application& application );
	void PrintVulkanSwapChainInformation(const Vulkan::Application& application, bool benchmark);
	void SetVulkanDevice(Vulkan::Application& application);
}

int main(int argc, const char* argv[])
{
	const Options options(argc, argv);
	const UserSettings userSettings = CreateUserSettings(options);
	const Vulkan::WindowConfig windowConfig
	{
		"Real-time Ray Tracing",
		options.Width,
		options.Height,
		options.Benchmark&& options.Fullscreen,
			options.Fullscreen,
			false
	};
	RayTracer application(userSettings, windowConfig, static_cast<VkPresentModeKHR>(options.PresentMode));
	PrintVulkanSdkInformation();
	PrintVulkanInstanceInformation(application, options.Benchmark);
	PrintVulkanLayersInformation(application, options.Benchmark);
	PrintVulkanDevices(application);

	SetVulkanDevice(application);

	PrintVulkanSwapChainInformation(application, options.Benchmark);

	application.Start();

	return EXIT_SUCCESS;
	
	return 0;
}

namespace
{

	UserSettings CreateUserSettings(const Options& options)
	{
		UserSettings userSettings{};

		userSettings.Benchmark = options.Benchmark;
		userSettings.BenchmarkNextScenes = options.BenchmarkNextScenes;
		userSettings.BenchmarkMaxTime = options.BenchmarkMaxTime;

		userSettings.SceneIndex = options.SceneIndex;

		userSettings.IsRayTraced = true;
		userSettings.AccumulateRays = true;
		userSettings.NumberOfSamples = options.Samples;
		userSettings.NumberOfBounces = options.Bounces;
		userSettings.MaxNumberOfSamples = options.MaxSamples;

		userSettings.ShowSettings = !options.Benchmark;
		userSettings.ShowOverlay = true;

		userSettings.ShowHeatmap = false;
		userSettings.HeatmapScale = 1.5f;
		return userSettings;
	}

	void PrintVulkanSdkInformation()
	{
		std::cout << "Vulkan SDK Header Version: " << VK_HEADER_VERSION << std::endl;
		std::cout << std::endl;
	}

	void PrintVulkanInstanceInformation(const Vulkan::Application& application, const bool benchmark)
	{
		if (benchmark)
		{
			return;
		}

		std::cout << "Vulkan Instance Extensions: " << std::endl;

		for (const auto& extension : application.get_extensions())
		{
			std::cout << "- " << extension.extensionName << " (" << Vulkan::Version(extension.specVersion) << ")" << std::endl;
		}

		std::cout << std::endl;
	}

	void PrintVulkanLayersInformation(const Vulkan::Application& application, const bool benchmark)
	{
		if (benchmark)
		{
			return;
		}

		std::cout << "Vulkan Instance Layers: " << std::endl;

		for (const auto& layer : application.get_layers())
		{
			std::cout
				<< "- " << layer.layerName
				<< " (" << Vulkan::Version(layer.specVersion) << ")"
				<< " : " << layer.description << std::endl;
		}

		std::cout << std::endl;
	}

	void PrintVulkanDevices(const Vulkan::Application& application)
	{
		std::cout << "Vulkan Devices: " << std::endl;

		for (const auto& device : application.get_physicalDevices())
		{
			VkPhysicalDeviceDriverProperties driverProp{};
			driverProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

			VkPhysicalDeviceProperties2 deviceProp{};
			deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			deviceProp.pNext = &driverProp;

			vkGetPhysicalDeviceProperties2(device, &deviceProp);

			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(device, &features);

			const auto& prop = deviceProp.properties;

			const Vulkan::Version vulkanVersion(prop.apiVersion);
			const Vulkan::Version driverVersion(prop.driverVersion, prop.vendorID);

			std::cout << "- [" << prop.deviceID << "] ";
			std::cout <<Vulkan::VendorId(prop.vendorID) << " '" << prop.deviceName;
			std::cout << "' (";
			std::cout << Vulkan::DeviceType(prop.deviceType) << ": ";
			std::cout << "vulkan " << vulkanVersion << ", ";
			std::cout << "driver " << driverProp.driverName << " " << driverProp.driverInfo << " - " << driverVersion;
			std::cout << ")" << std::endl;

		}

		std::cout << std::endl;
	}

	void PrintVulkanSwapChainInformation(const Vulkan::Application& application, const bool benchmark)
	{
		const auto& swapChain = application.get_swapChain();

		std::cout << "Swap Chain: " << std::endl;
		std::cout << "- image count: " << swapChain.Images().size() << std::endl;
		std::cout << "- present mode: " << swapChain.get_presentMode() << std::endl;
		std::cout << std::endl;
	}

	void SetVulkanDevice(Vulkan::Application& application)
	{
		const auto& physicalDevices = application.get_physicalDevices();
		const auto result = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](const VkPhysicalDevice& device)
			{
				// We want a device with geometry shader support.
				VkPhysicalDeviceFeatures deviceFeatures;
				vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

				if (!deviceFeatures.geometryShader)
				{
					return false;
				}

				// We want a device that supports the ray tracing extension.
				const auto extensions = Vulkan::get_vkEnumerate(device, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties);
				const auto hasRayTracing = std::find_if(extensions.begin(), extensions.end(), [](const VkExtensionProperties& extension)
					{
						return strcmp(extension.extensionName, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) == 0;
					});

				if (hasRayTracing == extensions.end())
				{
					std::cerr << "noRayTracing" << std::endl;
					return false;
				}

				// We want a device with a graphics queue.
				const auto queueFamilies = Vulkan::get_vkEnumerate(device, vkGetPhysicalDeviceQueueFamilyProperties);
				const auto hasGraphicsQueue = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& queueFamily)
					{		
						
						return queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
					});

				if (hasGraphicsQueue == queueFamilies.end())
				{
					std::cerr << "no graphic queue" << std::endl;

					return false;
				}
				
			});

		if (result == physicalDevices.end())
		{
			//std::cerr << "cannot find a suitable device" << std::endl;
			Vulkan::Throw(std::runtime_error("cannot find a suitable device"));
		}

		VkPhysicalDeviceProperties2 deviceProp{};
		deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		vkGetPhysicalDeviceProperties2(*result, &deviceProp);

		std::cout << "Setting Device [" << deviceProp.properties.deviceID << "]:" << std::endl;

		application.set_physical_device(*result);

		

		std::cout << std::endl;
	}

}



//EOF vim:syntax=cpp:foldmethod=marker:ts=4:noexpandtab: 
