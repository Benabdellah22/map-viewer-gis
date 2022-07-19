#pragma once

#include <types.h>

#include <utils/exception.h>
#include <utils/singleton.h>
#include <utils/logger.h>
#include <utils/assert.h>
#include <utils/ref.h>

#include <core/window.h>

#include <debug/profiler.h>

namespace mv {

  /* Use this to create a graphics context and connect the window */
  /* with it. In this application this create a OpenGL function   */
  /* for the window represented by the `native_window_handle`.    */
  class GraphicsContext final : public Singleton<GraphicsContext> {
    REGISTER_SINGLETON_CLASS( GraphicsContext );
  protected:
    GraphicsContext( uintptr_t native_window_handle );
    virtual ~GraphicsContext( void );
  public:
    /* swap buffer in the swapchain */
    virtual void swap_buffers( void );

    /* clear the bound framebuffer */
    virtual void clear_buffer( void );

    /* resize the default swapchain framebuffer */
    virtual void resize_buffer( uint32_t width, uint32_t height );

    /* enable/disable vsync */
    virtual void set_vsync( bool enable );
    virtual bool get_vsync( void ) const;
  private:
    uintptr_t _native_window_handle = 0;
    bool _vsync = true;
  };

}
