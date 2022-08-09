#include "Vulkan/Application.hpp"
int main()
{
	const Vulkan::WindowConfig windowConfig
	{
		"Real-time Ray Tracing",
		1920,
		1080,
		false,
		false
	};
	Vulkan::Application application(windowConfig, VK_PRESENT_MODE_IMMEDIATE_KHR,true);
	application.Start();
	
	return 0;
}



//EOF vim:syntax=cpp:foldmethod=marker:ts=4:noexpandtab: 
