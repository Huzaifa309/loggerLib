#include "logger.h"
#include "loggerwrapper.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <string>

int main() {
    std::cout << "=== LoggerLib Ultra-Fast Logging Demo ===" << std::endl;
    
    // Example 1: Single logger with 10MB rotation in logs folder
    Logger logger("logs/myapp.log", 10 * 1024 * 1024); // 10MB rotation
    
    logger.info_fast("Application started");
    logger.warn_fast("This is a test with ", 42, " items");
    logger.error_fast("Error occurred at ", "line ", 123);
    logger.debug_fast("Debug info: x=", 42, " y=", 3.14);
    
    // Generate enough data to trigger rotation (10MB = ~10,485,760 bytes)
    std::cout << "Generating data to trigger 10MB rotation..." << std::endl;
    std::string large_message = "This is a very long log message that contains a lot of data to fill up the log file quickly. ";
    large_message += "It includes multiple sentences and various types of information like user IDs, timestamps, ";
    large_message += "error codes, and detailed descriptions of what happened in the system. ";
    large_message += "This message is designed to be approximately 200 characters long to help us reach the 10MB limit faster. ";
    
    // Write enough messages to exceed 10MB (will take more iterations)
    for (int i = 0; i < 50000; ++i) {
        logger.info_fast("Log entry ", i, ": ", large_message, "Additional data: ", i * 1000);
        if (i % 10000 == 0) {
            std::cout << "Generated " << i << " log entries..." << std::endl;
        }
    }
    
    // Example 2: Sharded logger with 5MB rotation per shard in logs folder
    LoggerWrapper wrapper(2, "logs/sharded", 5 * 1024 * 1024); // 5MB rotation
    wrapper.info_fast(0, "Shard 0 message");
    wrapper.info_fast(1, "Shard 1 message");
    wrapper.info_fast(0, "General message on shard 0"); // Use any shard for general messages
    
    // Generate data for shards to trigger rotation
    std::cout << "Generating data for sharded logs..." << std::endl;
    for (int i = 0; i < 20000; ++i) {
        wrapper.info_fast(0, "Shard 0 - Log entry ", i, ": ", large_message, "Shard data: ", i * 500);
        wrapper.info_fast(1, "Shard 1 - Log entry ", i, ": ", large_message, "Shard data: ", i * 500);
        if (i % 5000 == 0) {
            std::cout << "Generated " << i << " sharded log entries..." << std::endl;
        }
    }
    
    // Example 3: Performance test
    std::cout << "\n=== Performance Test ===" << std::endl;
    
    // SLOW method (stringstream)
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        std::ostringstream oss;
        oss << "Processing request: " << i << " with data: " << "data" << i;
        logger.info(oss.str());
    }
    auto slow_end = std::chrono::high_resolution_clock::now();
    
    // FAST method (direct concatenation)
    auto fast_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        logger.info_fast("Processing request: ", i, " with data: ", "data", i);
    }
    auto fast_end = std::chrono::high_resolution_clock::now();
    
    auto slow_duration = std::chrono::duration_cast<std::chrono::microseconds>(slow_end - start);
    auto fast_duration = std::chrono::duration_cast<std::chrono::microseconds>(fast_end - fast_start);
    
    std::cout << "Slow method (stringstream): " << slow_duration.count() << " microseconds" << std::endl;
    std::cout << "Fast method (direct): " << fast_duration.count() << " microseconds" << std::endl;
    std::cout << "Speedup: " << static_cast<double>(slow_duration.count()) / fast_duration.count() << "x faster!" << std::endl;
    
    // Example 4: Small file size for testing rotation (1KB) in logs folder
    Logger test_logger("logs/rotation_test.log", 1024); // 1KB rotation
    std::cout << "Generating data for 1KB rotation test..." << std::endl;
    for (int i = 0; i < 100; ++i) {
        test_logger.info_fast("Test log entry ", i, " - This is a long message to fill up the file quickly for rotation testing. ", 
                             "Additional data: ", i * 100, " More text to ensure we reach the 1KB limit and trigger rotation.");
    }
    
    std::cout << "\nLogging completed! Check the generated log files in the logs folder:" << std::endl;
    std::cout << "- logs/myapp.log (10MB rotation)" << std::endl;
    std::cout << "- logs/sharded_shard_0.log (5MB rotation)" << std::endl;
    std::cout << "- logs/sharded_shard_1.log (5MB rotation)" << std::endl;
    std::cout << "- logs/rotation_test.log (1KB rotation - should have multiple .1.log, .2.log files)" << std::endl;
    
    return 0;
} 