#include "logger.h"

Logger::Logger()
{
    m_loggingMode = LoggingMode::Console;
}

Logger::~Logger()
{
}

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

void Logger::SetLoggingMode(LoggingMode loggingMode)
{
    m_loggingMode = loggingMode;
}
