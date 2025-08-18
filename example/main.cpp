#include "sharded_logging.h"
#include <chrono>
#include <thread>
#include <vector>

int main() {

  // Initialize shards ONCE with base prefix and shard count
  Sharded_Logger::getInstance().initialize(3, "logs/app", 10 * 1024 * 1024);
  Sharded_Logger::getInstance().set_log_level_all(
      Sharded_Logger::LogLevel::DEBUG);

  // Write some logs
  Sharded_Logger::getInstance().info(0, "Shard 0: Starting application");
  Sharded_Logger::getInstance().debug(1, "Shard 1: Debugging initialization");
  Sharded_Logger::getInstance().debug(2,
                                      "Shard 2: Debugging thread initialized");

  // Worker threads
  std::vector<std::thread> workers;
  for (int shard = 0; shard < 3; ++shard) {
    workers.emplace_back([shard]() {
      for (int i = 0; i < 5; ++i) {
        Sharded_Logger::getInstance().debug_fast(shard, "Shard {} message {}",
                                                 shard, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
    });
  }

  // Join all threads
  for (auto &t : workers) {
    t.join();
  }

  return 0;
}
