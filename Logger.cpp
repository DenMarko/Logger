#include "logger.h"
#include <Windows.h>
#include <time.h>

namespace Loger
{
  tm GetCurTime(char date[], size_t len)
  {
    time_t t = time(NULL);
    tm curtime;
    
    localtime_s(&curtime, &t);
    strftime(date, len, "%H:%M:%S", &curtime);
    return curtime;
  }
  
  size_t Format(char buffer[], size_t maxlen, const char *fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    size_t len = vsnprintf_s(buffer, maxlen, _TRUNCATE, fmt, ap);
    va_end(ap);
    
    if(len >= maxlen)
    {
      buffer[maxlen - 1] = '\0';
      return (maxlen - 1);
    } else {
      return len;
    }
  }
  
  size_t FormatArgs(_core::c8 *buffer, size_t maxlength, _core::cc8 *fmt, va_list ap)
	{
		size_t len = vsnprintf_s(buffer, maxlength, _TRUNCATE, fmt, ap);

		if (len >= maxlength)
		{
			buffer[maxlength - 1] = '\0';
			return (maxlength - 1);
		}
		else
		{
			return len;
		}
	}

	Logger::Logger(_core::cc8 *FileName_log, _core::cc8 *FileName_Error)
	{
		_core::c8 buffer[MAX_PATH], data[32];
		FILE *fp = NULL;
		tm curtime = GetCurectTime(data, sizeof(data));

		Format(FileName[0], 128, "%s", FileName_log);
		Format(FileName[1], 128, "%s", FileName_Error);

		GetCurrentDirectory(sizeof(buffer), buffer);
		m_logValue.Log_dir.append(buffer);
		m_logValue.Log_dir += "\\log\\";

		CreateDirectory(m_logValue.Log_dir.c_str(), NULL);

		m_logValue.PathFileLog += m_logValue.Log_dir.c_str();
		m_logValue.PathFileLog += curtime.tm_mday;
		m_logValue.PathFileLog += "_";
		m_logValue.PathFileLog += FileName_log;

		m_logValue.CurDatePrintDayLog = curtime.tm_mday;
		fopen_s(&fp, m_logValue.PathFileLog.c_str(), "a+");
		if(fp)
		{
			m_logValue.PrintDayLog = true;
			fprintf_s(fp, "L [%s]: Log session started (file \"%s\")\n", data, FileName_log);
			fclose(fp);
		}

		fp = NULL;
		m_logValue.PathFileError += m_logValue.Log_dir.c_str();
		m_logValue.PathFileError += curtime.tm_mday;
		m_logValue.PathFileError += "_";
		m_logValue.PathFileError += FileName_Error;

		m_logValue.CurDatePrintDayError = curtime.tm_mday;
		fopen_s(&fp, m_logValue.PathFileError.c_str(), "a+");
		if(fp)
		{
			m_logValue.PrintDayError = true;
			fprintf_s(fp, "L [%s]: Error Log session started (file \"%s\")\n", data, FileName_Error);
			fclose(fp);
		}
	}
	
	Logger::~Logger()
	{
		FILE *fp = NULL;
		_core::c8 data[32];
		
		GetCurectTime(data, sizeof(data));

		fopen_s(&fp, m_logValue.PathFileLog.c_str(), "a+");
		if(fp)
		{
			fprintf_s(fp, "L [%s]: Log session end\n", data);
			fclose(fp);
		}

		fp = NULL;
		fopen_s(&fp, m_logValue.PathFileError.c_str(), "a+");
		if(fp)
		{
			fprintf_s(fp, "L [%s]: Error Log session end\n", data);
			fclose(fp);
		}
	}

	void Logger::Write(LOG WriteLog, _core::cc8 msg[], ...)
	{
		va_list ap;
		va_start(ap, msg);

		switch (WriteLog)
		{
		case Loger::_LOG:
			this->WriteLog(msg, ap);
			break;
		case Loger::_LOG_ERROR:
			this->WriteLogError(msg, ap);
			break;
		default:
			break;
		}
		va_end(ap);
	}

	void Logger::WriteLog(_core::cc8 msg[], va_list ap)
	{
		FILE *fp = NULL;
		_core::c8 date[32], buffer[3072];

		tm curtime = GetCurectTime(date, 32);

		if(m_logValue.CurDatePrintDayLog != curtime.tm_mday)
		{
			m_logValue.PathFileLog = m_logValue.Log_dir.c_str();
			m_logValue.PathFileLog += curtime.tm_mday;
			m_logValue.PathFileLog += "_";
			m_logValue.PathFileLog += FileName[0];
			m_logValue.PrintDayLog = false;
			m_logValue.CurDatePrintDayLog = curtime.tm_mday;
		}

		fopen_s(&fp, m_logValue.PathFileLog.c_str(), "a+");
		if(!m_logValue.PrintDayLog)
		{
			m_logValue.PrintDayLog = true;
			fprintf_s(fp, "L [%s]: Log session started (file \"%s\")\n", date, FileName[0]);
		}

		FormatArgs(buffer, sizeof(buffer), msg, ap);

		fprintf_s(fp, "L [%s]: %s\n", date, buffer);
		fclose(fp);
	}

	void Logger::WriteLogError(_core::cc8 msg[], va_list ap)
	{
		FILE *fp = NULL;
		_core::c8 date[32], buffer[3072];

		tm curtime = GetCurectTime(date, 32);

		if(m_logValue.CurDatePrintDayError != curtime.tm_mday)
		{
			m_logValue.PathFileError = m_logValue.Log_dir.c_str();
			m_logValue.PathFileError += curtime.tm_mday;
			m_logValue.PathFileError += "_";
			m_logValue.PathFileError += FileName[1];
			m_logValue.PrintDayError = false;
			m_logValue.CurDatePrintDayError = curtime.tm_mday;
		}

		fopen_s(&fp, m_logValue.PathFileError.c_str(), "a+");
		if(!m_logValue.PrintDayError)
		{
			m_logValue.PrintDayError = true;
			fprintf_s(fp, "L [%s]: Log session started (file \"%s\")\n", date, FileName[1]);
		}

		FormatArgs(buffer, sizeof(buffer), msg, ap);

		fprintf_s(fp, "L [%s]: %s\n", date, buffer);
		fclose(fp);
	}

}
