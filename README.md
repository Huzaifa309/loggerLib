# LoggerLib

A C++ logging library that provides a clean, simple interface for logging with automatic Quill C++ integration (completely hidden from users, but available for fmt-style logging).

---

## How to Use This Logger Library in Your Project

1. **Clone and build the library:**
   ```bash
   git clone https://github.com/Huzaifa309/loggerLib
   cd loggerLib
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```
2. **Install the library (requires sudo):**
   ```bash
   sudo make install
   ```
   This will install:
   - `logger.h`, `loggerwrapper.h` to `/usr/local/include/`
   - All required Quill and bundled fmt headers to `/usr/local/include/quill/bundled/fmt/`
   - The static library to `/usr/local/lib/libloggerlib.a`

3. **In your own project:**
   - Just `#include "logger.h"` or `#include "loggerwrapper.h"`
   - Link to `/usr/local/lib/libloggerlib.a`
   - **You do NOT need to include or link to Quill or fmt yourself.**
   - You can use `info_fast`, `warn_fast`, etc. with fmt-style formatting and any arguments:
     ```cpp
     logger.info_fast("Hello {} this is Huzaifa {}", i, "Ahmed");
     wrapper.info_fast(0, "Shard {}: value={}", shard_id, value);
     ```

4. **CMake Example for your project:**
   ```cmake
   cmake_minimum_required(VERSION 3.16)
   project(MyApp)
   set(CMAKE_CXX_STANDARD 17)
   include_directories(/usr/local/include)
   add_executable(myapp main.cpp)
   target_link_libraries(myapp /usr/local/lib/libloggerlib.a pthread)
   ```

5. **Troubleshooting:**
   - If you get an error like `fatal error: quill/bundled/fmt/core.h: No such file or directory`, make sure `/usr/local/include/quill/bundled/fmt/core.h` exists. If not, re-run `sudo make install` from your loggerLib build directory.
   - Do **not** add or link to system Quill or fmt. All dependencies are vendored and installed for you.

---

## Features

- **Ultra-Fast Logging**: Direct fmt-style logging methods with zero stringstream overhead (use `{}` placeholders)
- **Clean Interface**: No Quill or fmt includes required in user code
- **Smart Log Rotation**: Automatic file rotation with timestamped filenames
- **PIMPL Pattern**: Complete encapsulation of internal implementation details
- **Manual Log Level Control**: Set and get log levels at runtime
- **Sharded Logging**: High-performance logging across multiple files
- **Thread-Safe**: Built on Quill's thread-safe foundation
- **Static Library**: Easy integration with any C++ project

## Dependencies

- CMake 3.16 or higher
- C++17 compatible compiler
- Git (for fetching Quill C++)
- pthreads (standard system library)

## Building

### Prerequisites

```bash
# Install build tools (Ubuntu/Debian)
sudo apt update
sudo apt install build-essential cmake git

# Or on CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake git
```

### Build Instructions

1. **Clone and build:**
```bash
git clone https://github.com/Huzaifa309/loggerLib
cd loggerLib
mkdir build && cd build
cmake ..
make -j$(nproc)
```

2. **Build with example:**
```bash
cmake -DBUILD_EXAMPLE=ON ..
make -j$(nproc)
```

3. **Install the library:**
```bash
sudo make install
```

## Usage

### Basic Usage

```cpp
#include "logger.h"

int main() {
    // Create a logger with 10MB rotation
    Logger logger("my_app.log", 10 * 1024 * 1024);
    
    // Log messages
    logger.info("Application started");
    logger.warn("This is a warning");
    logger.error("An error occurred");
    logger.debug("Debug information");
    
    // Ultra-fast logging (fmt-style, no stringstream, direct formatting)
    logger.info_fast("User {} logged in at {}", "alice", 1234567890);
    logger.warn_fast("Low disk space: {}% remaining", 42);
    logger.error_fast("Failed to open file: {}", "/tmp/data.txt");
    logger.debug_fast("Debug: x={} y={}", 42, 3.14);
    return 0;
}
```

**Note:** You do NOT need to include any Quill or fmt headers. The logger library vendors and exposes everything you need for fmt-style logging.

### Manual Log Level Control

```cpp
#include "logger.h"

int main() {
    Logger logger("my_app.log", 10 * 1024 * 1024);
    
    // Set log level to only show warnings and errors
    logger.set_log_level(LogLevel::WARNING);
    
    // These won't be logged (below WARNING level)
    logger.debug("Debug message - won't appear");
    logger.info("Info message - won't appear");
    
    // These will be logged
    logger.warn("Warning message - will appear");
    logger.error("Error message - will appear");
    
    // Check current log level
    LogLevel current = logger.get_log_level();
    if (current == LogLevel::WARNING) {
        logger.info("Log level is set to WARNING");
    }
    
    // Set to debug level to see all messages
    logger.set_log_level(LogLevel::DEBUG);
    logger.debug("Now debug messages will appear");
    
    return 0;
}
```

### Sharded Logging (Ultra-Fast)

The `LoggerWrapper` provides high-performance sharded logging across multiple files, ideal for applications that need to separate logs by different components or threads.

#### Basic Usage

```cpp
#include "loggerwrapper.h"

int main() {
    // Create a sharded logger with 3 shards, 10MB rotation
    LoggerWrapper wrapper(3, "my_app", 10 * 1024 * 1024);
    
    // Log to different shards
    wrapper.info(0, "Processing user data");
    wrapper.warn(1, "Low memory warning");
    wrapper.error(2, "Database connection failed");
    wrapper.debug(0, "Debug information for shard 0");
    
    // Ultra-fast logging to different shards (fmt-style)
    for (uint8_t i = 0; i < 3; ++i) {
        wrapper.info_fast(i, "Shard {} processed batch {}", i, 100 + i);
        wrapper.warn_fast(i, "Shard {} memory usage: {}%", i, 85.5);
        wrapper.error_fast(i, "Shard {} error code: {}", i, 404);
        wrapper.debug_fast(i, "Shard {} debug: x={} y={}", i, 42, 3.14);
    }
    return 0;
}
```

### Example Program

```cpp
#include "logger.h"
#include "loggerwrapper.h"
#include <iostream>
#include <chrono>
#include <sstream>
int main() {
    std::cout << "=== LoggerLib Ultra-Fast Logging Demo ===" << std::endl;
    
    // Example 1: Single logger with 10MB rotation
    Logger logger("logs/myapp.log", 10 * 1024 * 1024);
    
    logger.info_fast("Application started");
    logger.warn_fast("This is a test with {} items", 42);
    logger.error_fast("Error occurred at line {}", 123);
    logger.debug_fast("Debug info: x={} y={}", 42, 3.14);
    
    // Example 2: Sharded logger with 5MB rotation per shard
    LoggerWrapper wrapper(2, "logs/sharded", 5 * 1024 * 1024);
    wrapper.info_fast(0, "Shard 0 message");
    wrapper.info_fast(1, "Shard 1 message");
    
    // Example 3: Performance comparison (slow vs fast methods)
    std::cout << "\n=== Performance Test ===" << std::endl;
    
    // SLOW method (stringstream)
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        std::ostringstream oss;
        oss << "Processing request: " << i << " with data: " << "data" << i;
        logger.info(oss.str());
    }
    auto slow_end = std::chrono::high_resolution_clock::now();
    
    // FAST method (fmt-style)
    auto fast_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        logger.info_fast("Processing request: {} with data: {}{}", i, "data", i);
    }
    auto fast_end = std::chrono::high_resolution_clock::now();
    
    auto slow_duration = std::chrono::duration_cast<std::chrono::microseconds>(slow_end - start);
    auto fast_duration = std::chrono::duration_cast<std::chrono::microseconds>(fast_end - fast_start);
    
    std::cout << "Slow method (stringstream): " << slow_duration.count() << " microseconds" << std::endl;
    std::cout << "Fast method (fmt-style): " << fast_duration.count() << " microseconds" << std::endl;
    std::cout << "Speedup: " << static_cast<double>(slow_duration.count()) / fast_duration.count() << "x faster!" << std::endl;
    
    // Example 4: Small file rotation test (1KB)
    Logger test_logger("logs/rotation_test.log", 1024);
    for (int i = 0; i < 100; ++i) {
        test_logger.info_fast("Test log entry {} - This is a long message to fill up the file quickly for rotation testing.", i);
    }
    
    std::cout << "\nLogging completed! Check the generated log files in the logs folder:" << std::endl;
    std::cout << "- logs/myapp.log (10MB rotation)" << std::endl;
    std::cout << "- logs/sharded_shard_0.log (5MB rotation)" << std::endl;
    std::cout << "- logs/sharded_shard_1.log (5MB rotation)" << std::endl;
    std::cout << "- logs/rotation_test.log (1KB rotation - should have multiple .1.log, .2.log files)" << std::endl;
    
    return 0;
}
```

## CMake Integration

When using the installed library, you do NOT need to add any Quill or fmt includes or link to them directly. The logger library vendors and links everything you need.

**Example CMakeLists.txt for your project:**

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyApp)
set(CMAKE_CXX_STANDARD 17)

# Add LoggerLib headers
include_directories(/usr/local/include)

add_executable(myapp main.cpp)
target_link_libraries(myapp /usr/local/lib/libloggerlib.a pthread)
```

**Troubleshooting:**
- If you get linker errors about missing fmt or quill symbols, make sure you are linking to the correct static library and not mixing system fmt/quill with the vendored one.
- If you use CMake's `find_package`, ensure you do NOT link to system fmt/quill unless you want to override the vendored version.

---

**You can now use fmt-style logging everywhere in your code, with no extra includes or dependencies!**
