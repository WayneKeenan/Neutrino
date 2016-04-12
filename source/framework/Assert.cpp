#include "Assert.h"
#include "Log.h"
#include <stdio.h>

#if defined _DEBUG

int ShowAssert(const char * _sMsg, ...)
{
	char sBuffer[256] = {'\0'};

	va_list marker;
	va_start (marker, _sMsg);
	{
		vsnprintf_s(sBuffer, _TRUNCATE, _sMsg, marker);
	}
	va_end (marker);

	SYSLOG(Framework::LOG_ERROR, sBuffer);

	// return...
	//	0 to always ignore,
	//	1 to break,
	//	2 to skip once
	return 1;
}

#endif //defined _DEBUG
