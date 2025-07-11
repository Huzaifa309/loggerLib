#pragma once

#include <string>
#include <type_traits>
#include <cstring>
#include <memory>

class LoggerImpl; // PIMPL forward declaration

class Logger {
public:
    Logger();
    explicit Logger(const std::string& log_file, size_t max_file_size = 0);
    ~Logger();

    void initialize(const std::string& log_file, size_t max_file_size = 0);
    
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);

    // Ultra-fast formatting methods (no stringstream overhead)
    template<typename... Args>
    void info_fast(Args&&... args) {
        this->log_fast("info", std::forward<Args>(args)...);
    }
    template<typename... Args>
    void warn_fast(Args&&... args) {
        this->log_fast("warn", std::forward<Args>(args)...);
    }
    template<typename... Args>
    void error_fast(Args&&... args) {
        this->log_fast("error", std::forward<Args>(args)...);
    }
    template<typename... Args>
    void debug_fast(Args&&... args) {
        this->log_fast("debug", std::forward<Args>(args)...);
    }

private:
    std::unique_ptr<LoggerImpl> impl_;

    template<typename... Args>
    void log_fast(const char* level, Args&&... args) {
        this->log_fast_impl(level, fast_concat(std::forward<Args>(args)...));
    }
    void log_fast_impl(const char* level, const std::string& msg);

    template<typename... Args>
    std::string fast_concat(Args&&... args) {
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
};

