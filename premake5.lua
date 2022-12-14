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
dofile( "glslc.lua" )
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
		"./third_party/freetype/**.h",
		--"./third_party/boost/**.h",
		--"./third_party/boost/**.hpp",
	}

	kind "ConsoleApp"

	files( sources )
	includedirs
	{
		"./third_party/GLFW/include",
		"./third_party/volk/include",
		"./third_party/freetype/include",
		"./third_party/glm/include",
		"./third_party/stb/include",
		"./third_party/tiny_obj_loader",
		"./third_party/ImGUI",
		"./third_party",
		"./Core/Utilities",
		"./Core/Vulkan",
		"./Core/Assets",
		"./third_party/boost_1_80_0"
	}
	
	libdirs
		{
			"./third_party/boost_1_80_0/stage/lib",
		}
	defines 
	{ 
		"_GLFW_WIN32",
		"_CRT_SECURE_NO_WARNINGS"
	}
	
	links "x-volk"
	links "GLFW"
	links "x-glm"
	links "x-stb"
	links "./lib/freetype.lib"
	
	
project "RT-shaders"
	local shaders = { 
		"./Core/Assets/shaders/*.frag",
		"./Core/Assets/shaders/*.vert",
		"./Core/Assets/shaders/*.glsl",
		"./Core/Assets/shaders/*.rchit",
		"./Core/Assets/shaders/*.rint",
		"./Core/Assets/shaders/*.rgen",
		"./Core/Assets/shaders/*.rmiss",
	}

	kind "Utility"
	location "./Core/Assets/shaders"

	files( shaders )

	handle_glsl_files( "-O", "assets/shaders", {} )	
--EOF
