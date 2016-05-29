#ifndef HIDER_FILE_LOGGER
#define HIDER_FILE_LOGGER

#include "Types.h"
#include "string.h"

namespace Loger
{
	struct LogValue {
		_core::stringc Log_dir;
		_core::stringc PathFileLog;
		_core::stringc PathFileError;
		bool PrintDayLog;
		bool PrintDayError;
		int CurDatePrintDayLog;
		int CurDatePrintDayError;

		LogValue()
		{
			PrintDayError = false;
			PrintDayLog = false;

			CurDatePrintDayLog = 0;
			CurDatePrintDayError = 0;
		}
	};
	
	enum LOG {
		_LOG = 0,
		_LOG_ERROR,
	};

	class Logger
	{
	public:
		Logger(_core::cc8 *FileName_log, _core::cc8 *FileName_Error);
		~Logger();

		void Write(LOG WriteLog, _core::cc8 msg[], ...);
	private:
		void WriteLog(_core::cc8 msg[], va_list ap);
		void WriteLogError(_core::cc8 msg[], va_list ap);

		LogValue m_logValue;
		_core::c8 FileName[2][128];
	};
};

#endif // !HIDER_FILE_LOGGER
