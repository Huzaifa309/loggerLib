#include "sharded_logging.h"

namespace {
static quill::Logger *create_logger_for_file(const std::string &log_file,
                                             size_t max_file_size) {
    std::shared_ptr<quill::Sink> sink;
    if (max_file_size > 0) {
        sink = quill::Frontend::create_or_get_sink<quill::RotatingFileSink>(
            log_file, [max_file_size] {
                quill::RotatingFileSinkConfig cfg;
                cfg.set_open_mode('a');
                cfg.set_filename_append_option(
                    quill::FilenameAppendOption::StartDateTime);
                cfg.set_rotation_max_file_size(max_file_size);
                cfg.set_max_backup_files(5);
                cfg.set_overwrite_rolled_files(false);
                return cfg;
            }());
    } else {
        sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
            log_file,
            [] {
                quill::FileSinkConfig cfg;
                cfg.set_open_mode('a');
                cfg.set_filename_append_option(
                    quill::FilenameAppendOption::None);
                return cfg;
            }(),
            quill::FileEventNotifier{});
    }

    auto *logger = quill::Frontend::create_or_get_logger(
        log_file, std::move(sink),
        "[%(time)] [%(log_level)] [%(process_id)] [%(logger)] %(message)",
        "%Y-%m-%d %H:%M:%S.%Qms ", quill::Timezone::LocalTime);
    return logger;
}
}  // namespace

std::once_flag Sharded_Logger::backend_init_flag_;

void Sharded_Logger::init_backend_once_() {
    std::call_once(backend_init_flag_, [] { quill::Backend::start(); });
}

static quill::LogLevel pick_default_quill_level() {
    return quill::LogLevel::Info;
}

quill::LogLevel Sharded_Logger::to_quill_level_(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:
            return quill::LogLevel::TraceL3;
        case LogLevel::DEBUG:
            return quill::LogLevel::Debug;
        case LogLevel::INFO:
            return quill::LogLevel::Info;
        case LogLevel::WARNING:
            return quill::LogLevel::Warning;
        case LogLevel::ERROR:
            return quill::LogLevel::Error;
        case LogLevel::CRITICAL:
            return quill::LogLevel::Critical;
        default:
            return pick_default_quill_level();
    }
}

Sharded_Logger::~Sharded_Logger() = default;

void Sharded_Logger::initialize(uint8_t shard_count,
                                const std::string &log_file_prefix,
                                size_t max_file_size) {
    init_backend_once_();

    // Clear any previous state to avoid duplicates on re-initialization
    shard_loggers_.clear();
    shard_levels_.clear();
    shard_loggers_.reserve(shard_count);
    shard_levels_.reserve(shard_count);

    for (uint8_t shard_id = 0; shard_id < shard_count; ++shard_id) {
        // Generate unique filename per shard
        std::string shard_log_file =
            fmtquill::format("{}_shard_{}.log", log_file_prefix, shard_id);

        quill::Logger *logger =
            create_logger_for_file(shard_log_file, max_file_size);

        // Ensure default level is applied and track per-shard level
        logger->set_log_level(to_quill_level_(LogLevel::INFO));

        shard_loggers_.emplace_back(logger);
        shard_levels_.emplace_back(LogLevel::INFO);
    }
}

// Forwarders:
void Sharded_Logger::info(uint8_t shard_id, const std::string &message) {
    if (shard_id < shard_loggers_.size()) {
        LOG_INFO(shard_loggers_[shard_id], "{}", message);
    }
}

void Sharded_Logger::warn(uint8_t shard_id, const std::string &message) {
    if (shard_id < shard_loggers_.size()) {
        LOG_WARNING(shard_loggers_[shard_id], "{}", message);
    }
}

void Sharded_Logger::error(uint8_t shard_id, const std::string &message) {
    if (shard_id < shard_loggers_.size()) {
        LOG_ERROR(shard_loggers_[shard_id], "{}", message);
    }
}

void Sharded_Logger::debug(uint8_t shard_id, const std::string &message) {
    if (shard_id < shard_loggers_.size()) {
        LOG_DEBUG(shard_loggers_[shard_id], "{}", message);
    }
}

// Log level control implementations
void Sharded_Logger::set_log_level(uint8_t shard_id, LogLevel level) {
    if (shard_id < shard_loggers_.size()) {
        shard_loggers_[shard_id]->set_log_level(to_quill_level_(level));
        shard_levels_[shard_id] = level;
    }
}

void Sharded_Logger::set_log_level_all(LogLevel level) {
    for (size_t i = 0; i < shard_loggers_.size(); ++i) {
        shard_loggers_[i]->set_log_level(to_quill_level_(level));
        shard_levels_[i] = level;
    }
}

Sharded_Logger::LogLevel Sharded_Logger::get_log_level(uint8_t shard_id) const {
    if (shard_id < shard_levels_.size()) {
        return shard_levels_[shard_id];
    }
    return LogLevel::INFO;  // Default fallback
}
