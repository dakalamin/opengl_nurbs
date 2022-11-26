-- dependencies/glad.lua
-- GLAD LIB PREMAKE --


project "Glad"
	location "glad"

	kind "StaticLib"
	language "C"
	staticruntime "off"
	
	targetdir ( "%{prj.location}/bin/"       .. outputdir )
	objdir    ( "%{prj.location}/bin_inter/" .. outputdir )


	local srcdir = "glad/src/"
	local incdir = "glad/include/"

	includedirs { incdir }

	files {
		incdir .. "glad/gl.h",
		incdir .. "KHR/khrplatform.h",

		srcdir .. "gl.c"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"