#pragma once

#include <vector>
#include <memory>
#include <string>
#include "logger.h"

class LoggerWrapper {
public:
    LoggerWrapper();
    LoggerWrapper(uint8_t shard_count, const std::string& log_file_prefix);
    ~LoggerWrapper();

    // Clean interface methods
    void info(uint8_t shard_id, const std::string& message);
    void warn(uint8_t shard_id, const std::string& message);
    void error(uint8_t shard_id, const std::string& message);
    void debug(uint8_t shard_id, const std::string& message);

    // Ultra-fast formatting methods (no stringstream overhead)
    template<typename... Args>
    void info_fast(uint8_t shard_id, Args&&... args);
    
    template<typename... Args>
    void warn_fast(uint8_t shard_id, Args&&... args);
    
    template<typename... Args>
    void error_fast(uint8_t shard_id, Args&&... args);
    
    template<typename... Args>
    void debug_fast(uint8_t shard_id, Args&&... args);

    // Messaging shard methods (shard_id = shard_count)
    void info_msg(const std::string& message);
    void warn_msg(const std::string& message);
    void error_msg(const std::string& message);
    void debug_msg(const std::string& message);

    // Ultra-fast messaging methods
    template<typename... Args>
    void info_msg_fast(Args&&... args);
    
    template<typename... Args>
    void warn_msg_fast(Args&&... args);
    
    template<typename... Args>
    void error_msg_fast(Args&&... args);
    
    template<typename... Args>
    void debug_msg_fast(Args&&... args);

private:
    std::vector<std::unique_ptr<Logger>> shard_loggers_;
};

// === Template Implementations ===

template<typename... Args>
void LoggerWrapper::info_fast(uint8_t shard_id, Args&&... args) {
    if (shard_id < shard_loggers_.size()) {
        shard_loggers_[shard_id]->info_fast(std::forward<Args>(args)...);
    }
}

template<typename... Args>
void LoggerWrapper::warn_fast(uint8_t shard_id, Args&&... args) {
    if (shard_id < shard_loggers_.size()) {
        shard_loggers_[shard_id]->warn_fast(std::forward<Args>(args)...);
    }
}

template<typename... Args>
void LoggerWrapper::error_fast(uint8_t shard_id, Args&&... args) {
    if (shard_id < shard_loggers_.size()) {
        shard_loggers_[shard_id]->error_fast(std::forward<Args>(args)...);
    }
}

template<typename... Args>
void LoggerWrapper::debug_fast(uint8_t shard_id, Args&&... args) {
    if (shard_id < shard_loggers_.size()) {
        shard_loggers_[shard_id]->debug_fast(std::forward<Args>(args)...);
    }
}

template<typename... Args>
void LoggerWrapper::info_msg_fast(Args&&... args) {
    if (!shard_loggers_.empty()) {
        shard_loggers_.back()->info_fast(std::forward<Args>(args)...);
    }
}

template<typename... Args>
void LoggerWrapper::warn_msg_fast(Args&&... args) {
    if (!shard_loggers_.empty()) {
        shard_loggers_.back()->warn_fast(std::forward<Args>(args)...);
    }
}

template<typename... Args>
void LoggerWrapper::error_msg_fast(Args&&... args) {
    if (!shard_loggers_.empty()) {
        shard_loggers_.back()->error_fast(std::forward<Args>(args)...);
    }
}

template<typename... Args>
void LoggerWrapper::debug_msg_fast(Args&&... args) {
    if (!shard_loggers_.empty()) {
        shard_loggers_.back()->debug_fast(std::forward<Args>(args)...);
    }
}

