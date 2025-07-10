# LoggerLib

A C++ logging library that provides a clean, simple interface for logging with automatic Quill C++ integration (completely hidden from users).

## Features

- **Clean Interface**: No Quill dependencies exposed to users
- Single logger instance for basic logging needs
- Sharded logger wrapper for high-performance logging across multiple files
- Automatic Quill C++ integration (vendored and hidden)
- Static library (.a) output for easy integration
- CMake-based build system

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
git clone <your-repo-url>
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
    // Create a logger
    Logger logger("my_app.log");
    
    // Log messages
    logger.info("Application started");
    logger.warn("This is a warning");
    logger.error("An error occurred");
    logger.debug("Debug information");
    
    return 0;
}
```

### Formatted Logging

```cpp
#include "logger.h"
#include <sstream>

int main() {
    Logger logger("my_app.log");
    
    // Format messages using string streams
    std::ostringstream oss;
    oss << "Processing request: " << request_id;
    logger.info(oss.str());
    
    oss.str("");
    oss.clear();
    oss << "User " << username << " attempted " << attempts << " times";
    logger.warn(oss.str());
    
    return 0;
}
```

### Sharded Logging

```cpp
#include "loggerwrapper.h"
#include <sstream>

int main() {
    // Create a sharded logger with 3 shards
    LoggerWrapper wrapper(3, "my_app");
    
    // Log to different shards
    for (uint8_t i = 0; i < 3; ++i) {
        std::ostringstream oss;
        oss << "Message from shard " << static_cast<int>(i);
        wrapper.info(i, oss.str());
    }
    
    // Log to messaging shard
    wrapper.info_msg("Messaging log entry");
    
    return 0;
}
```

## Integration in Other Projects

### Using the installed library

```cmake
# In your CMakeLists.txt
find_package(Threads REQUIRED)
add_library(loggerlib STATIC IMPORTED)
set_target_properties(loggerlib PROPERTIES
    IMPORTED_LOCATION /usr/local/lib/libloggerlib.a
    INTERFACE_INCLUDE_DIRECTORIES /usr/local/include
)
target_link_libraries(your_target loggerlib Threads::Threads)
```

### Using the static library directly

```cmake
# In your CMakeLists.txt
find_package(Threads REQUIRED)
add_library(loggerlib STATIC IMPORTED)
set_target_properties(loggerlib PROPERTIES
    IMPORTED_LOCATION /path/to/libloggerlib.a
    INTERFACE_INCLUDE_DIRECTORIES /path/to/include
)
target_link_libraries(your_target loggerlib Threads::Threads)
```

### Manual linking

```bash
g++ -o myapp myapp.cpp -L/path/to/lib -lloggerlib -lpthread
```

## Generated Files

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

## Build Output

The build process generates:
- `lib/libloggerlib.a` - Static library
- `bin/example` - Example executable (if BUILD_EXAMPLE=ON)

## Key Benefits

1. **No Quill Dependencies**: Users never need to include or know about Quill
2. **Simple Interface**: Clean, straightforward logging API
3. **Vendored Dependencies**: Everything is self-contained
4. **Thread-Safe**: Built on Quill's thread-safe foundation
5. **High Performance**: Leverages Quill's asynchronous logging

## License

[Add your license information here] 