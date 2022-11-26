-- dependencies/glfw.lua
-- GLFW LIB PREMAKE --


project "GLFW"
	location "glfw"

	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir ( "%{prj.location}/bin/"       .. outputdir )
	objdir    ( "%{prj.location}/bin_inter/" .. outputdir )


	local srcdir = "glfw/src/"
	local incdir = "glfw/include/GLFW/"

	includedirs { "glfw/include" }

	files {
		incdir .. "glfw3.h",
		incdir .. "glfw3native.h",
		
		srcdir .. "context.c",
		srcdir .. "init.c",
		srcdir .. "input.c",
		srcdir .. "internal.h",
		srcdir .. "monitor.c",
		srcdir .. "platform.c",
		srcdir .. "glfw_config.h",
		srcdir .. "vulkan.c",  -- can't remove this, turns out it's used by some "win_**.c"
		srcdir .. "window.c",

		srcdir .. "null_init.c",
		srcdir .. "null_joystick.c",
		srcdir .. "null_monitor.c",
		srcdir .. "null_platform.h",
		srcdir .. "null_window.c"		
	}


	filter "system:linux"
		pic "On"

		systemversion "latest"
		
		files {
			srcdir .. "x11_init.c",
			srcdir .. "x11_monitor.c",
			srcdir .. "x11_window.c",
			srcdir .. "xkb_unicode.c",
			srcdir .. "posix_time.c",
			srcdir .. "posix_thread.c",
			srcdir .. "glx_context.c",
			srcdir .. "egl_context.c",
			srcdir .. "osmesa_context.c",
			srcdir .. "linux_joystick.c"
		}

		defines {
			"_GLFW_X11"
		}

	filter "system:windows"
		systemversion "latest"

		files {
			srcdir .. "win32_init.c",
			srcdir .. "win32_joystick.c",
			srcdir .. "win32_module.c",
			srcdir .. "win32_monitor.c",
			srcdir .. "win32_platform.h",
			srcdir .. "win32_thread.c",
			srcdir .. "win32_time.c",
			srcdir .. "win32_window.c",
			srcdir .. "wgl_context.c",
			srcdir .. "egl_context.c",
			srcdir .. "osmesa_context.c"
		}

		defines { 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
