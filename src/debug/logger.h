#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

class Logger
{
    Logger();
    ~Logger();

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    enum class LoggingMode
    {
        Disabled,
        Console
    };

    static Logger& GetInstance();

    void SetLoggingMode(LoggingMode loggingMode);

    template<typename T>
    void Log(const T& value) const
    {
        if (m_loggingMode == LoggingMode::Disabled) return;
        std::cout << value << std::endl;
    }

    template<typename T>
    void Warn(const T& value) const
    {
        if (m_loggingMode == LoggingMode::Disabled) return;
        std::cout << "Warning: " << value << std::endl;
    }

    template<typename T>
    void Error(const T& value) const
    {
        if (m_loggingMode == LoggingMode::Disabled) return;
        std::cerr << "Error: " << value << std::endl;
    }

    template<typename T>
    static std::string DecmialToHex(T value)
    {
        static_assert(std::is_integral_v<T>, "DecimalToHex can only be used with integral types.");

        const std::string hexChars = "0123456789ABCDEF";
        int hexCharCount = sizeof(value) * 2;
        std::vector<char> result(hexCharCount + 3);
        result[0] = '0';
        result[1] = 'x';

        for (int i = hexCharCount + 1; i > 1; i--, value >>= 4)
        {
            result[i] = hexChars[value & 0xF];
        }

        return std::string(result.begin(), result.end());
    }

private:
    LoggingMode m_loggingMode;
};