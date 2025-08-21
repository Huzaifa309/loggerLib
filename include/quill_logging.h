#pragma once

#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>

#include "log_level.h"
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/bundled/fmt/core.h"
#include "quill/bundled/fmt/format.h"
#include "quill/sinks/FileSink.h"
#include "quill/sinks/RotatingFileSink.h"

// Use shared LogLevel

class qLogger {
   public:
    static qLogger &get() {
        static qLogger instance;
        return instance;
    }

    qLogger(const qLogger &) = delete;
    qLogger &operator=(const qLogger &) = delete;
    qLogger(qLogger &&) = delete;
    qLogger &operator=(qLogger &&) = delete;

    ~qLogger() = default;

    // Default Call for get():
    void initialize(const std::string &logFile, size_t maxFileSize = 0) {
        initialize(logFile, LogLevel::DEBUG, maxFileSize);
    }

    void initialize(const std::string &logFile, LogLevel level,
                    size_t maxFileSize = 0) {
        std::call_once(_initFlag, [&] {
            init_backend();
            try {
                std::shared_ptr<quill::Sink> sink;
                if (maxFileSize > 0) {
                    sink = quill::Frontend::create_or_get_sink<
                        quill::RotatingFileSink>(logFile, [maxFileSize] {
                        quill::RotatingFileSinkConfig cfg;
                        cfg.set_open_mode('a');
                        cfg.set_filename_append_option(
                            quill::FilenameAppendOption::StartDateTime);
                        cfg.set_rotation_max_file_size(maxFileSize);
                        cfg.set_max_backup_files(5);
                        cfg.set_overwrite_rolled_files(false);
                        return cfg;
                    }());
                } else {
                    sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
                        logFile,
                        [] {
                            quill::FileSinkConfig cfg;
                            cfg.set_open_mode('a');
                            cfg.set_filename_append_option(
                                quill::FilenameAppendOption::None);
                            return cfg;
                        }(),
                        quill::FileEventNotifier{});
                }
                _quillLogger = quill::Frontend::create_or_get_logger(
                    logFile, std::move(sink),
                    "[%(time)] [%(log_level)] [%(process_id)] [%(logger)] "
                    "%(message)",
                    "%Y-%m-%d %H:%M:%S.%Qms ", quill::Timezone::LocalTime);
                set_log_level(level);
            } catch (const std::exception &ex) {
                std::cerr << "Log initialization failed: " << ex.what()
                          << std::endl;
            }
        });
    }

    void set_log_level(LogLevel level) {
        if (!_quillLogger) return;
        quill::LogLevel logLevel;
        switch (level) {
            case LogLevel::TRACE:
                logLevel = quill::LogLevel::TraceL3;
                break;
            case LogLevel::DEBUG:
                logLevel = quill::LogLevel::Debug;
                break;
            case LogLevel::INFO:
                logLevel = quill::LogLevel::Info;
                break;
            case LogLevel::WARNING:
                logLevel = quill::LogLevel::Warning;
                break;
            case LogLevel::ERROR:
                logLevel = quill::LogLevel::Error;
                break;
            case LogLevel::CRITICAL:
                logLevel = quill::LogLevel::Critical;
                break;
            default:
                logLevel = quill::LogLevel::Info;
                break;
        }
        _quillLogger->set_log_level(logLevel);
        _logLevel = level;
    }

    LogLevel get_log_level() const { return _logLevel; }

    // Logging Function Takes message in {} fmt straightaway
    void info(const std::string &message) {
        if (_quillLogger) {
            LOG_INFO(_quillLogger, "{}", message);
        }
    }
    void warn(const std::string &message) {
        if (_quillLogger) {
            LOG_WARNING(_quillLogger, "{}", message);
        }
    }
    void error(const std::string &message) {
        if (_quillLogger) {
            LOG_ERROR(_quillLogger, "{}", message);
        }
    }
    void debug(const std::string &message) {
        if (_quillLogger) {
            LOG_DEBUG(_quillLogger, "{}", message);
        }
    }

    // Ultra-fast logging: fmt-style, variadic, user does not need to include
    // fmt/quill
    template <typename... Args>
    void info_fast(const char *fmt, Args &&...args) {
        info(fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
    }
    template <typename... Args>
    void warn_fast(const char *fmt, Args &&...args) {
        warn(fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
    }
    template <typename... Args>
    void error_fast(const char *fmt, Args &&...args) {
        error(fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
    }
    template <typename... Args>
    void debug_fast(const char *fmt, Args &&...args) {
        debug(fmtquill::vformat(fmt, fmtquill::make_format_args(args...)));
    }

   private:
    void init_backend() {
        static bool backend_started = false;
        if (!backend_started) {
            backend_started = true;
            quill::Backend::start();
        }
    }
    qLogger() = default;

    std::once_flag _initFlag;
    quill::Logger *_quillLogger = nullptr;
    LogLevel _logLevel = LogLevel::INFO;
};
