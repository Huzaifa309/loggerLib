#include "sharded_logging.h"

void Sharded_Logger::initialize(uint8_t shard_count,
                                const std::string &log_file_prefix,
                                size_t max_file_size) {
  shard_loggers_.clear();
  shard_loggers_.reserve(shard_count);

  for (uint8_t shard_id = 0; shard_id < shard_count; ++shard_id) {
    std::string shard_log_file =
        log_file_prefix + "_shard_" + std::to_string(shard_id) + ".log";
    auto logger = std::make_unique<qLogger>();
    logger->initialize(shard_log_file, max_file_size);
    shard_loggers_.emplace_back(std::move(logger));
  }
}

// Forwarders:
void Sharded_Logger::info(uint8_t shard_id, const std::string &message) {
  if (shard_id < shard_loggers_.size()) {
    shard_loggers_[shard_id]->info(message);
  }
}

void Sharded_Logger::warn(uint8_t shard_id, const std::string &message) {
  if (shard_id < shard_loggers_.size()) {
    shard_loggers_[shard_id]->warn(message);
  }
}

void Sharded_Logger::error(uint8_t shard_id, const std::string &message) {
  if (shard_id < shard_loggers_.size()) {
    shard_loggers_[shard_id]->error(message);
  }
}

void Sharded_Logger::debug(uint8_t shard_id, const std::string &message) {
  if (shard_id < shard_loggers_.size()) {
    shard_loggers_[shard_id]->debug(message);
  }
}

// Log level control implementations
void Sharded_Logger::set_log_level(uint8_t shard_id, LogLevel level) {
  if (shard_id < shard_loggers_.size()) {
    shard_loggers_[shard_id]->set_log_level(level);
  }
}

void Sharded_Logger::set_log_level_all(LogLevel level) {
  for (auto &logger : shard_loggers_) {
    logger->set_log_level(level);
  }
}

LogLevel Sharded_Logger::get_log_level(uint8_t shard_id) const {
  if (shard_id < shard_loggers_.size()) {
    return shard_loggers_[shard_id]->get_log_level();
  }
  return LogLevel::INFO; // Default fallback
}
