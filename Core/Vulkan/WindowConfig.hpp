#pragma once
#include <string>
namespace Vulkan
{
	struct WindowConfig
	{
		std::string windowTitle;
		uint32_t width;
		uint32_t height;
		bool Fullscreen = false;
		bool Resizable = false;
	};

}

