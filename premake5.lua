-- /premake5.lua
-- COURSEWORK APP PREMAKE --


workspace "CourseWork"
	architecture "x86_64"
	startproject "CourseWork"

	configurations { "Debug", "Release" }
	

	root      = "%{wks.location}/"
	outputdir = "%{cfg.buildcfg}-%{cfg.system}"
	bindir    = "bin/" .. outputdir

	local srcdir   = root .. "source"
	local incdir   = root .. "include"
	local depsdir  = root .. "dependencies"

	local builddir = "%{wks.location}/build/" .. outputdir


	-- Virtual dir to contain several projects
	group "DEPENDENCIES"
		include "dependencies/glfw.lua"
		include "dependencies/glad.lua"
		include "dependencies/imgui.lua"
		-- 'glm'      does not require compilation, it's header only
		-- 'freetype' does not require compilation, it has a compiled lib & is used only inside imgui
	group ""


	flags {
		"FatalWarnings",		 -- Treat all warnings as errors
		"MultiProcessorCompile"  -- Enable VS to use multiple compiler processes when building
	}


	project "CourseWork"
		targetname "CourseWork App"  -- Executable name
		kind "ConsoleApp"  -- Kind of binary object created by the project (https://premake.github.io/docs/kind/)
		language "C++"
		cppdialect "C++latest"
		staticruntime "off"	

		dependson {
			"GLFW",
			"Glad",
			"ImGui"
		}


		targetdir (root .. bindir)  -- Destination dir for compiled binary target
		objdir    (root .. "bin_inter/" .. outputdir)  -- Destination dir for object and other intermediate files

		-- Include file search paths for the compiler
		includedirs {
			srcdir,
			incdir,

			depsdir .. "/glfw/include",
			depsdir .. "/glad/include",
			depsdir .. "/imgui",
			depsdir .. "/glm"
		}

		files {
			srcdir  .. "/**.h",
			srcdir  .. "/**.c",
			srcdir  .. "/**.hpp",
			srcdir  .. "/**.cpp",

			depsdir .. "/glm/glm/**.hpp",
			depsdir .. "/glm/glm/**.inl",

			depsdir .. "/imgui/backends/imgui_impl_glfw.cpp",
			depsdir .. "/imgui/backends/imgui_impl_opengl3.cpp",
		}

		-- Library search paths for the linker
		libdirs { depsdir }

		-- List of libraries & projects to link against
		links {
			"glfw/"  .. bindir .. "/GLFW.lib",
			"glad/"  .. bindir .. "/Glad.lib",
			"imgui/" .. bindir .. "/ImGui.lib",
			"opengl32.lib"
		}


		filter "system:windows"
			systemversion "latest"
		filter "system:linux"
			-- links { }
		filter "system:macosx"
			-- links { }

		filter "configurations:Debug"
			defines "CW_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "CW_RELEASE"
			runtime "Release"
			optimize "on"

		filter {}  -- reset filter
