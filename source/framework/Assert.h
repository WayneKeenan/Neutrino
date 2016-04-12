#pragma once
#include <stdarg.h>
#include <signal.h>
#include <stdio.h>

#define STRINGIZE(X) #X
// Convert an expanded macro to a string
#define EXP_STRINGIZE(X)	STRINGIZE(X)

#define __FILE_LINE__ __FILE__ "(" EXP_STRINGIZE(__LINE__) ") : "



#if defined DEBUG

	#if defined WIN32
	#define ASSERT(exp, ...)											\
	do																	\
	{																	\
		static int s_nAssert = 0x1;										\
		if (s_nAssert && !(exp))										\
		{																\
			s_nAssert = ShowAssert (									\
			__FILE_LINE__ "ASSERT("#exp"): " ##__VA_ARGS__, #exp);		\
																		\
			if (s_nAssert == 1)											\
				__debugbreak();											\
		}																\
	}while(0)
	#elif defined __linux__
	#define ASSERT(exp, ...)											\
	do																	\
	{																	\
		static int s_nAssert = 0x1;										\
		if (s_nAssert && !(exp))										\
		{																\
			s_nAssert = ShowAssert (									\
			__FILE_LINE__ "ASSERT("#exp"): " __VA_ARGS__, #exp);		\
																		\
			if (s_nAssert == 1)											\
				raise(SIGTRAP);											\
		}																\
	}while(0)
	#endif //defined WIN32

	int ShowAssert(const char * _sMsg, ...);

#else

#define ASSERT(exp, ...)	do{}while(0)

#endif // defined _DEBUG
