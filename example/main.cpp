#include "logger.h"
#include "loggerwrapper.h"
#include <iostream>
#include <sstream>
#include <chrono> // Added for performance comparison

int main() {
    // Example 1: Single logger with clean interface
    Logger logger("example.log");
    
    // Simple logging
    logger.info("Hello from single logger!");
    logger.warn("This is a warning message");
    logger.error("This is an error message");
    logger.debug("This is a debug message");
    
    // ULTRA-FAST logging (no stringstream overhead!)
    logger.info_fast("Processing request: ", "REQ123");
    logger.warn_fast("User ", "john", " attempted ", 5, " times");
    logger.error_fast("Failed to connect to ", "localhost", ":", 8080);
    logger.debug_fast("Debug info: ", "value=", 42, " status=", "ok");
    
    // Example 2: Sharded logger wrapper with clean interface
    LoggerWrapper wrapper(3, "sharded_example");
    
    // Log to different shards using ultra-fast methods
    for (uint8_t i = 0; i < 3; ++i) {
        wrapper.info_fast(i, "Message from shard ", static_cast<int>(i));
        wrapper.info_fast(i, "Processing shard ", static_cast<int>(i), " with data: data", static_cast<int>(i));
    }
    
    // Log to messaging shard using ultra-fast methods
    wrapper.info_msg_fast("This is a messaging log entry");
    wrapper.info_msg_fast("Received message from user: ", "alice");
    
    // Performance comparison
    std::cout << "=== Performance Test ===" << std::endl;
    
    // SLOW method (stringstream)
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; ++i) {
        std::ostringstream oss;
        oss << "Processing request: " << i << " with data: " << "data" << i;
        logger.info(oss.str());
    }
    auto slow_end = std::chrono::high_resolution_clock::now();
    
    // FAST method (direct concatenation)
    auto fast_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; ++i) {
        logger.info_fast("Processing request: ", i, " with data: ", "data", i);
    }
    auto fast_end = std::chrono::high_resolution_clock::now();
    
    auto slow_duration = std::chrono::duration_cast<std::chrono::microseconds>(slow_end - start);
    auto fast_duration = std::chrono::duration_cast<std::chrono::microseconds>(fast_end - fast_start);
    
    std::cout << "Slow method (stringstream): " << slow_duration.count() << " microseconds" << std::endl;
    std::cout << "Fast method (direct): " << fast_duration.count() << " microseconds" << std::endl;
    std::cout << "Speedup: " << static_cast<double>(slow_duration.count()) / fast_duration.count() << "x faster!" << std::endl;
    
    std::cout << "Logging examples completed. Check the generated log files." << std::endl;
    return 0;
} 