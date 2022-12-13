-- dependencies/imgui_vesk.lua
-- IMGUI LIB PREMAKE --


project "ImGui"
	local project_name = "imgui"
	local src_dir = project_name .. "/"


	location ( project_name )

	kind "StaticLib"
	language "C++"

	targetdir ( "%{prj.location}/" .. bin_dir      )
	objdir    ( "%{prj.location}/" .. bininter_dir )

	includedirs {
		src_dir,
		src_dir .. "misc/freetype/",

		"freetype/include/"
	}

	files {
		src_dir .. "imconfig.h",
		src_dir .. "imgui.h",
		src_dir .. "imgui.cpp",
		src_dir .. "imgui_draw.cpp",
		src_dir .. "imgui_internal.h",
		src_dir .. "imgui_tables.cpp",
		src_dir .. "imgui_widgets.cpp",
		src_dir .. "imstb_rectpack.h",
		src_dir .. "imstb_textedit.h",
		src_dir .. "imstb_truetype.h",

		src_dir .. "imgui_demo.cpp",

		src_dir .. "misc/freetype/imgui_freetype.cpp",
		src_dir .. "misc/freetype/imgui_freetype.h"
	}

	links { "freetype/freetype.lib" }


	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter {}  -- reset filter


	success ( project_name )