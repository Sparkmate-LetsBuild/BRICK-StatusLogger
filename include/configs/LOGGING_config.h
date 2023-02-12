#pragma once
#define LOGGING_config_h

// -- STATUS LOGGER HANDLER
#define LOG_SIZE 4096
#define LOG_LEVEL_SAVE 2  // 0 is log nothing, 1 is errors, 2 is +reports/stats, 3 is warnings, 4 is everything else (see ../bricks/status_logger.h)
#define LOG_LEVEL_PRINT 3 // 0 is log nothing, 1 is errors, 2 is +reports/stats, 3 is warnings, 4 is everything else (see ../bricks/status_logger.h)
