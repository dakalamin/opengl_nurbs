-- dependencies/glfw.lua
-- GLFW LIB PREMAKE --


project "GLFW"
	local project_name = "glfw"
	local src_dir = project_name .. "/src/"
	local inc_dir = project_name .. "/include/GLFW/"


	location ( project_name )

	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir ( "%{prj.location}/" .. bin_dir      )
	objdir    ( "%{prj.location}/" .. bininter_dir )

	includedirs { inc_dir }

	files {
		inc_dir .. "glfw3.h",
		inc_dir .. "glfw3native.h",
		
		src_dir .. "context.c",
		src_dir .. "glfw_config.h",
		src_dir .. "init.c",
		src_dir .. "input.c",
		src_dir .. "internal.h",
		src_dir .. "monitor.c",
		src_dir .. "platform.c",
		src_dir .. "vulkan.c",
		src_dir .. "window.c",

		src_dir .. "null_init.c",
		src_dir .. "null_joystick.c",
		src_dir .. "null_monitor.c",
		src_dir .. "null_platform.h",
		src_dir .. "null_window.c"		
	}


	filter "system:windows"
		systemversion "latest"

		files {
			src_dir .. "win32_init.c",
			src_dir .. "win32_joystick.c",
			src_dir .. "win32_module.c",
			src_dir .. "win32_monitor.c",
			src_dir .. "win32_platform.h",
			src_dir .. "win32_thread.c",
			src_dir .. "win32_time.c",
			src_dir .. "win32_window.c",

			src_dir .. "egl_context.c",
			src_dir .. "wgl_context.c",
			src_dir .. "osmesa_context.c"
		}

		defines { 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "system:linux"
		systemversion "latest"
		pic "On"
		
		files {
			src_dir .. "x11_init.c",
			src_dir .. "x11_monitor.c",
			src_dir .. "x11_platform.c",
			src_dir .. "x11_window.c",

			src_dir .. "xkb_unicode.c",

			src_dir .. "posix_time.c",
			src_dir .. "posix_thread.c",

			src_dir .. "egl_context.c",
			src_dir .. "glx_context.c",
			src_dir .. "osmesa_context.c",

			src_dir .. "linux_joystick.c"
		}

		defines {
			"_GLFW_X11"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter {}  -- reset filter

	
	success ( project_name )