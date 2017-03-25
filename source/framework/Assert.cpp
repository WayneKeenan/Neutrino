#include "Assert.h"
#include "Log.h"
#include <stdio.h>

int ShowAssert(const char * _sMsg, ...)
{
	char sBuffer[1024] = {'\0'};

	va_list marker;
	va_start (marker, _sMsg);
	{
		vsnprintf(sBuffer, 1024, _sMsg, marker);
	}
	va_end (marker);

	SYSLOG(Neutrino::LOG_ERROR, sBuffer);

	// return...
	//	0 to always ignore,
	//	1 to break,
	//	2 to skip once
	return 1;
}
