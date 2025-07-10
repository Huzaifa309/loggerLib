#pragma once

#include <string>
#include <type_traits>
#include <cstring>
#include "quill/Logger.h"
#include "quill/LogMacros.h"

class Logger {
public:
    Logger();
    explicit Logger(const std::string& log_file);
    ~Logger();

    void initialize(const std::string& log_file);
    
    // Clean interface methods that don't expose Quill types
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);

    // Ultra-fast formatting methods (no stringstream overhead)
    template<typename... Args>
    void info_fast(Args&&... args);
    
    template<typename... Args>
    void warn_fast(Args&&... args);
    
    template<typename... Args>
    void error_fast(Args&&... args);
    
    template<typename... Args>
    void debug_fast(Args&&... args);

private:
    void* logger_ = nullptr; // Hide Quill logger completely
    
    // Fast string concatenation helper
    template<typename... Args>
    std::string fast_concat(Args&&... args);
};

// === Template Implementations ===

template<typename... Args>
void Logger::info_fast(Args&&... args) {
    if (logger_) {
        auto* quill_logger = static_cast<quill::Logger*>(logger_);
        std::string message = fast_concat(std::forward<Args>(args)...);
        LOG_INFO(quill_logger, "{}", message);
    }
}

template<typename... Args>
void Logger::warn_fast(Args&&... args) {
    if (logger_) {
        auto* quill_logger = static_cast<quill::Logger*>(logger_);
        std::string message = fast_concat(std::forward<Args>(args)...);
        LOG_WARNING(quill_logger, "{}", message);
    }
}

template<typename... Args>
void Logger::error_fast(Args&&... args) {
    if (logger_) {
        auto* quill_logger = static_cast<quill::Logger*>(logger_);
        std::string message = fast_concat(std::forward<Args>(args)...);
        LOG_ERROR(quill_logger, "{}", message);
    }
}

template<typename... Args>
void Logger::debug_fast(Args&&... args) {
    if (logger_) {
        auto* quill_logger = static_cast<quill::Logger*>(logger_);
        std::string message = fast_concat(std::forward<Args>(args)...);
        LOG_DEBUG(quill_logger, "{}", message);
    }
}

template<typename... Args>
std::string Logger::fast_concat(Args&&... args) {
    size_t total_size = 0;
    auto calc_size = [&total_size](const auto& arg) {
        if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
            total_size += arg.size();
        } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, const char*>) {
            total_size += strlen(arg);
        } else if constexpr (std::is_integral_v<std::decay_t<decltype(arg)>>) {
            total_size += 20;
        } else {
            total_size += 32;
        }
    };
    (calc_size(args), ...);
    std::string result;
    result.reserve(total_size);
    auto append = [&result](const auto& arg) {
        if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
            result += arg;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, const char*>) {
            result += arg;
        } else if constexpr (std::is_integral_v<std::decay_t<decltype(arg)>>) {
            result += std::to_string(arg);
        } else {
            result += std::to_string(arg);
        }
    };
    (append(args), ...);
    return result;
}

