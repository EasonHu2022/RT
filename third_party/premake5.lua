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
	
project( "x-glm" )
	kind "Utility"

	location "."

	files( "glm/include/**.h" )
	files( "glm/include/**.hpp" )
	files( "glm/include/**.inl" )
	
project( "x-stb" )
	kind "StaticLib"

	location "."
	includedirs
	{
		"./stb/include",

	}
	files( "stb/src/*.c" )
	files( "stb/include/*.h" )
--EOF
