#include "logger.h"

#include <memory>
#include <iostream>

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/sinks/FileSink.h"
#include "quill/LogMacros.h"

Logger::Logger() = default;

Logger::Logger(const std::string& log_file) {
    initialize(log_file);
}

Logger::~Logger() = default;

void Logger::initialize(const std::string& log_file) {
    static bool backend_started = false;

    if (!backend_started) {
        backend_started = true;
        quill::Backend::start();
    }

    try {
        // Create file sink
        auto file_sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
            log_file,
            [] {
                quill::FileSinkConfig cfg;
                cfg.set_open_mode('a');
                cfg.set_filename_append_option(quill::FilenameAppendOption::None);
                return cfg;
            }(),
            quill::FileEventNotifier{}
        );

        // Create logger
        auto* quill_logger = quill::Frontend::create_or_get_logger(
            log_file,
            file_sink,
            "[%(time)] [%(log_level)] [%(process_id)] [%(logger)] %(message)",
            "%Y-%m-%d %H:%M:%S.%Qms ",
            quill::Timezone::LocalTime
        );

        quill_logger->set_log_level(quill::LogLevel::Debug);
        logger_ = static_cast<void*>(quill_logger);
    } catch (const std::exception& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
}

// Clean interface implementations
void Logger::info(const std::string& message) {
    if (logger_) {
        auto* quill_logger = static_cast<quill::Logger*>(logger_);
        LOG_INFO(quill_logger, "{}", message);
    }
}

void Logger::warn(const std::string& message) {
    if (logger_) {
        auto* quill_logger = static_cast<quill::Logger*>(logger_);
        LOG_WARNING(quill_logger, "{}", message);
    }
}

void Logger::error(const std::string& message) {
    if (logger_) {
        auto* quill_logger = static_cast<quill::Logger*>(logger_);
        LOG_ERROR(quill_logger, "{}", message);
    }
}

void Logger::debug(const std::string& message) {
    if (logger_) {
        auto* quill_logger = static_cast<quill::Logger*>(logger_);
        LOG_DEBUG(quill_logger, "{}", message);
    }
}
