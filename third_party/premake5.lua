-- Third party projects

includedirs( "volk/include" );
includedirs( "vulkan/include" );

project( "x-volk" )
	kind "StaticLib"

	location "."

	files( "volk/src/*.c" )
	files( "volk/include/volk/*.h" )

project( "x-vulkan-headers" )
	kind "Utility"

	location "."

	files( "vulkan/include/**.c" )
	
project( "vma" )
	kind "StaticLib"

	location "."

	filter "toolset:gcc or toolset:clang"
		buildoptions { 
			"-Wno-unused-variable",
			"-Wno-reorder"
		}
	filter {}

	files( "VulkanMemoryAllocator/src/*.cpp" )
--EOF
