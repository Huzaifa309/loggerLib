#pragma once


#include <vector>
#include <memory>
#include <string>
#include "logger.h"

class LoggerWrapper {
public:
    LoggerWrapper();
    LoggerWrapper(uint8_t shard_count, const std::string& log_file_prefix);

    quill::Logger* get_logger(uint8_t shard_id) const;

private:
    std::vector<std::unique_ptr<Logger>> shard_loggers_;
};

