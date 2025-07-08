#pragma once

#include <vector>
#include <memory>
#include <string>
#include "logger.hpp"


class LoggerWrapper {
public:
    LoggerWrapper() = default;

    LoggerWrapper(uint8_t shard_count, const std::string& log_file_prefix)
    {
        for (uint8_t shard_id = 0; shard_id < shard_count; ++shard_id) {
            std::string shard_log_file = log_file_prefix + "_shard_" + std::to_string(shard_id) + ".log";
            shard_loggers_.emplace_back(std::make_unique<Logger>(shard_log_file));
        }
        // One extra for messaging
        shard_loggers_.emplace_back(std::make_unique<Logger>(log_file_prefix + "_messaging.log"));
    }

    // Get the quill::Logger* for a given shard
   inline quill::Logger* get_logger(uint8_t shard_id) const {
        return shard_loggers_[shard_id]->get_logger();
    }

private:
    std::vector<std::unique_ptr<Logger>> shard_loggers_;
};
