#pragma once

#include "log_level.h"
#include "quill_logging.h"
#include "sharded_logging.h"

// Optional: convenience aliases under a lightweight namespace
namespace Loggerlib {
using ::LogLevel;
using ::qLogger;
using ::ShardedLogger;

}  // namespace Loggerlib
