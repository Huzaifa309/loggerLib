#pragma once

#include <string>
#include "quill/Logger.h"

class Logger {
public:
    Logger();
    explicit Logger(const std::string& log_file);

    void initialize(const std::string& log_file);
    quill::Logger* get_logger() const;

private:
    quill::Logger* logger_ = nullptr;
};

