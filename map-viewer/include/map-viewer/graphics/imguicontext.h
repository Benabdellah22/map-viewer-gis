#pragma once

#include <base/event.h>
#include <utils/singleton.h>

namespace mv {

  class ImGuiContext : public Singleton<ImGuiContext> {
    REGISTER_SINGLETON_CLASS( ImGuiContext );
  public:
    ImGuiContext( void );
    ~ImGuiContext( void );
  public:
    void begin_frame( void );
    void end_frame( void );
    void on_event( Event& e );

    void block_events( bool block ) { _block_events = block; }
  private:
    bool _block_events = true;
  };

}
