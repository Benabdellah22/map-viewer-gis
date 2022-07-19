#include <core/core.h>

#include <csetjmp>

#include <base/window-events.h>

#include <utils/logger.h>
#include <utils/file-reader.h>

#include <core/window.h>
#include <core/timer.h>
#include <core/input.h>

#include <graphics/context.h>
#include <graphics/renderstate.h>
#include <graphics/imguicontext.h>

namespace mv {

  Core::Core( uint32_t width, uint32_t height, const string& name ) {
    /* initialize logger */
    Log::init();

    /* create window instance */
    Window::create_instance( width, height, name );
    Window::ref().bind_event_func( BIND_EVENT_FN( Core::on_event ) );

    /* create graphics context */
    GraphicsContext::create_instance( Window::ref().window_handle() );

    /* create imgui context */
    ImGuiContext::create_instance();

    /* create render state */
    RenderState::create_instance();

    /* push a viewport which covers the entire screen */
    RenderState::ref().push_viewport( { 0, 0, width, height, 0.0f, 1.0f } );
  
  }

  Core::~Core( void ) {

    /* pop the layers */
    {
      std::vector<string> layer_names( _layers.size() );
      for ( const auto& layer : _layers )
        layer_names.push_back( layer->name() );

      for ( const auto& layer_name : layer_names )
        pop_layer( layer_name );
    }
    

    /* pop the viewport pushed in the constructor */
    RenderState::ref().pop_viewport();

    /* destroy the modules */
    RenderState::destory_instance();
    ImGuiContext::destory_instance();
    GraphicsContext::destory_instance();
    Window::destory_instance();
  }

  void Core::run( void ) {
    while ( !_close_application ) {
      PROFILE_FUNCTION();

      /* update the timer */
      Timer::update();

      /* update the input class */
      Input::update();

      /* poll window events for */
      Window::ref().poll_event();

      /* process the captured events */
      process_events();

      /* update the layers */
      update( Timer::delta_time() );

      /* clear the buffer for drawing */
      GraphicsContext::ref().clear_buffer();

      /* render */
      {
        /* render function */
        render();
      }
      
      /* draw imgui ui */
      {
        /* imgui render function */
        imgui_render();
      }

      /* swap the drawn buffer with the screen framebuffer */
      GraphicsContext::ref().swap_buffers();
    }
  }

  void Core::push_layer( Ref<Layer> layer ) {
    for ( const auto& l : _layers ) {
      if ( l->name() == layer->name() ) {
        LOG_ERROR( "layer with name `{}` already pushed, cannot add duplicate layer", layer->name() );
        return;
      }
    }
    
    _layers.push_back( layer );

    LOG_TRACE( "layer pushed -> {}", layer->name() );
  }

  void Core::pop_layer( const string& layer_name ) {
    _layers.remove_if( [&] ( const Ref<Layer>& layer ) -> bool {
      if ( layer->name() == layer_name ) {
        LOG_TRACE( "layer popped -> {}", layer_name );
        return true;
      }
      return false;
    });
  }

  void Core::update( float delta_time ) {
    PROFILE_FUNCTION();

    // LOG_INFO("FPS: {}", Timer::fps());

    RenderState::ref().update();

    for ( auto& layer : _layers )
      layer->update( delta_time );
  }

  void Core::render( void ) {
    PROFILE_FUNCTION();

    for ( auto& layer : _layers )
      layer->render();
  }

  void Core::imgui_render( void ) {
    PROFILE_FUNCTION();

    ImGuiContext::ref().begin_frame();

    for ( auto& layer : _layers )
      layer->on_imgui_render();

    ImGuiContext::ref().end_frame();
  }

  bool Core::on_event( Event& e ) {
    PROFILE_FUNCTION();

    /* pass the event to ImGui and check weather */
    /* ImGui want to capture the event           */
    ImGuiContext::ref().on_event( e );
    if ( e.handled() )
      return false;

    /* simply push the events to the queue */
    _event_queue.push_back( e.clone() );

    return false;
  }

  void Core::process_events( void ) {
    PROFILE_FUNCTION();
    
    /* process the events */
    for ( auto event : _event_queue ) {
      Event& e = *event;

      /* handle window close */
      if ( e.type() == EventType::WindowClose )
        _close_application = true;

      /* pass events to input */
      Input::on_event( e );

      /* pass events to the layers */
      for ( const auto& layer : _layers )
        layer->on_event( e );
    }

    /* remove the events from the queue */
    for ( auto event : _event_queue ) {
      delete event;
    }
    _event_queue.clear();

  }

}
