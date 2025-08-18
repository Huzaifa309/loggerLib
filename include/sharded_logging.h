#pragma once

#include "log_level.h"
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/bundled/fmt/core.h"
#include "quill/bundled/fmt/format.h"
#include "quill/sinks/FileSink.h"
#include "quill/sinks/RotatingFileSink.h"
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class Sharded_Logger {
public:
  static Sharded_Logger &getInstance() {
    static Sharded_Logger instance;
    return instance;
  }

  Sharded_Logger(const Sharded_Logger &) = delete;
  Sharded_Logger &operator=(const Sharded_Logger &) = delete;
  Sharded_Logger(Sharded_Logger &&) = delete;
  Sharded_Logger &operator=(Sharded_Logger &&) = delete;

  ~Sharded_Logger();

  // Use shared LogLevel but keep the same API surface
  using LogLevel = ::LogLevel;

  void initialize(uint8_t shard_count, const std::string &log_file_prefix,
                  size_t max_file_size = 0);

  // Clean interface methods
  void info(uint8_t shard_id, const std::string &message);
  void warn(uint8_t shard_id, const std::string &message);
  void error(uint8_t shard_id, const std::string &message);
  void debug(uint8_t shard_id, const std::string &message);

  // Ultra-fast logging: fmt-style, variadic, user does not need to include
  // fmt/quill
  template <typename... Args>
  void info_fast(uint8_t shard_id, const char *fmt, Args &&...args) {
    if (shard_id < shard_loggers_.size()) {
      info(shard_id,
           fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
    }
  }
  template <typename... Args>
  void warn_fast(uint8_t shard_id, const char *fmt, Args &&...args) {
    if (shard_id < shard_loggers_.size()) {
      warn(shard_id,
           fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
    }
  }
  template <typename... Args>
  void error_fast(uint8_t shard_id, const char *fmt, Args &&...args) {
    if (shard_id < shard_loggers_.size()) {
      error(shard_id,
            fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
    }
  }
  template <typename... Args>
  void debug_fast(uint8_t shard_id, const char *fmt, Args &&...args) {
    if (shard_id < shard_loggers_.size()) {
      debug(shard_id,
            fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
    }
  }

  // Log level control methods
  void set_log_level(uint8_t shard_id, LogLevel level);
  void set_log_level_all(LogLevel level);
  LogLevel get_log_level(uint8_t shard_id) const;

private:
  Sharded_Logger() = default;

  static void init_backend_once_();
  static std::once_flag backend_init_flag_;

  static quill::LogLevel to_quill_level_(LogLevel level);

  std::vector<quill::Logger *> shard_loggers_;
  std::vector<LogLevel> shard_levels_;
};
