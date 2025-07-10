#include "logger.h"
#include "loggerwrapper.h"
#include <iostream>
#include <sstream>

int main() {
    // Example 1: Single logger with clean interface
    Logger logger("example.log");
    
    // Simple logging
    logger.info("Hello from single logger!");
    logger.warn("This is a warning message");
    logger.error("This is an error message");
    logger.debug("This is a debug message");
    
    // Formatted logging using string streams
    std::ostringstream oss;
    oss << "Processing request: " << "REQ123";
    logger.info(oss.str());
    
    oss.str("");
    oss.clear();
    oss << "User john attempted " << 5 << " times";
    logger.warn(oss.str());
    
    oss.str("");
    oss.clear();
    oss << "Failed to connect to localhost:8080";
    logger.error(oss.str());
    
    // Example 2: Sharded logger wrapper with clean interface
    LoggerWrapper wrapper(3, "sharded_example");
    
    // Log to different shards
    for (uint8_t i = 0; i < 3; ++i) {
        oss.str("");
        oss.clear();
        oss << "Message from shard " << static_cast<int>(i);
        wrapper.info(i, oss.str());
        
        oss.str("");
        oss.clear();
        oss << "Processing shard " << static_cast<int>(i) << " with data: data" << static_cast<int>(i);
        wrapper.info(i, oss.str());
    }
    
    // Log to messaging shard
    wrapper.info_msg("This is a messaging log entry");
    wrapper.info_msg("Received message from user: alice");
    
    std::cout << "Logging examples completed. Check the generated log files." << std::endl;
    return 0;
} 