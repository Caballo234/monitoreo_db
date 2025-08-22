#pragma once
#include <string>
#include <mutex>
#include <fstream>

enum class LogLevel { info, warning, error };

class Logger {
public:
    static Logger& getInstance();
    void log(LogLevel level, const std::string& message);
private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream log_file;
    std::mutex log_mutex;

    std::string levelToString(LogLevel level);
};