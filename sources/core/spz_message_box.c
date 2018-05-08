/*
 *  spzmessagebox.c : Window Management
 *  Dragon
 *
 *
 */

//#include "spz_message_box.h"

#ifdef WIN32
	#include <windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
	#include <Carbon/Carbon.h>
	#include <AGL/agl.h>
#endif

//#include "../core/dkernel.h"

#include "spz_dbg_break.h"

void SpzMessageBox(const char* pTitle, const char* pMessage) {
	static char* pLastMessage; // Hack: Not released at shutdown.

	if (pLastMessage != NULL)
		if (strcmp(pLastMessage, pMessage) == 0)
			return; // Ignore reprints

	#if defined(__APPLE__) && defined(__MACH__)
		CFStringRef titleStr = CFStringCreateWithCString(NULL, title, kCFStringEncodingUTF8);
		CFStringRef messageStr = CFStringCreateWithCString(NULL, message, kCFStringEncodingUTF8);
		CFUserNotificationDisplayNotice (0, kCFUserNotificationCautionAlertLevel, NULL, NULL, NULL, titleStr, messageStr, CFSTR("OK"));
		CFRelease(titleStr);
		CFRelease(messageStr);
	#elif WIN32
		MessageBoxA(0, pMessage, pTitle, 0);
	#elif __linux__
		#error Not implemented.
	#else
		#error Unsupported platform.
	#endif

	SPZ_DBG_BREAK;

	// Save the message
 	free(pLastMessage);
	pLastMessage = _strdup(pMessage);
}
