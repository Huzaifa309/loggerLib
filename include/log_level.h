#pragma once

// Shared log level enum used by both qLogger and Sharded_Logger

enum class LogLevel {
  TRACE = 0,
  DEBUG = 1,
  INFO = 2,
  WARNING = 3,
  ERROR = 4,
  CRITICAL = 5
};