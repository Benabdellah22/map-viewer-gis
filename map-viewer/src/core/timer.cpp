#include <core/timer.h>

#include <utils/logger.h>

namespace mv {

  std::chrono::duration<float> Timer::s_delta_time;
  time_point Timer::s_start = std::chrono::high_resolution_clock::now();
  time_point Timer::s_time = std::chrono::high_resolution_clock::now();
  uint32_t Timer::s_last_fps_second = 0;
  uint32_t Timer::s_int_fps = 0;
  uint32_t Timer::s_last_frame_fps = 0;

  void Timer::update( void ) {
    /* update the time and compute delta time */
    auto current_time = std::chrono::high_resolution_clock::now();
    s_delta_time = current_time - s_time;
    s_time = current_time;

    /* compute frames */
    if ( (uint32_t)time() > s_last_fps_second ) {
      s_last_fps_second = (uint32_t)time();
      s_last_frame_fps = s_int_fps;
      s_int_fps = 0;
    }

    s_int_fps++;
  }

  time_type Timer::time( void ) {
    /* return current time */
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - s_start );
    return (time_type)duration.count() / (time_type)1000000;
  }

  time_type Timer::delta_time( void ) {
    /* return previous frame's delta time */
    return s_delta_time.count();
  }

  time_type Timer::fps( void ) {
    /* return fps */
    time_type delta = delta_time();
    return (time_type)1 / delta;
  }

  uint32_t Timer::last_frame_fps( void ) {
    return s_last_frame_fps;
  }

  TimeThis::TimeThis( bool is_ms /*= false */ ) :
    _is_ms( is_ms ) {
    _start_time = Timer::time();
  }

  TimeThis::~TimeThis( void ) {
    time_type diff = Timer::time() - _start_time;

    if ( _is_ms )
      LOG_WARN( "Scope Took: {}ms", diff * (time_type)1000 );
    else
      LOG_WARN( "Scope Took: {}s", diff );

  }

}
