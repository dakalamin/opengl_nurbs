#pragma once

// Only there could CW_ENABLE_ASSERTS be defenied 
// So we make sure it has be taken into account
// before implementing CW_ASSERT functionality
#include "Core/Base.h"


#ifdef CW_ENABLE_ASSERTS
	#include <cassert>
	#include <filesystem>
	#include <iostream>

	#define CW_INTIMPL_ASSERT(expr, message, file, line) { \
	std::cerr << "Assert '" << CW_STRINGIFY_MACRO(expr) << "' failed at\n" << file << ": " << line << '\n'; \
	std::cerr << '\n' << message << '\n'; \
	CW_DEBUGBREAK(); }

	#define CW_ASSERT(expr, message) \
	if (!(expr)) CW_INTIMPL_ASSERT(expr, message, std::filesystem::path(__FILE__).filename().string(), __LINE__);

#else
	#define CW_ASSERT(...)
#endif
