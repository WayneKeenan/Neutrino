#include <stdarg.h>
#include <stdio.h>

#pragma once
#if defined(DEBUG)
	void dmesg(const char* msg, ...);
#else
	#define dmesg(msg, ...)	{ do {} while(0); }
#endif
