-- /premake5.lua
-- COURSEWORK APP PREMAKE --


function success ( name )
	print ( name .. " is configured successfully!" )
end


local project_name = "CourseWork"
local config_path  = "config.json"

workspace ( project_name )
	architecture "x86_64"
	startproject ( project_name )

	configurations { "Debug", "Release" }

	
	local config, err = json.decode ( io.readfile ( config_path ))
	if not config then
		print ( "\n" .. err )
		os.exit()
	end

	root_dir     = "%{wks.location}"
	output_dir   = "%{cfg.buildcfg}_%{cfg.system}"

	bin_dir      = config.dir.binaries              .. "\\" .. output_dir
	bininter_dir = config.dir.binaries_intermediate .. "\\" .. output_dir

	local src_dir   = root_dir .. "\\" .. config.dir.src
	local inc_dir   = root_dir .. "\\" .. config.dir.include
	local deps_dir  = root_dir .. "\\" .. config.dir.deps

	local build_dir = root_dir .. "\\" .. config.dir.build .. "\\" .. output_dir .. "\\" .. project_name


	-- Virtual dir to contain several projects
	group "DEPENDENCIES"
		include ( config.dir.deps .. "\\glfw.lua"  )
		include ( config.dir.deps .. "\\glad.lua"  )
		include ( config.dir.deps .. "\\imgui.lua" )
		-- 'glm'      does not require compilation, it's header only
		-- 'freetype' does not require compilation, it has a compiled lib & is used only inside imgui
	group ""


	flags {
		"FatalWarnings",		 -- Treat all warnings as errors
		"MultiProcessorCompile"  -- Enable VS to use multiple compiler processes when building
	}


	project ( project_name )
		targetname ( project_name .. " App" )  -- Executable name
		kind "ConsoleApp"  -- Kind of binary object created by the project (https://premake.github.io/docs/kind/)
		language "C++"
		cppdialect "C++latest"
		staticruntime "off"	

		dependson {
			"GLFW",
			"Glad",
			"ImGui"
		}


		targetdir ( root_dir .. "\\" .. bin_dir      )  -- Destination dir for compiled binary target
		objdir    ( root_dir .. "\\" .. bininter_dir )  -- Destination dir for object and other intermediate files

		-- Include file search paths for the compiler
		includedirs {
			src_dir,
			inc_dir,

			deps_dir .. "\\glfw\\include",
			deps_dir .. "\\glfw\\include\\GLFW",
			deps_dir .. "\\glad\\include",
			deps_dir .. "\\glad\\include\\glad",
			deps_dir .. "\\imgui",
			deps_dir .. "\\glm"
		}

		files {
			src_dir  .. "\\**.h",
			src_dir  .. "\\**.c",
			src_dir  .. "\\**.hpp",
			src_dir  .. "\\**.cpp",

			deps_dir .. "\\glm\\glm\\**.hpp",
			deps_dir .. "\\glm\\glm\\**.inl",

			deps_dir .. "\\imgui\\backends\\imgui_impl_glfw.cpp",
			deps_dir .. "\\imgui\\backends\\imgui_impl_opengl3.cpp",
		}

		-- Library search paths for the linker
		libdirs { deps_dir }

		-- List of libraries & projects to link against
		links {
			"glfw\\"  .. bin_dir .. "\\GLFW.lib",
			"glad\\"  .. bin_dir .. "\\Glad.lib",
			"imgui\\" .. bin_dir .. "\\ImGui.lib",
			"opengl32.lib"
		}


		postbuildcommands {
			"\"" .. config.dir.script .. "\"\\" .. "setup.py solution copy " .. build_dir
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
		

		success ( project_name )