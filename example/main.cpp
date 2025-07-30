#include "logger.h"
#include "loggerwrapper.h"
#include <iostream>

int main() {
  // Create a logger with 10MB rotation
  Logger logger("logs/my_app.log", 10 * 1024 * 1024);
  logger.set_log_level(LogLevel::WARNING);

  // Basic logging
  logger.info("Application started");
  logger.warn("This is a warning");
  logger.error("An error occurred");
  logger.debug("Debug information");

  // FMT-style logging (works with any format string, literal or variable)
  int user_id = 42;
  std::string username = "alice";
  logger.info_fast("User {} logged in with id {}", "helo", user_id);
  logger.warn_fast("Low disk space: {}% remaining", 15);
  logger.error_fast("Failed to open file: {}", "/tmp/data.txt");
  logger.debug_fast("Debug: x={} y={}, {} {}", 42, 3.14, "hello", "world");

  // Sharded logging with LoggerWrapper
  LoggerWrapper *wrapper =
      new LoggerWrapper(2, "logs/sharded", 5 * 1024 * 1024);
  int shard_id = 0;
  wrapper->info_fast(shard_id, "Shard {} message: {}", shard_id, "hello");
  wrapper->info_fast(shard_id, "Shard {} message: {}", shard_id, "world");
  wrapper->debug_fast(shard_id, "Shard {} This is a debugging message ",
                      shard_id);

  // shard_id = 1;
  // wrapper.info_fast(shard_id, "Shard {} message: {}", shard_id, "hello");
  // wrapper.info_fast(shard_id, "Shard {} message: {}", shard_id, "world");
  // wrapper.debug_fast(shard_id, "Shard {} This is a debugging message ",
  // shard_id);

  delete wrapper;

  // Log level control
  logger.set_log_level(LogLevel::WARNING);
  logger.info("This info will NOT be logged (level is WARNING)");
  logger.warn("This warning WILL be logged");

  // Demonstrate that format string can be a variable (since we use
  // fmtquill::format)
  const char *fmt = "User {} performed action {}";
  logger.info_fast(fmt, username, "logout");

  std::cout << "Logging complete. Check the logs directory for output."
            << std::endl;
  return 0;
}