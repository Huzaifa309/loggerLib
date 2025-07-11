# LoggerLib

A C++ logging library that provides a clean, simple interface for logging with automatic Quill C++ integration (completely hidden from users).

## Features

- **Ultra-Fast Logging**: Direct variadic logging methods with zero stringstream overhead
- **Clean Interface**: No Quill dependencies exposed to users
- **Smart Log Rotation**: Automatic file rotation with timestamped filenames
- **PIMPL Pattern**: Complete encapsulation of internal implementation details
- Single logger instance for basic logging needs
- Sharded logger wrapper for high-performance logging across multiple files
- Automatic Quill C++ integration (vendored and hidden)
- Static library (.a) output for easy integration
- CMake-based build system

## Key Benefits

1. **Ultra-Fast Logging**: Direct variadic logging, no stringstream overhead
2. **Smart Rotation**: Automatic file rotation with timestamped filenames
3. **Complete Encapsulation**: Users never need to include or know about Quill
4. **Simple Interface**: Clean, straightforward logging API
5. **Vendored Dependencies**: Everything is self-contained
6. **Thread-Safe**: Built on Quill's thread-safe foundation
7. **High Performance**: Leverages Quill's asynchronous logging
8. **Configurable**: Easy to adjust rotation sizes and other settings

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

### Sharded Logging (Ultra-Fast)

```cpp
#include "loggerwrapper.h"

int main() {
    // Create a sharded logger with 3 shards
    LoggerWrapper wrapper(3, "my_app");
    
    // Log to different shards (ultra-fast)
    for (uint8_t i = 0; i < 3; ++i) {
        wrapper.info_fast(i, "Shard ", static_cast<int>(i), " processed batch ", 100 + i);
    }
    
    // Log to messaging shard (ultra-fast)
    wrapper.info_msg_fast("Received message from user: ", "bob");
    return 0;
}
```

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
- `{prefix}_messaging.log` - Messaging log file

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
- `lib/libloggerlib.a` - Static library
- `bin/example` - Example executable (if BUILD_EXAMPLE=ON)

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
- **Private Linking**: Quill linked privately, not exposed to consumers
- **Fixed Log Rotation**: Improved rotation behavior with proper timestamped filenames
- **Better File Naming**: Rotated files now include creation timestamps for easy identification
- **Proper Configuration**: Updated Quill integration for optimal performance and reliability
