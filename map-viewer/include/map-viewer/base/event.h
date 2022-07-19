#pragma once

#include <functional>

#include <base/events.h>
#include <types.h>

namespace mv {

  #define BIND_EVENT_FN( fn ) std::bind( &fn, this, std::placeholders::_1 )

  class Event;

  using event_emit_func = std::function<bool( Event& e )>;

  /* Event wrapper class, whenever any event is dispatched */
  /* in the application, it's base class is always `Event` */
  /*  */
  class Event {
    friend class EventDispatcher;
  protected:
    Event() {}
  public:
    virtual ~Event() {}
  public:
    virtual EventType type( void ) const = 0;
    virtual string    name( void ) const = 0;
    virtual int32_t   category( void ) const = 0;
    virtual string    to_string( void ) const = 0;
    virtual Event*    clone( void ) const = 0;

    inline bool handled( void ) const { return _handled; }
  private:
    bool _handled = false;
  };

  /* Used in event class declaration to automatically handle pure */
  /* virtual functions and a static type function.                */
  #define REGISTER_EVENT_TYPE( T )                                                   \
            virtual EventType type(  void ) const override { return  EventType::T; } \
            virtual string    name(  void ) const override { return #T; }            \
            static  EventType static_type( void ) { return EventType::T; }

  #define REGISTER_EVENT_CATEGORY( C ) \
            virtual int32_t category( void ) const override { return C; }

  #define REGISTER_EVENT_CLONE( E, ... ) virtual Event* clone( void ) const override { return dynamic_cast<Event*>( new E( __VA_ARGS__ ) ); }

  /* Convenient wrapper to handle events, this also takes    */
  /* care weather the event is handled or not. If the event  */
  /* is already handled then the corresponding function will */
  /* not called.                                             */
  class EventDispatcher {
  public:
    EventDispatcher( Event& e ) : _e( e ) {}

    template<typename T>
    void dispatch( std::function<bool( T& e )> func ) {
      if ( !_e._handled && func && _e.type() == T::static_type() ) {
        T& e = *dynamic_cast<T*>( &_e );
        _e._handled = func( e );
      }
    }

    template<>
    void dispatch<Event>( std::function<bool( Event& e )> func ) {
      if ( !_e._handled && func ) {
        _e._handled = func( _e );
      }
    }
  private:
    Event& _e;
  };

}
