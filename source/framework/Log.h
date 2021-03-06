#pragma once
#include <stdio.h>

#define FOUR_BYTE_HEX " %#010x "

//#if defined DEBUG
#define ENABLE_LOGGING
//#endif //defined DEBUG

#if defined ENABLE_LOGGING
#define SYSLOG(_nLogType, _sMsg, ...) \
	do{ Neutrino::GetSystemLog()->WriteLn(_nLogType, _sMsg, ##__VA_ARGS__);}while(0)
#else
#define SYSLOG(_nLogType, _sMsg, ...) do{}while(0)
#endif
#define LOG_INFO(_sMsg, ...) SYSLOG(Neutrino::LOG_INFO, _sMsg, ##__VA_ARGS__)
#define LOG_WARNING(_sMsg, ...) SYSLOG(Neutrino::LOG_WARNING, _sMsg, ##__VA_ARGS__)
#define LOG_ERROR(_sMsg, ...) SYSLOG(Neutrino::LOG_ERROR, _sMsg, ##__VA_ARGS__)

namespace Neutrino
{
#ifdef _WIN32
	#define ANSI_COLOUR_RED     ""
	#define ANSI_COLOUR_GREEN   ""
	#define ANSI_COLOUR_YELLOW  ""
	#define ANSI_COLOUR_BLUE    ""
	#define ANSI_COLOUR_MAGENTA ""
	#define ANSI_COLOUR_CYAN    ""
	#define ANSI_COLOUR_RESET   ""
#else
	#define ANSI_COLOUR_RED     "\x1b[31m"
	#define ANSI_COLOUR_GREEN   "\x1b[32m"
	#define ANSI_COLOUR_YELLOW  "\x1b[33m"
	#define ANSI_COLOUR_BLUE    "\x1b[34m"
	#define ANSI_COLOUR_MAGENTA "\x1b[35m"
	#define ANSI_COLOUR_CYAN    "\x1b[36m"
	#define ANSI_COLOUR_RESET   "\x1b[0m"
#endif

	enum eLogMsgType
	{
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR,
	};

	class Log
	{
		FILE *m_pFile;
		char m_sFilename[4096];
		
	public:

		Log(const char *_sFilename);
		~Log();

		void WriteLn(eLogMsgType _nType, const char *_sMsg, ...);

	private:

		void OpenLogFile();
		void CloseLogFile();
		void WriteHeader();
		void Write(const char *_sBuff, bool bLog=true);
	};

	Log *GetSystemLog();
}
