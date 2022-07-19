#pragma once

#include <string>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#pragma warning( push, 0 )
# include <spdlog/spdlog.h>
# include <spdlog/sinks/stdout_sinks.h>
# include <spdlog/sinks/stdout_color_sinks.h>
# include <spdlog/pattern_formatter.h>
#pragma warning( pop )

namespace mv {

  /* Logger is a completely static to that it can be  */
  /* accessed anywhere without creating an instance.  */
  /* Due to it's static nature the `init` function    */
  /* must be called to initialize internal variables. */
  class Log {
  public:
    /* initialize variables and pattern */
    static void init( void );

    /* return logger */
    static std::shared_ptr<spdlog::logger> get_logger( void );
  private:
    static std::shared_ptr<spdlog::logger> s_logger; /* spdlog logger object */
  };


}

/* logging macros to log filename and line number before the message */
#define LOG_TRACE( ... )    SPDLOG_LOGGER_TRACE( mv::Log::get_logger(), __VA_ARGS__ )
#define LOG_DEBUG( ... )    SPDLOG_LOGGER_DEBUG( mv::Log::get_logger(), __VA_ARGS__ )
#define LOG_INFO( ... )     SPDLOG_LOGGER_INFO( mv::Log::get_logger(), __VA_ARGS__ )
#define LOG_WARN( ... )     SPDLOG_LOGGER_WARN( mv::Log::get_logger(), __VA_ARGS__ )
#define LOG_ERROR( ... )    SPDLOG_LOGGER_ERROR( mv::Log::get_logger(), __VA_ARGS__ )
#define LOG_CRITICAL( ... ) SPDLOG_LOGGER_CRITICAL( mv::Log::get_logger(), __VA_ARGS__ )