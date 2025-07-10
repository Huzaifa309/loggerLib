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

    // Messaging shard methods (shard_id = shard_count)
    void info_msg(const std::string& message);
    void warn_msg(const std::string& message);
    void error_msg(const std::string& message);
    void debug_msg(const std::string& message);

private:
    std::vector<std::unique_ptr<Logger>> shard_loggers_;
};

