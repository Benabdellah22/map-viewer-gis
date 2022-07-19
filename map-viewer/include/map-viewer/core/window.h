#pragma once

#include <base/event.h>
#include <utils/singleton.h>
#include <types.h>

struct GLFWwindow;

namespace mv {

  /* Represents a window object that can be used */
  /* as a render target.                         */
  class Window final : public Singleton<Window> {
    REGISTER_SINGLETON_CLASS( Window );
    friend void window_size_callback( ::GLFWwindow* window, int x, int y );
  protected:
    Window( uint32_t width, uint32_t height, const std::string& name );
    ~Window( void );
  public:
    void poll_event( void ); /* must be called every frame */

    void bind_event_func( event_emit_func func );

    inline uint32_t width( void ) const { return _width; }
    inline uint32_t height( void ) const { return _height; }
    inline event_emit_func get_event_function( void ) const { return _event_func; }
    inline uintptr_t window_handle( void ) const { return _native_window_handle; }
  private:
    /* We use `uintptr_t` so that we don't have to      */
    /* include platform dependent code and to simplify  */
    /* the code as well. This can be anything, for this */
    /* application this handle is a `GLFWwindow*`.      */
    uintptr_t _native_window_handle = 0;

    uint32_t  _width                = 0;
    uint32_t  _height               = 0;

    /* This function is called whenever the window has to */
    /* dispatch any event. Do not confuse this with event */
    /* handling function, this one create events rather   */
    /* than consuming them. Use this as a callback.       */
    event_emit_func _event_func;
  };

}
