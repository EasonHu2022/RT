workspace "RayTracer"
	language "C++"
	cppdialect "C++17"

	platforms { "x64" }
	configurations { "debug", "release" }

	flags "NoPCH"
	flags "MultiProcessorCompile"

	startproject "RayTracing VK"

	debugdir "%{wks.location}"
	objdir "_build_/%{cfg.buildcfg}-%{cfg.platform}-%{cfg.toolset}"
	targetsuffix "-%{cfg.buildcfg}-%{cfg.platform}-%{cfg.toolset}"
	
	-- Default toolset options
	filter "toolset:gcc or toolset:clang"
		linkoptions { "-pthread" }
		buildoptions { "-march=native", "-Wall", "-pthread" }

	filter "toolset:msc-*"
		defines { "_CRT_SECURE_NO_WARNINGS=1" }
		defines { "_SCL_SECURE_NO_WARNINGS=1" }
		buildoptions { "/utf-8" }
	
	filter {}

	
	filter "system:windows"

	filter {}

	-- default outputs
	filter "kind:StaticLib"
		targetdir "lib/"

	filter "kind:ConsoleApp"
		targetdir "bin/"
		targetextension ".exe"
	
	filter {}

	--configurations
	filter "debug"
		symbols "On"
		defines { "_DEBUG=1" }

	filter "release"
		optimize "On"
		defines { "NDEBUG=1" }

	filter {}

-- Third party dependencies
include "third_party" 
include "third_party/GLFW" 
-- Projects
project "RayTracing VK"
	local sources = { 
		"Core/**.cpp",
		"Core/**.hpp",
		"Core/**.hxx",
		"Core/**.h",
		"./third_party/ImGUI/**.h",
		"./third_party/ImGUI/**.cpp",
		"./third_party/tiny_obj_loader/**.h",
		"./third_party/tiny_obj_loader/**.cc",
	}

	kind "ConsoleApp"

	files( sources )


	includedirs
	{
		"./third_party/GLFW/include",
		"./third_party/volk/include",
		"./third_party/glm/include",
		"./third_party/stb/include",
		"./third_party/tiny_obj_loader",
		"./third_party/VulkanMemoryAllocator/include",
		"./third_party/ImGUI",
		"./Core/Utilities",
		"./Core/Vulkan",
		"./Core/Assets"
	}
	
	defines 
	{ 
		"_GLFW_WIN32",
		"_CRT_SECURE_NO_WARNINGS"
	}
	
	links "x-volk"
	links "GLFW"
	links "vma"
	links "x-glm"
	links "x-stb"
--EOF
