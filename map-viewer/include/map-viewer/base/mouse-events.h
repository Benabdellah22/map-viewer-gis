#pragma once

#include <base/event.h>
#include <base/mousecodes.h>

namespace mv {

  /* mouse pressed event */
  class MousePressedEvent : public Event {
  public:
    MousePressedEvent( MouseCode mouse_code ) :
      _mouse_code( mouse_code ) {}

    REGISTER_EVENT_TYPE( MousePressed );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_INPUT | EVENT_CATEGORY_MOUSE );
    REGISTER_EVENT_CLONE( MousePressedEvent, _mouse_code );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name() << ": " << _mouse_code;
      return oss.str();
    }

    inline MouseCode mouse_code( void ) const { return _mouse_code; }
  private:
    MouseCode _mouse_code;
  };

  /* mouse released event */
  class MouseReleasedEvent : public Event {
  public:
    MouseReleasedEvent( MouseCode mouse_code ) :
      _mouse_code( mouse_code ) {}

    REGISTER_EVENT_TYPE( MouseReleased );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_INPUT | EVENT_CATEGORY_MOUSE );
    REGISTER_EVENT_CLONE( MouseReleasedEvent, _mouse_code );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name() << ": " << _mouse_code;
      return oss.str();
    }

    inline MouseCode mouse_code( void ) const { return _mouse_code; }
  private:
    MouseCode _mouse_code;
  };

  /* mouse moved event */
  class MouseMovedEvent : public Event {
  public:
    MouseMovedEvent( float x_pos, float y_pos ) :
      _x_pos( x_pos ), _y_pos( y_pos ) {}

    REGISTER_EVENT_TYPE( MouseMoved );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_INPUT | EVENT_CATEGORY_MOUSE );
    REGISTER_EVENT_CLONE( MouseMovedEvent, _x_pos, _y_pos );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name() << ": " << _x_pos << ", " << _y_pos;
      return oss.str();
    }

    inline float x( void ) const { return _x_pos; }
    inline float y( void ) const { return _y_pos; }
  private:
    float _x_pos;
    float _y_pos;
  };

  /* mouse scroll event */
  class MouseScrolledEvent : public Event {
  public:
    MouseScrolledEvent( float x_pos, float y_pos ) :
      _x_scroll( x_pos ), _y_scroll( y_pos ) {}

    REGISTER_EVENT_TYPE( MouseScrolled );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_INPUT | EVENT_CATEGORY_MOUSE );
    REGISTER_EVENT_CLONE( MouseScrolledEvent, _x_scroll, _y_scroll );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name() << ": " << _x_scroll << ", " << _y_scroll;
      return oss.str();
    }

    inline float x( void ) const { return _x_scroll; }
    inline float y( void ) const { return _y_scroll; }
  private:
    float _x_scroll;
    float _y_scroll;
  };

}