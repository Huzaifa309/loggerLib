# LoggerLib

A C++ logging library that provides a clean, simple interface for logging with automatic Quill integration (hidden from users, while still enabling fast fmt-style logging).

---

## Quick Start for Users

If you just want to use the library in your project:

1. Install the prebuilt library (optional):
   ```bash
   # Download and install from releases, or contact maintainer
   # The library will be installed to /usr/local/
   ```

2. Use in your project (single-file logging):
   ```cpp
   #include "quill_logging.h"
   
   int main() {
       // Initialize once; 10MB rotation enabled
       qLogger::getInstance().initialize("my_app.log", LogLevel::DEBUG, 10 * 1024 * 1024);
       qLogger::getInstance().info_fast("Hello {} from LoggerLib!", "World");
       return 0;
   }
   ```

3. Or sharded logging (multiple files, one per shard):
   ```cpp
   #include "sharded_logging.h"
   
   int main() {
       // Creates logs: logs/app_shard_0.log, logs/app_shard_1.log, logs/app_shard_2.log
       Sharded_Logger::getInstance().initialize(3, "logs/app", 10 * 1024 * 1024);
       Sharded_Logger::getInstance().set_log_level_all(Sharded_Logger::LogLevel::DEBUG);
       Sharded_Logger::getInstance().info_fast(0, "Shard {} hello", 0);
       return 0;
   }
   ```

4. CMake setup (installed library):
   ```cmake
   cmake_minimum_required(VERSION 3.16)
   project(MyApp)
   set(CMAKE_CXX_STANDARD 17)
   include_directories(/usr/local/include)
   add_executable(myapp main.cpp)
   target_link_libraries(myapp /usr/local/lib/libloggerlib.a pthread)
   ```

That's it. No separate Quill or fmt setup required by users.

---

## Building from Source (Developers)

If you want to build the library from source or contribute:

1. Clone and build the library:
   ```bash
   git clone https://github.com/Huzaifa309/loggerLib
   cd loggerLib
   # IMPORTANT: Initialize submodules (needed for Quill dependency)
   git submodule update --init --recursive
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```
   Note: If you do not run the submodule command, the build will fail with an error about missing Quill.

2. Install the library (requires sudo):
   ```bash
   sudo make install
   ```
   This installs:
   - Public headers to `/usr/local/include/` (notably `quill_logging.h`, `sharded_logging.h`)
   - The static library to `/usr/local/lib/libloggerlib.a`
   - Vendored Quill headers to `/usr/local/include/`

3. Build with example:
   ```bash
   cmake -DBUILD_EXAMPLE=ON ..
   make -j$(nproc)
   ./bin/example
   ```

---

## How to Use the Library

### Single-File Logging (qLogger)

Header-only interface for simple logging into a single file.

```cpp
#include "quill_logging.h"

int main() {
    // Initialize once; pass optional log level and rotation size (bytes)
    qLogger::getInstance().initialize("logs/my_app.log", LogLevel::INFO, 10 * 1024 * 1024);

    // Basic logging
    qLogger::getInstance().info("Application started");
    qLogger::getInstance().warn("This is a warning");
    qLogger::getInstance().error("An error occurred");
    qLogger::getInstance().debug("Debug information");

    // FMT-style logging (format string can be a variable or literal)
    int user_id = 42;
    std::string username = "alice";
    qLogger::getInstance().info_fast("User {} logged in with id {}", username, user_id);

    // Manual log level control
    qLogger::getInstance().set_log_level(LogLevel::WARNING);
    qLogger::getInstance().info("This will NOT be logged");
    qLogger::getInstance().warn("This WILL be logged");

    return 0;
}
```

- Log levels (`LogLevel`) are: TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL.
- Rotation is enabled when `max_file_size > 0`; otherwise logs append to a single file.

### Sharded Logging (Sharded_Logger)

High-performance logging across multiple files (one per shard). Shard IDs are 0-based.

```cpp
#include "sharded_logging.h"
#include <thread>

int main() {
    // Initialize 3 shards: logs/app_shard_0.log, logs/app_shard_1.log, logs/app_shard_2.log
    Sharded_Logger::getInstance().initialize(3, "logs/app", 10 * 1024 * 1024);

    // Per-shard level control (defaults to INFO on init)
    Sharded_Logger::getInstance().set_log_level(0, Sharded_Logger::LogLevel::INFO);
    Sharded_Logger::getInstance().set_log_level(1, Sharded_Logger::LogLevel::DEBUG);
    Sharded_Logger::getInstance().set_log_level_all(Sharded_Logger::LogLevel::DEBUG); // set all at once

    // Log to different shards
    Sharded_Logger::getInstance().info(0, "Shard 0 started");
    Sharded_Logger::getInstance().debug(1, "Shard 1 debug message");

    // Ultra-fast fmt-style logging per shard
    for (uint8_t id = 0; id < 3; ++id) {
        Sharded_Logger::getInstance().info_fast(id, "Shard {} processed batch {}", id, 100 + id);
    }

    return 0;
}
```

Notes:
- Each shard is a separate logger with its own level. Use `set_log_level_all(...)` to keep them in sync.
- Re-initializing with `initialize(...)` clears previous shard state and recreates one sink/logger per shard.

---

## Log Rotation and File Behavior

- Default (no rotation): When `max_file_size == 0`, the library uses a non-rotating `FileSink` in append mode. Filenames are exactly what you specify.
- Rotating: When `max_file_size > 0`, the library uses `RotatingFileSink` per file/shard.
  - Rotation trigger: size exceeds `max_file_size`.
  - Backups: up to 5 rolled files are kept.
  - Naming: rolled files include the start datetime in their names.
  - Open mode: append.

Shard filenames are generated as `"{base}_shard_{i}.log"` for `i` in `[0, shard_count)`. Example: base `logs/app` → `logs/app_shard_0.log`, `logs/app_shard_1.log`, ...

---

## CMake Integration (Installed Library)

Users do not need to add or link to system Quill or fmt. Everything is vendored and linked via `libloggerlib.a`.

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyApp)
set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include)
add_executable(myapp main.cpp)
# Link pthread as well
target_link_libraries(myapp /usr/local/lib/libloggerlib.a pthread)
```

---

## Troubleshooting

- If you get `fatal error: quill/bundled/fmt/core.h: No such file or directory`, ensure the library was installed and headers exist under `/usr/local/include/`.
- Do not add or link to system Quill or fmt; the library vendors and installs the needed headers.
- If you see multiple unexpected files when using shards, ensure you:
  - Initialize once with a base prefix (e.g., `"logs/app"`),
  - Use 0-based shard IDs,
  - Avoid re-initializing unless you intend to reset and recreate shards.
