#include "loggerlib.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

int main() {

  // -----------------------------
  // Single-file logging with qLogger
  // -----------------------------
  std::cout << "[qLogger] Initializing single-file logger..." << std::endl;
  qLogger::getInstance().initialize("logs/single_app.log", LogLevel::DEBUG,
                                    1 * 1024 * 1024); // 1MB rotation

  // Basic logging
  qLogger::getInstance().info("Application started (single-file)");
  qLogger::getInstance().warn("This is a warning (single-file)");
  qLogger::getInstance().error("An error occurred (single-file)");
  qLogger::getInstance().debug("Debug information (single-file)");

  // FMT-style logging (literal and variable format string)
  int user_id = 42;
  std::string username = "alice";
  qLogger::getInstance().info_fast("User {} logged in with id {} (single-file)",
                                   username, user_id);
  const char *fmt = "User {} performed action {} (single-file)";
  qLogger::getInstance().info_fast(fmt, username, "logout");

  // Change level to WARNING (filters out INFO/DEBUG)
  qLogger::getInstance().set_log_level(LogLevel::WARNING);
  qLogger::getInstance().info("This info will NOT be logged (single-file)");
  qLogger::getInstance().warn("This warning WILL be logged (single-file)");

  // -----------------------------
  // Sharded logging with Sharded_Logger
  // -----------------------------
  std::cout << "[Sharded_Logger] Initializing 3 shards..." << std::endl;
  Sharded_Logger::getInstance().initialize(3, "logs/sharded_app",
                                           2 * 1024 * 1024); // 2MB rotation

  // Set a global level, then override a specific shard
  Sharded_Logger::getInstance().set_log_level_all(
      Sharded_Logger::LogLevel::INFO);
  Sharded_Logger::getInstance().set_log_level(
      1, Sharded_Logger::LogLevel::WARNING);

  // Basic per-shard logging (0-based shard IDs)
  Sharded_Logger::getInstance().info(0, "Shard 0 started");
  Sharded_Logger::getInstance().debug(0, "Shard 0 debug (visible? INFO -> no)");
  Sharded_Logger::getInstance().warn(1, "Shard 1 warning (level WARNING)");
  Sharded_Logger::getInstance().info(1, "Shard 1 info (filtered by WARNING)");
  Sharded_Logger::getInstance().debug(2, "Shard 2 debug (filtered by INFO)");
  Sharded_Logger::getInstance().error(2, "Shard 2 error (always visible)");

  // Ultra-fast fmt-style logging per shard
  Sharded_Logger::getInstance().info_fast(0, "Shard {} processed batch {}", 0,
                                          101);
  Sharded_Logger::getInstance().warn_fast(1, "Shard {} memory {}%", 1, 83);
  Sharded_Logger::getInstance().error_fast(2, "Shard {} error code {}", 2, 404);

  // Multithreaded shard logging demo
  std::cout << "[Sharded_Logger] Spawning worker threads..." << std::endl;
  std::vector<std::thread> workers;
  for (int shard = 0; shard < 3; ++shard) {
    workers.emplace_back([shard]() {
      for (int i = 0; i < 20; ++i) {
        Sharded_Logger::getInstance().debug_fast(
            shard, "[thread] shard={} message={} value={}", shard, i, i * 10);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });
  }

  for (auto &t : workers) {
    t.join();
  }

  // Demonstrate changing all shard levels to DEBUG, then emit DEBUG messages
  std::cout << "[Sharded_Logger] Setting all shards to DEBUG..." << std::endl;
  Sharded_Logger::getInstance().set_log_level_all(
      Sharded_Logger::LogLevel::DEBUG);
  for (int i = 0; i < 3; ++i) {
    Sharded_Logger::getInstance().debug(i,
                                        "Shard DEBUG now visible everywhere");
  }

  std::cout << "Demo complete. Check the logs directory for output."
            << std::endl;
  return 0;
}
