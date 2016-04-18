#include "Log.h"
#include "Memory.h"
#include <string.h>
#include <stdarg.h>
#include "Time.h"

namespace Neutrino
{
	static Log *s_pSysLog=0;
	Log *GetSystemLog()
	{
		if(!s_pSysLog)
		{
			s_pSysLog = NEWX Log("Neutrino.log");
		}

		return s_pSysLog;
	}

	////////////////////////////////////////////////

	static const char *s_asTypeNameLUT[]=
	{
		"INF",
		"WRN",
		"ERR",
	};

	Log::Log(const char *_sFilename)
		: m_pFile(0)
	{
		strcpy(m_sFilename, _sFilename);
	}

	Log::~Log()
	{
		CloseLogFile();
	}

	void Log::OpenLogFile()
	{
		if(m_pFile)
			return;

		m_pFile = fopen(m_sFilename, "w");
		WriteHeader();
	}

	void Log::CloseLogFile()
	{
		if(!m_pFile)
			return;
		fclose(m_pFile);
		m_pFile=0;
		m_sFilename[0]='\0';
	}

	void Log::WriteLn(eLogMsgType _nType, const char *_sFmt, ...)
	{
		OpenLogFile();

		if(!m_pFile)
			return;

		// [hh:mm:ss][INF/WRN/ERR] message
		// TO_DO: Add Sytem Time to this buffer...
		// TO_DO: Add framecount to this...

		char sBuff[4096]={'\0'};
		snprintf(sBuff, 8, "%s: ", s_asTypeNameLUT[_nType]);
		Write(sBuff);

		va_list ap;
		va_start(ap, _sFmt);
		{
			vsnprintf(sBuff, 4096-1, _sFmt, ap);
		}
		va_end(ap);

		Write(sBuff);
		Write("\n");
	}

	void Log::WriteHeader()
	{
		if(!m_pFile)
			return;

		char sBuff[4096]={'\0'};
		sprintf(sBuff, "\n\nNeutrino Framework started: %s\n", GetTimeStamp());
		Write(sBuff);
	}

	void Log::Write(const char *_sBuff)
	{
		size_t nBufflen = strlen(_sBuff);
		fwrite(_sBuff, nBufflen, 1, m_pFile);
		fflush(m_pFile);
	}
}
