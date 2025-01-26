#pragma once

#include <iostream>
#include <string>
#include <iomanip>

class Logger
{
    Logger();
    ~Logger();

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    enum class LoggingModes
    {
        Disabled,
        Console
    };

    static Logger& GetInstance();

    void SetLoggingMode(LoggingModes loggingMode);

    template<typename T>
    void Log(const T& value)
    {
        if (m_loggingMode == LoggingModes::Disabled) return;
        std::cout << value << std::endl;
    }

    template<typename T>
    void Warn(const T& value)
    {
        if (m_loggingMode == LoggingModes::Disabled) return;
        std::cout << "Warning: " << value << std::endl;
    }

    template<typename T>
    void Error(const T& value)
    {
        if (m_loggingMode == LoggingModes::Disabled) return;
        std::cerr << "Error: " << value << std::endl;
    }

    template<typename T>
    std::string DecmialToHex(T value)
    {
        // TODO: Implement function
        return "";
    }

private:
    LoggingModes m_loggingMode;
};