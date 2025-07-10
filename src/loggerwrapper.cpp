#include "loggerwrapper.h"

LoggerWrapper::LoggerWrapper() = default;

LoggerWrapper::LoggerWrapper(uint8_t shard_count, const std::string& log_file_prefix)
{
    for (uint8_t shard_id = 0; shard_id < shard_count; ++shard_id) {
        std::string shard_log_file = log_file_prefix + "_shard_" + std::to_string(shard_id) + ".log";
        shard_loggers_.emplace_back(std::make_unique<Logger>(shard_log_file));
    }
    // One extra for messaging
    shard_loggers_.emplace_back(std::make_unique<Logger>(log_file_prefix + "_messaging.log"));
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

// Messaging shard methods
void LoggerWrapper::info_msg(const std::string& message) {
    if (!shard_loggers_.empty()) {
        shard_loggers_.back()->info(message);
    }
}

void LoggerWrapper::warn_msg(const std::string& message) {
    if (!shard_loggers_.empty()) {
        shard_loggers_.back()->warn(message);
    }
}

void LoggerWrapper::error_msg(const std::string& message) {
    if (!shard_loggers_.empty()) {
        shard_loggers_.back()->error(message);
    }
}

void LoggerWrapper::debug_msg(const std::string& message) {
    if (!shard_loggers_.empty()) {
        shard_loggers_.back()->debug(message);
    }
}


