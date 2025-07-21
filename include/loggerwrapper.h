#pragma once

#include <vector>
#include <memory>
#include <string>
#include "logger.h"
#include "quill/bundled/fmt/core.h"

class LoggerWrapper {
public:
    LoggerWrapper();
    LoggerWrapper(uint8_t shard_count, const std::string& log_file_prefix, size_t max_file_size = 0);
    ~LoggerWrapper();

    // Clean interface methods
    void info(uint8_t shard_id, const std::string& message);
    void warn(uint8_t shard_id, const std::string& message);
    void error(uint8_t shard_id, const std::string& message);
    void debug(uint8_t shard_id, const std::string& message);

    // Ultra-fast logging: fmt-style, variadic, user does not need to include fmt/quill
    template<typename... Args>
    void info_fast(uint8_t shard_id, const std::string& fmt, Args&&... args) {
        if (shard_id < shard_loggers_.size()) {
            shard_loggers_[shard_id]->info_fast(fmt, std::forward<Args>(args)...);
        }
    }
    template<typename... Args>
    void warn_fast(uint8_t shard_id, const std::string& fmt, Args&&... args) {
        if (shard_id < shard_loggers_.size()) {
            shard_loggers_[shard_id]->warn_fast(fmt, std::forward<Args>(args)...);
        }
    }
    template<typename... Args>
    void error_fast(uint8_t shard_id, const std::string& fmt, Args&&... args) {
        if (shard_id < shard_loggers_.size()) {
            shard_loggers_[shard_id]->error_fast(fmt, std::forward<Args>(args)...);
        }
    }
    template<typename... Args>
    void debug_fast(uint8_t shard_id, const std::string& fmt, Args&&... args) {
        if (shard_id < shard_loggers_.size()) {
            shard_loggers_[shard_id]->debug_fast(fmt, std::forward<Args>(args)...);
        }
    }

    // Log level control methods
    void set_log_level(uint8_t shard_id, LogLevel level);
    void set_log_level_all(LogLevel level);
    LogLevel get_log_level(uint8_t shard_id) const;

private:
    std::vector<std::unique_ptr<Logger>> shard_loggers_;
};

