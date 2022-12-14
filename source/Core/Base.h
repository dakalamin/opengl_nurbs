#pragma once

#include <memory>
#include <iostream>

#include "Core/PlatformDetector.h"


#ifdef CW_DEBUG // (in premake.lua)
	// -- (in Core/PlatformDetector.h) --
	#if defined(CW_PLATFORM_WINDOWS)           // Windows x64
		#define CW_DEBUGBREAK() __debugbreak()
	#elif defined(CW_PLATFORM_LINUX)           // Linux
		#include <signal.h>
		#define CW_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif

	#define CW_ENABLE_ASSERTS
#elif CW_RELEASE // (in premake.lua)
	#define CW_DEBUGBREAK()
#else
	#error "Unknown configuration, expected CW_DEBUG/CW_RELEASE!"
#endif

#include "Debug/Assert.h"


template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}


template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}
