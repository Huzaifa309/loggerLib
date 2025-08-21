#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "log_level.h"
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/bundled/fmt/core.h"
#include "quill/bundled/fmt/format.h"
#include "quill/sinks/FileSink.h"
#include "quill/sinks/RotatingFileSink.h"

class ShardedLogger {
   public:
    static ShardedLogger &get() {
        static ShardedLogger shardedLogger;
        return shardedLogger;
    }

    ShardedLogger(const ShardedLogger &) = delete;
    ShardedLogger &operator=(const ShardedLogger &) = delete;
    ShardedLogger(ShardedLogger &&) = delete;
    ShardedLogger &operator=(ShardedLogger &&) = delete;

    ~ShardedLogger();

    // Use shared LogLevel but keep the same API surface
    using LogLevel = ::LogLevel;

    void initialize(uint8_t shard_count, const std::string &log_file_prefix,
                    size_t max_file_size = 0);

    // Clean interface methods
    void info(uint8_t shardId, const std::string &message);
    void warn(uint8_t shardId, const std::string &message);
    void error(uint8_t shardId, const std::string &message);
    void debug(uint8_t shardId, const std::string &message);

    // Ultra-fast logging: fmt-style, variadic, user does not need to include
    // fmt/quill
    template <typename... Args>
    void info_fast(uint8_t shardId, const char *fmt, Args &&...args) {
        if (shardId < _shardLoggers.size()) {
            info(shardId,
                 fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
        }
    }

    template <typename... Args>
    void warn_fast(uint8_t shardId, const char *fmt, Args &&...args) {
        if (shardId < _shardLoggers.size()) {
            warn(shardId,
                 fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
        }
    }

    template <typename... Args>
    void error_fast(uint8_t shardId, const char *fmt, Args &&...args) {
        if (shardId < _shardLoggers.size()) {
            error(shardId,
                  fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
        }
    }

    template <typename... Args>
    void debug_fast(uint8_t shardId, const char *fmt, Args &&...args) {
        if (shardId < _shardLoggers.size()) {
            debug(shardId,
                  fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
        }
    }

    // Log level control methods
    void set_log_level(uint8_t shardId, LogLevel level);
    void set_log_level_all(LogLevel level);
    LogLevel get_log_level(uint8_t shardId) const;

   private:
    ShardedLogger() = default;

    static void _init_backend_once();
    static std::once_flag _backendInitFlag;

    static quill::LogLevel _to_quill_level(LogLevel level);

    std::vector<quill::Logger *> _shardLoggers;
    std::vector<LogLevel> _shardLevels;
};
