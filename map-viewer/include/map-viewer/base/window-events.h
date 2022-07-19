#pragma once

#include <base/event.h>

namespace mv {

  /* window close event, `_should_close` is always true */
  class WindowCloseEvent : public Event {
  public:
    WindowCloseEvent( bool should_close ) :
      _should_close( should_close ) {}

    REGISTER_EVENT_TYPE( WindowClose );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_WINDOW );
    REGISTER_EVENT_CLONE( WindowCloseEvent, _should_close );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name();
      return oss.str();
    }

    inline bool close( void ) const { return _should_close; }
  private:
    bool _should_close;
  };

  /* window resize event */
  class WindowResizeEvent : public Event {
  public:
    WindowResizeEvent( uint32_t x_dim, uint32_t y_dim ) :
      _x_dim( x_dim ), _y_dim( y_dim ) {}

    REGISTER_EVENT_TYPE( WindowResize );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_WINDOW );
    REGISTER_EVENT_CLONE( WindowResizeEvent, _x_dim, _y_dim );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name() << ": " << _x_dim << ", " << _y_dim;
      return oss.str();
    }

    inline uint32_t x( void ) const { return _x_dim; }
    inline uint32_t y( void ) const { return _y_dim; }
  private:
    uint32_t _x_dim;
    uint32_t _y_dim;
  };

  /* window moved event */
  class WindowMoveEvent : public Event {
  public:
    WindowMoveEvent( uint32_t x_pos, uint32_t y_pos ) :
      _x_pos( x_pos ), _y_pos( y_pos ) {}

    REGISTER_EVENT_TYPE( WindowMove );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_WINDOW );
    REGISTER_EVENT_CLONE( WindowMoveEvent, _x_pos, _y_pos );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name() << ": " << _x_pos << ", " << _y_pos;
      return oss.str();
    }

    inline uint32_t x( void ) const { return _x_pos; }
    inline uint32_t y( void ) const { return _y_pos; }
  private:
    uint32_t _x_pos;
    uint32_t _y_pos;
  };

}