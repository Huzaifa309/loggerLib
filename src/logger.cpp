#include "logger.h"

#include <memory>
#include <iostream>
#include <utility>
#include <string>

#include "quill/bundled/fmt/core.h"
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/sinks/FileSink.h"
#include "quill/sinks/RotatingFileSink.h"
#include "quill/LogMacros.h"

// PIMPL implementation
class LoggerImpl {
public:
    LoggerImpl() = default;
    ~LoggerImpl() = default;

    void initialize(const std::string& log_file, LogLevel level, size_t max_file_size) {
        static bool backend_started = false;
        if (!backend_started) {
            backend_started = true;
            quill::Backend::start();
        }
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

    LogLevel get_log_level() const {
        return current_level_;
    }

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
    void log_fast_impl(const char* level, const std::string& msg) {
        if (!quill_logger_) return;
        if (strcmp(level, "info") == 0) {
            LOG_INFO(quill_logger_, "{}", msg);
        } else if (strcmp(level, "warn") == 0) {
            LOG_WARNING(quill_logger_, "{}", msg);
        } else if (strcmp(level, "error") == 0) {
            LOG_ERROR(quill_logger_, "{}", msg);
        } else if (strcmp(level, "debug") == 0) {
            LOG_DEBUG(quill_logger_, "{}", msg);
        }
    }

private:
    quill::Logger* quill_logger_ = nullptr;
    LogLevel current_level_ = LogLevel::INFO;
};

Logger::Logger() : impl_(std::make_unique<LoggerImpl>()) {}

Logger::Logger(const std::string& log_file, size_t max_file_size)
    : impl_(std::make_unique<LoggerImpl>()) {
    impl_->initialize(log_file, LogLevel::DEBUG, max_file_size);
}

Logger::Logger(const std::string& log_file, LogLevel level, size_t max_file_size)
    : impl_(std::make_unique<LoggerImpl>()) {
    impl_->initialize(log_file, level, max_file_size);
}

Logger::~Logger() = default;

void Logger::initialize(const std::string& log_file, size_t max_file_size) {
    impl_->initialize(log_file, LogLevel::DEBUG, max_file_size);
}

void Logger::initialize(const std::string& log_file, LogLevel level, size_t max_file_size) {
    impl_->initialize(log_file, level, max_file_size);
}

void Logger::set_log_level(LogLevel level) {
    impl_->set_log_level(level);
}

LogLevel Logger::get_log_level() const {
    return impl_->get_log_level();
}

void Logger::info(const std::string& message) {
    impl_->info(message);
}
void Logger::warn(const std::string& message) {
    impl_->warn(message);
}
void Logger::error(const std::string& message) {
    impl_->error(message);
}
void Logger::debug(const std::string& message) {
    impl_->debug(message);
}
void Logger::log_fast_impl(const char* level, const std::string& msg) {
    impl_->log_fast_impl(level, msg);
}

void Logger::log_fast(const char* level, const std::string& msg) {
    log_fast_impl(level, msg);
}


