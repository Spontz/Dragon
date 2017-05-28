/*
 *  spzmessagebox.c : Window Management
 *  Inferno
 *
 *
 */

#include "spzmessagebox.h"

#ifdef WIN32
	#include <windows.h>
#endif

#include "../core/dkernel.h"

#if defined( __APPLE__ ) && defined( __MACH__ )
	#include <Carbon/Carbon.h>
	#include <AGL/agl.h>
#endif

#if _WIN64 || __cplusplus_cli
	#define SL_DBG_BREAK __debugbreak();
#elif __linux__
	#define SL_DBG_BREAK asm("int $3");
	#define __FUNCTION__ __func__
#elif __APPLE__
	#define SL_DBG_BREAK __builtin_trap();
#elif WIN32
	#define SL_DBG_BREAK __asm { int 3 }
#else
	// emscripten
	#define SL_DBG_BREAK abort();
#endif

void SpzMessageBox(const char* pTitle, const char* pMmessage)
	{
	static char* pLastMessage;
		
	if (pLastMessage != NULL)
		if (strcmp(pLastMessage, pMmessage) == 0)
			return; // Hack: The message has been already displayed, so don't show it again (return the function)
		
	#if defined( __APPLE__ ) && defined( __MACH__ )
		CFStringRef titleStr = CFStringCreateWithCString(NULL, title, kCFStringEncodingUTF8);
		CFStringRef messageStr = CFStringCreateWithCString(NULL, message, kCFStringEncodingUTF8);
		CFUserNotificationDisplayNotice (0, kCFUserNotificationCautionAlertLevel, NULL, NULL, NULL, titleStr, messageStr, CFSTR("OK"));
		CFRelease(titleStr);
		CFRelease(messageStr);
	#elif WIN32
		MessageBoxA( 0, pMmessage, pTitle, 0 ); // Messagebox with an OK button
	#else
		#error Unsupported platform
	#endif

	SL_DBG_BREAK;

	// Save the message
	free(pLastMessage);
	pLastMessage = calloc(strlen(pMmessage) + 1, sizeof(char));
	pLastMessage = SDL_strdup(pMmessage);
	}
