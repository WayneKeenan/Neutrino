#include "dmesg.h"

#if defined(DEBUG)

FILE *pLogfile = stderr; 

void dmesg_SetOutputStream(FILE *pFile)
{
	if(NULL != pFile)
	{
		pLogfile = pFile;
	}
}

void dmesg(const char* msg, ...)
{
	va_list args, argsnull;	
	va_start(args, msg); 
	vfprintf(pLogfile, "DEBUG :: ", argsnull);
	vfprintf(pLogfile, msg, args); 
	va_end(args); 
};
#endif
