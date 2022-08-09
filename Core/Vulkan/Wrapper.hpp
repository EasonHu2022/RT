#pragma once
#include <volk/volk.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>



#define VULKAN_OBJECT(Class) \
	Class(const Class&) = delete; \
	Class(Class&&) = delete; \
	Class& operator = (const Class&) = delete; \
	Class& operator = (Class&&) = delete;

#define VULKAN_HANDLES(VulkanHandles, name) \
public: \
	VulkanHandles Handle() const { return name; } \
private: \
	VulkanHandles name{};


namespace Vulkan {

    void Check(const VkResult result, const char* const operation);
    const char* ToString(const VkResult result);
	std::string to_string(VkDebugUtilsMessageSeverityFlagBitsEXT aSeverity);
	std::string message_type_flags(VkDebugUtilsMessageTypeFlagsEXT aFlags);
}
