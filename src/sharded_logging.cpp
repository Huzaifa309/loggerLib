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

std::once_flag ShardedLogger::_backendInitFlag;

void ShardedLogger::_init_backend_once() {
    std::call_once(_backendInitFlag, [] { quill::Backend::start(); });
}

static quill::LogLevel pick_default_quill_level() {
    return quill::LogLevel::Info;
}

quill::LogLevel ShardedLogger::_to_quill_level(LogLevel level) {
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

ShardedLogger::~ShardedLogger() = default;

void ShardedLogger::initialize(uint8_t shard_count,
                               const std::string &log_file_prefix,
                               size_t max_file_size) {
    _init_backend_once();

    // Clear any previous state to avoid duplicates on re-initialization
    _shardLoggers.clear();
    _shardLevels.clear();
    _shardLoggers.reserve(shard_count);
    _shardLevels.reserve(shard_count);

    for (uint8_t shardId = 0; shardId < shard_count; ++shardId) {
        // Generate unique filename per shard
        std::string shard_log_file =
            fmtquill::format("{}_shard_{}.log", log_file_prefix, shardId);

        quill::Logger *logger =
            create_logger_for_file(shard_log_file, max_file_size);

        // Ensure default level is applied and track per-shard level
        logger->set_log_level(_to_quill_level(LogLevel::INFO));

        _shardLoggers.emplace_back(logger);
        _shardLevels.emplace_back(LogLevel::INFO);
    }
}

// Forwarders:
void ShardedLogger::info(uint8_t shardId, const std::string &message) {
    if (shardId < _shardLoggers.size()) {
        LOG_INFO(_shardLoggers[shardId], "{}", message);
    }
}

void ShardedLogger::warn(uint8_t shardId, const std::string &message) {
    if (shardId < _shardLoggers.size()) {
        LOG_WARNING(_shardLoggers[shardId], "{}", message);
    }
}

void ShardedLogger::error(uint8_t shardId, const std::string &message) {
    if (shardId < _shardLoggers.size()) {
        LOG_ERROR(_shardLoggers[shardId], "{}", message);
    }
}

void ShardedLogger::debug(uint8_t shardId, const std::string &message) {
    if (shardId < _shardLoggers.size()) {
        LOG_DEBUG(_shardLoggers[shardId], "{}", message);
    }
}

// Log level control implementations
void ShardedLogger::set_log_level(uint8_t shardId, LogLevel level) {
    if (shardId < _shardLoggers.size()) {
        _shardLoggers[shardId]->set_log_level(_to_quill_level(level));
        _shardLevels[shardId] = level;
    }
}

void ShardedLogger::set_log_level_all(LogLevel level) {
    for (size_t i = 0; i < _shardLoggers.size(); ++i) {
        _shardLoggers[i]->set_log_level(_to_quill_level(level));
        _shardLevels[i] = level;
    }
}

ShardedLogger::LogLevel ShardedLogger::get_log_level(uint8_t shardId) const {
    if (shardId < _shardLevels.size()) {
        return _shardLevels[shardId];
    }
    return LogLevel::INFO;  // Default fallback
}
