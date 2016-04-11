#include "dmesg.h"

#if defined(DEBUG)
void dmesg(const char* msg, ...)
{
	va_list args;	
	va_start(args, msg); 
	vfprintf(stderr, msg, args); 
	va_end(args); 
};
#endif
