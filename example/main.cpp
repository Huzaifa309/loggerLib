#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "loggerlib.h"

int main() {
    // -----------------------------
    // Single-file logging with qLogger
    // -----------------------------
    std::cout << "[qLogger] Initializing single-file logger..." << std::endl;
    qLogger::get().initialize("logs/single_app.log", LogLevel::DEBUG,
                              1 * 1024 * 1024);  // 1MB rotation

    // Basic logging
    qLogger::get().info("Application started (single-file)");
    qLogger::get().warn("This is a warning (single-file)");
    qLogger::get().error("An error occurred (single-file)");
    qLogger::get().debug("Debug information (single-file)");

    // FMT-style logging (literal and variable format string)
    int user_id = 42;
    std::string username = "alice";
    qLogger::get().info_fast("User {} logged in with id {} (single-file)",
                             username, user_id);
    const char *fmt = "User {} performed action {} (single-file)";
    qLogger::get().info_fast(fmt, username, "logout");

    // Change level to WARNING (filters out INFO/DEBUG)
    qLogger::get().set_log_level(LogLevel::WARNING);
    qLogger::get().info("This info will NOT be logged (single-file)");
    qLogger::get().warn("This warning WILL be logged (single-file)");

    // -----------------------------
    // Sharded logging with ShardedLogger
    // -----------------------------
    std::cout << "[ShardedLogger] Initializing 3 shards..." << std::endl;
    ShardedLogger::get().initialize(3, "logs/sharded_app",
                                    2 * 1024 * 1024);  // 2MB rotation

    // Set a global level, then override a specific shard
    ShardedLogger::get().set_log_level_all(ShardedLogger::LogLevel::INFO);
    ShardedLogger::get().set_log_level(1, ShardedLogger::LogLevel::WARNING);

    // Basic per-shard logging (0-based shard IDs)
    ShardedLogger::get().info(0, "Shard 0 started");
    ShardedLogger::get().debug(0, "Shard 0 debug (visible? INFO -> no)");
    ShardedLogger::get().warn(1, "Shard 1 warning (level WARNING)");
    ShardedLogger::get().info(1, "Shard 1 info (filtered by WARNING)");
    ShardedLogger::get().debug(2, "Shard 2 debug (filtered by INFO)");
    ShardedLogger::get().error(2, "Shard 2 error (always visible)");

    // Ultra-fast fmt-style logging per shard
    ShardedLogger::get().info_fast(0, "Shard {} processed batch {}", 0, 101);
    ShardedLogger::get().warn_fast(1, "Shard {} memory {}%", 1, 83);
    ShardedLogger::get().error_fast(2, "Shard {} error code {}", 2, 404);

    // Multithreaded shard logging demo
    std::cout << "[ShardedLogger] Spawning worker threads..." << std::endl;
    std::vector<std::thread> workers;
    for (int shard = 0; shard < 3; ++shard) {
        workers.emplace_back([shard]() {
            for (int i = 0; i < 20; ++i) {
                ShardedLogger::get().debug_fast(
                    shard, "[thread] shard={} message={} value={}", shard, i,
                    i * 10);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }

    for (auto &t : workers) {
        t.join();
    }

    // Demonstrate changing all shard levels to DEBUG, then emit DEBUG messages
    std::cout << "[ShardedLogger] Setting all shards to DEBUG..." << std::endl;
    ShardedLogger::get().set_log_level_all(ShardedLogger::LogLevel::DEBUG);
    for (int i = 0; i < 3; ++i) {
        ShardedLogger::get().debug(i, "Shard DEBUG now visible everywhere");
    }

    std::cout << "Demo complete. Check the logs directory for output."
              << std::endl;
    return 0;
}
