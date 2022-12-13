-- dependencies/glad.lua
-- GLAD LIB PREMAKE --


project "Glad"
	local project_name = "glad"
	local src_dir = project_name .. "/src/"
	local inc_dir = project_name .. "/include/"


	location ( project_name )

	kind "StaticLib"
	language "C"
	staticruntime "off"
	
	targetdir ( "%{prj.location}/" .. bin_dir      )
	objdir    ( "%{prj.location}/" .. bininter_dir )

	includedirs { inc_dir }

	files {
		inc_dir .. "glad/glad.h",
		inc_dir .. "KHR/khrplatform.h",

		src_dir .. "glad.c"
	}


	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter {}  -- reset filter


	success ( project_name )