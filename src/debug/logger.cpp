#include "logger.h"

Logger::Logger()
{
    m_loggingMode = LoggingModes::Console;
}

Logger::~Logger()
{
}

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

void Logger::SetLoggingMode(LoggingModes loggingMode)
{
    m_loggingMode = loggingMode;
}
