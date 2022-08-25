#include "Instance.hpp"
#include <sstream>
#include "Window.hpp"
#include "Utils.hpp"
namespace Vulkan
{

	VKAPI_ATTR VkBool32 VKAPI_CALL debug_util_callback(VkDebugUtilsMessageSeverityFlagBitsEXT aSeverity,
		VkDebugUtilsMessageTypeFlagsEXT aType, VkDebugUtilsMessengerCallbackDataEXT
		const* aData, void*);
    Instance::Instance(const Window& _window, const std::vector<const char*>& _validationLayers, uint32_t vulkanVersion)
        : window(_window), validationLayers(_validationLayers)
    {
		//load vulkan
		if (auto const res = volkInitialize(); VK_SUCCESS != res)
		{
			std::cerr << "Error: unable to load Vulkan API\n"<< std::endl;;
			
			return;
		}
		//check version
		uint32_t version = 0;
		Check(vkEnumerateInstanceVersion(&version),
			"query instance version");

		if (vulkanVersion > version)
		{
			std::ostringstream out;
			out << "minimum required version not found (required " /*<< Version(vulkanVersion)*/;
			out << ", found "/* << Version(version)*/ << ")";
			std::cerr << out.str() << std::endl;
		}

		// Get the list of required extensions.
		auto extensions = window.get_required_extensions();
		//validation layers
		const auto availableLayers = get_vkEnumerate(vkEnumerateInstanceLayerProperties);
		for (const char* layer : validationLayers)
		{
			auto result = std::find_if(availableLayers.begin(), availableLayers.end(), [layer](const VkLayerProperties& layerProperties)
				{
					return strcmp(layer, layerProperties.layerName) == 0;
				});

			if (result == availableLayers.end())
			{
				std::cerr << "could not find the requested validation layer: '" + std::string(layer) + "'" << std::endl;;
			}
		}
		if (!validationLayers.empty())
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		//create vulkan instance
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "RayTracer";
		appInfo.pEngineName = "YZH";
		appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
		appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
		appInfo.apiVersion = vulkanVersion;
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
		debugInfo.sType =
			VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugInfo.pfnUserCallback = &debug_util_callback;
		debugInfo.pUserData = nullptr;
		debugInfo.pNext = createInfo.pNext;
		createInfo.pNext = &debugInfo;
		Check(vkCreateInstance(&createInfo, nullptr, &instance),
			"create instance");
		volkLoadInstance(instance);


		GetVulkanPhysicalDevices();
		GetVulkanLayers();
		GetVulkanExtensions();
    }

    Instance::~Instance()
    {
		if (instance != nullptr)
		{
			vkDestroyInstance(instance, nullptr);
			instance = nullptr;
		}
    }

	void Instance::GetVulkanPhysicalDevices()
	{
		get_vkEnumerate(instance, vkEnumeratePhysicalDevices, physicalDevices);

		if (physicalDevices.empty())
		{
			std::cerr << "found no Vulkan physical devices" << std::endl;
			//Throw(std::runtime_error("found no Vulkan physical devices"));
		}
	}

	void Instance::GetVulkanLayers()
	{
		get_vkEnumerate(vkEnumerateInstanceLayerProperties, layers);
	}

	void Instance::GetVulkanExtensions()
	{
		get_vkEnumerate(static_cast<const char*>(nullptr), vkEnumerateInstanceExtensionProperties, extensions);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL debug_util_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT aSeverity,
		VkDebugUtilsMessageTypeFlagsEXT aType, VkDebugUtilsMessengerCallbackDataEXT
		const* aData, void*)
	{
		std::fprintf(stderr, "%s (%s): %s (%d)\n%s\n--\n", to_string(aSeverity).c_str()
			,message_type_flags(aType).c_str(), aData->pMessageIdName, aData->
			messageIdNumber, aData->pMessage);
		return VK_FALSE;
	}
};