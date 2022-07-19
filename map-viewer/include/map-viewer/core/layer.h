#pragma once

#include <types.h>
#include <base/event.h>
#include <utils/ref.h>

#include <imgui.h>

namespace mv {

  class Layer : public SharedObject {
  public:
    Layer( const string& name ) :
      _layer_name( name ) {}
    virtual ~Layer( void ) {}
  public:
    virtual void update( float delta_time ) {}
    virtual void render( void ) {}
    virtual void on_imgui_render( void ) {}
    virtual void on_event( Event& e ) {}

    inline string name( void ) const { return _layer_name; }
  private:
    string _layer_name;
  };

}
