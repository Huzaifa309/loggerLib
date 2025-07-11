#include "loggerwrapper.h"

LoggerWrapper::LoggerWrapper() = default;

LoggerWrapper::LoggerWrapper(uint8_t shard_count, const std::string& log_file_prefix, size_t max_file_size)
{
    for (uint8_t shard_id = 0; shard_id < shard_count; ++shard_id) {
        std::string shard_log_file = log_file_prefix + "_shard_" + std::to_string(shard_id) + ".log";
        shard_loggers_.emplace_back(std::make_unique<Logger>(shard_log_file, max_file_size));
    }
}

LoggerWrapper::~LoggerWrapper() = default;

// Clean interface implementations
void LoggerWrapper::info(uint8_t shard_id, const std::string& message) {
    if (shard_id < shard_loggers_.size()) {
        shard_loggers_[shard_id]->info(message);
    }
}

void LoggerWrapper::warn(uint8_t shard_id, const std::string& message) {
    if (shard_id < shard_loggers_.size()) {
        shard_loggers_[shard_id]->warn(message);
    }
}

void LoggerWrapper::error(uint8_t shard_id, const std::string& message) {
    if (shard_id < shard_loggers_.size()) {
        shard_loggers_[shard_id]->error(message);
    }
}

void LoggerWrapper::debug(uint8_t shard_id, const std::string& message) {
    if (shard_id < shard_loggers_.size()) {
        shard_loggers_[shard_id]->debug(message);
    }
}


