# LoggerLib

A C++ logging library that provides a clean, simple interface for logging with automatic Quill C++ integration (completely hidden from users).

## Features

- **Ultra-Fast Logging**: Direct variadic logging methods with zero stringstream overhead
- **Clean Interface**: No Quill dependencies exposed to users
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
    
    // Ultra-fast logging (no stringstream, direct variadic)
    logger.info_fast("User ", "alice", " logged in at ", 1234567890);
    logger.warn_fast("Low disk space: ", 42, "% remaining");
    logger.error_fast("Failed to open file: ", "/tmp/data.txt");
    logger.debug_fast("Debug: x=", 42, " y=", 3.14);
    return 0;
}
```

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

### Available Log Levels

```cpp
enum class LogLevel {
    DEBUG,    // Most verbose - shows all messages
    INFO,     // Information messages
    WARNING,  // Warning messages
    ERROR     // Only error messages
};
```

### Sharded Logging (Ultra-Fast)

The `LoggerWrapper` provides high-performance sharded logging across multiple files, ideal for applications that need to separate logs by different components or threads.

#### Constructor
```cpp
LoggerWrapper(uint8_t shard_count, const std::string& log_file_prefix, size_t max_file_size = 0)
```

**Parameters:**
- `shard_count`: Number of shards (0-255)
- `log_file_prefix`: Base filename for log files
- `max_file_size`: Maximum file size in bytes before rotation (0 = no rotation)

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
    
    return 0;
}
```

#### Ultra-Fast Logging

```cpp
#include "loggerwrapper.h"

int main() {
    // Create a sharded logger with 3 shards
    LoggerWrapper wrapper(3, "my_app");
    
    // Ultra-fast logging to different shards (no stringstream overhead)
    for (uint8_t i = 0; i < 3; ++i) {
        wrapper.info_fast(i, "Shard ", static_cast<int>(i), " processed batch ", 100 + i);
        wrapper.warn_fast(i, "Shard ", static_cast<int>(i), " memory usage: ", 85.5, "%");
        wrapper.error_fast(i, "Shard ", static_cast<int>(i), " error code: ", 404);
        wrapper.debug_fast(i, "Shard ", static_cast<int>(i), " debug: x=", 42, " y=", 3.14);
    }
    
    return 0;
}
```

#### Available Methods

**Standard Logging (with string formatting):**
- `info(shard_id, message)` - Log info message to specified shard
- `warn(shard_id, message)` - Log warning message to specified shard  
- `error(shard_id, message)` - Log error message to specified shard
- `debug(shard_id, message)` - Log debug message to specified shard

**Ultra-Fast Logging (direct variadic, no stringstream):**
- `info_fast(shard_id, args...)` - Fast info logging with variadic arguments
- `warn_fast(shard_id, args...)` - Fast warning logging with variadic arguments
- `error_fast(shard_id, args...)` - Fast error logging with variadic arguments
- `debug_fast(shard_id, args...)` - Fast debug logging with variadic arguments

**Error Handling:**
- Invalid shard IDs are safely ignored (no crash)
- Shard ID must be less than the total number of shards
- Each shard operates independently with its own log file

#### Log Level Control

```cpp
#include "loggerwrapper.h"

int main() {
    LoggerWrapper wrapper(3, "my_app");
    
    // Set log level for a specific shard
    wrapper.set_log_level(0, LogLevel::DEBUG);  // Shard 0: show all messages
    wrapper.set_log_level(1, LogLevel::WARNING); // Shard 1: only warnings and errors
    wrapper.set_log_level(2, LogLevel::ERROR);   // Shard 2: only errors
    
    // Set log level for all shards at once
    wrapper.set_log_level_all(LogLevel::INFO);
    
    // Check current log level of a shard
    LogLevel current_level = wrapper.get_log_level(0);
    
    // Test logging with different levels
    wrapper.debug(0, "This will appear (DEBUG level)");
    wrapper.info(1, "This will appear (INFO level)");
    wrapper.warn(2, "This will appear (WARNING level)");
    wrapper.error(2, "This will appear (ERROR level)");
    
    return 0;
}
```

**Available Log Level Methods:**
- `set_log_level(shard_id, level)` - Set log level for a specific shard
- `set_log_level_all(level)` - Set log level for all shards
- `get_log_level(shard_id)` - Get current log level of a specific shard

### Log Rotation

The library supports automatic log rotation with configurable file sizes:

```cpp
// Create logger with 1MB rotation
Logger logger("app.log", 1024 * 1024);

// Create logger with 100MB rotation  
Logger logger("app.log", 100 * 1024 * 1024);

// Create logger without rotation
Logger logger("app.log", 0);
```

When rotation occurs, files are renamed with timestamps:
- `app.log` - Current log file
- `app_20250710_164300.1.log` - First rotated file (July 10, 2025 at 16:43:00)
- `app_20250710_164126.2.log` - Second rotated file
- etc.

## Integration in Other Projects

### Using the pre-built library (Recommended)

The library comes with a pre-built static library in the `lib/` folder:

```cmake
# In your CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(MyApp)

set(CMAKE_CXX_STANDARD 17)

# Add LoggerLib headers
include_directories(${CMAKE_SOURCE_DIR}/path/to/loggerLib/include)

# Add LoggerLib library
link_directories(${CMAKE_SOURCE_DIR}/path/to/loggerLib/lib)

add_executable(myapp main.cpp)
target_link_libraries(myapp loggerlib pthread)
```

### Using the installed library

```cmake
# In your CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(MyApp)

set(CMAKE_CXX_STANDARD 17)
include_directories(/usr/local/include)

add_executable(myapp main.cpp)
target_link_libraries(myapp /usr/local/lib/libloggerlib.a)
```

### Using pkg-config

```bash
g++ -std=c++17 $(pkg-config --cflags --libs loggerlib) -o myapp main.cpp
```

### Manual linking

```bash
# Using pre-built library
g++ -std=c++17 -I/path/to/loggerLib/include -L/path/to/loggerLib/lib -lloggerlib -pthread -o myapp main.cpp

# Using installed library
g++ -std=c++17 -I/usr/local/include -L/usr/local/lib -lloggerlib -pthread -o myapp main.cpp
```

### Using as Git submodule

```bash
# In your project
git submodule add https://github.com/Huzaifa309/loggerLib third_party/loggerLib

# In your CMakeLists.txt
add_subdirectory(third_party/loggerLib)
target_link_libraries(myapp loggerlib)
```

## Generated Files

### Basic Logger
When using the basic logger with rotation:
- `app.log` - Current log file
- `app_YYYYMMDD_HHMMSS.1.log` - Rotated files with timestamps

### Sharded Logger
When using the sharded logger, the following files are created:
- `{prefix}_shard_0.log` - First shard log file
- `{prefix}_shard_1.log` - Second shard log file
- `{prefix}_shard_2.log` - Third shard log file
- etc.

## Log Format

Log entries follow this format:
```
[2025-07-10 15:39:20.020] [INFO] [12718] [example.log] Your message here
```

Components:
- `[2025-07-10 15:39:20.020]` - Timestamp with milliseconds
- `[INFO]` - Log level (DEBUG, INFO, WARNING, ERROR)
- `[12718]` - Process ID
- `[example.log]` - Logger name (filename)
- `Your message here` - The actual log message

## Build Output

The build process generates:
- `build/lib/libloggerlib.a` - Static library (in build directory)
- `build/bin/example` - Example executable (if BUILD_EXAMPLE=ON)

### Distribution

After building, you can copy the library to the distribution folder:

```bash
# Use the provided script
./copy_lib.sh

# Or manually
mkdir -p lib
cp build/lib/libloggerlib.a lib/
```

The `lib/` folder contains the pre-built static library ready for distribution.

## Architecture

### PIMPL Pattern
LoggerLib uses the PIMPL (Pointer to Implementation) pattern to completely hide the Quill dependency:

- **Public Interface**: Clean, minimal headers with no Quill dependencies
- **Private Implementation**: All Quill logic encapsulated in implementation files
- **Template Methods**: Fast logging methods use only standard types and internal forwarding

### Encapsulation Benefits
- **No Transitive Dependencies**: Users don't need to include or link Quill
- **Clean API**: Simple interface without implementation details
- **Easy Integration**: Drop-in logging solution for any C++ project
- **Version Independence**: Internal Quill version changes don't affect users

## Recent Improvements

- **PIMPL Implementation**: Complete encapsulation using Pointer to Implementation pattern
- **Clean Headers**: Removed all Quill dependencies from public interface
- **Manual Log Level Control**: Added runtime log level configuration
- **Fixed Log Rotation**: Improved rotation behavior with proper timestamped filenames
- **Proper Configuration**: Updated Quill integration for optimal performance and reliability

## Example Program

The included example demonstrates all features:

```cpp
#include "logger.h"
#include <iostream>

int main() {
    // Create logger with 1MB rotation
    Logger logger("example.log", 1024 * 1024);
    
    // Test different log levels
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warn("Warning message");
    logger.error("Error message");
    
    // Test fast logging
    logger.info_fast("Fast logging: ", "value=", 42, " time=", 1234567890);
    
    // Test log level control
    logger.set_log_level(LogLevel::WARNING);
    logger.info("This won't appear (level too low)");
    logger.warn("This will appear");
    
    logger.set_log_level(LogLevel::DEBUG);
    logger.debug("Now debug messages appear again");
    
    std::cout << "Logging complete. Check example.log for output." << std::endl;
    return 0;
}
```

Build and run the example:
```bash
cd build
cmake -DBUILD_EXAMPLE=ON ..
make
./bin/example
```
