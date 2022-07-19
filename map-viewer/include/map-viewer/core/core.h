#pragma once

#include <list>
#include <vector>
#include <any>

#include <base/event.h>
#include <utils/singleton.h>
#include <types.h>

#include <core/layer.h>
#include <debug/profiler.h>

namespace mv {

  class Core : public Singleton<Core> {
    REGISTER_SINGLETON_CLASS( Core );
  public:
    Core( uint32_t width, uint32_t height, const string& name );
    ~Core( void );
  public:
    /* run the application, this runs the runtime loop */
    void run( void );

    /* push and pop a layer */
    void push_layer( Ref<Layer> layer );
    void pop_layer( const string& layer_name );
  private:
    void update( float delta_time );
    void render( void );
    void imgui_render( void );
    bool on_event( Event& e );
    void process_events( void );
  private:
    bool _close_application = false;

    std::list<Ref<Layer>> _layers;

    std::vector<Event*> _event_queue;
  };

}
