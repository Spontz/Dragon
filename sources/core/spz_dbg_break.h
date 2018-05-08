#pragma once

#if _WIN64 || __cplusplus_cli
	#define SPZ_DBG_BREAK __debugbreak();
#elif __linux__
	#define SPZ_DBG_BREAK asm("int $3");
	#define __FUNCTION__ __func__
#elif __APPLE__
	#define SPZ_DBG_BREAK __builtin_trap();
#elif WIN32
	#define SPZ_DBG_BREAK __asm { int 3 }
#else
	#error Unsupported platform.
#endif