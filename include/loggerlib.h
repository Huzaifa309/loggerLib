#pragma once

#include "log_level.h"
#include "quill_logging.h"
#include "sharded_logging.h"

// Optional: convenience aliases under a lightweight namespace
namespace loggerlib {
using ::LogLevel;
using ::qLogger;
using ::Sharded_Logger;
} // namespace loggerlib