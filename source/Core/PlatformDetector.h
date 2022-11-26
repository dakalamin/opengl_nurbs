// https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
// https://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive

#if defined(WIN32)  || defined(__WIN32__) || \
	defined(_WIN32) || defined(__NT__)           // Windows x64/x86
	#ifdef _WIN64                                // Windows x64
		#define CW_PLATFORM_WINDOWS
	#else                                        // Windows x86
		#error "x86 Builds are not supported!"
	#endif

#elif defined(__APPLE__) || defined(__MACH__)    // iOS/MacOS
	#include <TargetConditionals.h>
	// TARGET_OS_MAC exists on all the platforms
	// so we must check all of them (in this order)
	// to ensure that we're running on MAC
	// and not some other Apple platform
	#if TARGET_IPHONE_SIMULATOR == 1             // iOS Simulatior
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1	                 // iOS
		#define CW_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1                     // MacOS
		#define CW_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif

// We also have to check ANDROID before LINUX
// since android is based on the linux kernel
// it has __linux__ defined
#elif defined(__ANDROID__)                       // Android
	#define CW_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)                         // Linux...
	#define CW_PLATFORM_LINUX                  // Debian, Ubuntu, Gentoo, Fedora, 
                                                 // openSUSE, RedHat, Centos & other
	#error "Linux is not supported!"
#else                                            // Unknown compiler/platform
	#error "Unknown platform!"
#endif
