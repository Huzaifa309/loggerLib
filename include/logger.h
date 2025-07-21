#pragma once

#include <string>
#include <type_traits>
#include <cstring>
#include <memory>
#include "quill/bundled/fmt/core.h"

class LoggerImpl; // PIMPL forward declaration

// Log levels enum (clean interface, no Quill exposure)
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    CRITICAL = 5
};

class Logger {
public:
    Logger();
    explicit Logger(const std::string& log_file, size_t max_file_size = 0);
    explicit Logger(const std::string& log_file, LogLevel level, size_t max_file_size = 0);
    ~Logger();

    void initialize(const std::string& log_file, size_t max_file_size = 0);
    void initialize(const std::string& log_file, LogLevel level, size_t max_file_size = 0);
    
    // Log level configuration
    void set_log_level(LogLevel level);
    LogLevel get_log_level() const;
    
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);

    // Ultra-fast logging: fmt-style, variadic, user does not need to include fmt/quill
    template<typename... Args>
    void info_fast(const std::string& fmt, Args&&... args) {
        this->log_fast("info", fmtquill::format(fmt, std::forward<Args>(args)...));
    }
    template<typename... Args>
    void warn_fast(const std::string& fmt, Args&&... args) {
        this->log_fast("warn", fmtquill::format(fmt, std::forward<Args>(args)...));
    }
    template<typename... Args>
    void error_fast(const std::string& fmt, Args&&... args) {
        this->log_fast("error", fmtquill::format(fmt, std::forward<Args>(args)...));
    }
    template<typename... Args>
    void debug_fast(const std::string& fmt, Args&&... args) {
        this->log_fast("debug", fmtquill::format(fmt, std::forward<Args>(args)...));
    }

private:
    std::unique_ptr<LoggerImpl> impl_;

    void log_fast(const char* level, const std::string& msg);
    void log_fast_impl(const char* level, const std::string& msg);
};

