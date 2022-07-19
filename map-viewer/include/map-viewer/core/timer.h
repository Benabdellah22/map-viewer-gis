#pragma once

/* we use chrono to keep track of time */
#include <chrono>

namespace mv {

  using time_point = std::chrono::time_point<std::chrono::steady_clock>;
  using time_type  = float;

  /* Sime timer class, can be used to get time since   */
  /* the start of the application; delta time and fps. */
  class Timer {
  public:
    /* Update must be called every frame to update */
    /* the time and compute delta time.            */
    static void update( void );
    static time_type time( void );
    static time_type delta_time( void );
    static time_type fps( void );
    static uint32_t last_frame_fps( void );
  private:
    static std::chrono::duration<float> s_delta_time;
    static time_point s_start;
    static time_point s_time;

    static uint32_t s_last_fps_second;
    static uint32_t s_int_fps;
    static uint32_t s_last_frame_fps;
  };

  /* This can be used to capture time of any scope.   */
  /* The constructor and destructor capture the time  */
  /* the at there corresponding duration and the then */
  /* the difference is printed upon destruction.      */
  class TimeThis {
  public:
    TimeThis( bool is_ms = false );
    ~TimeThis( void );
  private:
    time_type _start_time = 0.0f;
    bool _is_ms = false;
  };

  #define TIME_THIS_SCOPE_MS() TimeThis time_this( true )

}
