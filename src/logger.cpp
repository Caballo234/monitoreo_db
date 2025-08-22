#include "logger.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <ctime>

// src/logger.cpp
Logger::Logger() {
    // Simplemente quita la ruta del sistema. 
    // Esto creará el archivo en el mismo directorio donde se ejecuta el programa.
    log_file.open("monitor_service.log", std::ios::out | std::ios::app);
    if (!log_file.is_open()) {
        std::cerr << "FATAL: Could not open log file monitor_service.log" << std::endl;
    }
}

Logger::~Logger() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::info:    return "INFO";
        case LogLevel::warning: return "WARNING";
        case LogLevel::error:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf{};
    std::stringstream ss;
    ss << std::put_time(gmtime_r(&in_time_t, &tm_buf), "%Y-%m-%dT%H:%M:%SZ");

    nlohmann::json log_entry;
    log_entry["timestamp"] = ss.str();
    log_entry["level"] = levelToString(level);
    log_entry["message"] = message;

    std::lock_guard<std::mutex> lock(log_mutex);
    if (log_file.is_open()) {
        log_file << log_entry.dump() << std::endl;
    } else {
        std::cerr << log_entry.dump() << std::endl;
    }
}