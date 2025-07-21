#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <utility>
#include <type_traits>
#include <cstring>
#include "quill/bundled/fmt/core.h"
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/sinks/FileSink.h"
#include "quill/sinks/RotatingFileSink.h"
#include "quill/LogMacros.h"

// Log levels enum (clean interface, no Quill exposure)
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    CRITICAL = 5
};

class Logger {
public:
    Logger() { init_backend(); }
    explicit Logger(const std::string& log_file, size_t max_file_size = 0) { initialize(log_file, LogLevel::DEBUG, max_file_size); }
    explicit Logger(const std::string& log_file, LogLevel level, size_t max_file_size = 0) { initialize(log_file, level, max_file_size); }
    ~Logger() = default;

    void initialize(const std::string& log_file, size_t max_file_size = 0) { initialize(log_file, LogLevel::DEBUG, max_file_size); }
    void initialize(const std::string& log_file, LogLevel level, size_t max_file_size = 0) {
        init_backend();
        try {
            std::shared_ptr<quill::Sink> sink;
            if (max_file_size > 0) {
                sink = quill::Frontend::create_or_get_sink<quill::RotatingFileSink>(
                    log_file,
                    [max_file_size] {
                        quill::RotatingFileSinkConfig cfg;
                        cfg.set_open_mode('w');
                        cfg.set_filename_append_option(quill::FilenameAppendOption::StartDateTime);
                        cfg.set_rotation_max_file_size(max_file_size);
                        cfg.set_max_backup_files(5);
                        cfg.set_overwrite_rolled_files(false);
                        return cfg;
                    }()
                );
            } else {
                sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
                    log_file,
                    [] {
                        quill::FileSinkConfig cfg;
                        cfg.set_open_mode('a');
                        cfg.set_filename_append_option(quill::FilenameAppendOption::None);
                        return cfg;
                    }(),
                    quill::FileEventNotifier{}
                );
            }
            quill_logger_ = quill::Frontend::create_or_get_logger(
                log_file,
                std::move(sink),
                "[%(time)] [%(log_level)] [%(process_id)] [%(logger)] %(message)",
                "%Y-%m-%d %H:%M:%S.%Qms ",
                quill::Timezone::LocalTime
            );
            set_log_level(level);
        } catch (const std::exception& ex) {
            std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        }
    }

    void set_log_level(LogLevel level) {
        if (!quill_logger_) return;
        quill::LogLevel quill_level;
        switch (level) {
            case LogLevel::TRACE: quill_level = quill::LogLevel::TraceL3; break;
            case LogLevel::DEBUG: quill_level = quill::LogLevel::Debug; break;
            case LogLevel::INFO: quill_level = quill::LogLevel::Info; break;
            case LogLevel::WARNING: quill_level = quill::LogLevel::Warning; break;
            case LogLevel::ERROR: quill_level = quill::LogLevel::Error; break;
            case LogLevel::CRITICAL: quill_level = quill::LogLevel::Critical; break;
            default: quill_level = quill::LogLevel::Info; break;
        }
        quill_logger_->set_log_level(quill_level);
        current_level_ = level;
    }

    LogLevel get_log_level() const { return current_level_; }

    void info(const std::string& message) {
        if (quill_logger_) {
            LOG_INFO(quill_logger_, "{}", message);
        }
    }
    void warn(const std::string& message) {
        if (quill_logger_) {
            LOG_WARNING(quill_logger_, "{}", message);
        }
    }
    void error(const std::string& message) {
        if (quill_logger_) {
            LOG_ERROR(quill_logger_, "{}", message);
        }
    }
    void debug(const std::string& message) {
        if (quill_logger_) {
            LOG_DEBUG(quill_logger_, "{}", message);
        }
    }

    // Ultra-fast logging: fmt-style, variadic, user does not need to include fmt/quill
    template<typename... Args>
    void info_fast(const char* fmt, Args&&... args) {
        info(fmtquill::format(fmt, std::forward<Args>(args)...));
    }
    template<typename... Args>
    void warn_fast(const char* fmt, Args&&... args) {
        warn(fmtquill::format(fmt, std::forward<Args>(args)...));
    }
    template<typename... Args>
    void error_fast(const char* fmt, Args&&... args) {
        error(fmtquill::format(fmt, std::forward<Args>(args)...));
    }
    template<typename... Args>
    void debug_fast(const char* fmt, Args&&... args) {
        debug(fmtquill::format(fmt, std::forward<Args>(args)...));
    }

private:
    void init_backend() {
        static bool backend_started = false;
        if (!backend_started) {
            backend_started = true;
            quill::Backend::start();
        }
    }
    quill::Logger* quill_logger_ = nullptr;
    LogLevel current_level_ = LogLevel::INFO;
};

