#pragma once

#include <string>

class Logger {
public:
    Logger();
    explicit Logger(const std::string& log_file);
    ~Logger();

    void initialize(const std::string& log_file);
    
    // Clean interface methods that don't expose Quill types
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);

private:
    void* logger_ = nullptr; // Hide Quill logger completely
};

