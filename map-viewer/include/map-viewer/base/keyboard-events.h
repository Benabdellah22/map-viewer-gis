#pragma once

#include <base/event.h>
#include <base/keycodes.h>

namespace mv {

  /* key pressed event */
  class KeyPressedEvent : public Event {
  public:
    KeyPressedEvent( KeyCode key_code ) :
      _key_code( key_code ) {}

    REGISTER_EVENT_TYPE( KeyPressed );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_INPUT | EVENT_CATEGORY_KEYBOARD );
    REGISTER_EVENT_CLONE( KeyPressedEvent, _key_code );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name() << ": " << _key_code;
      return oss.str();
    }

    inline KeyCode key_code( void ) const { return _key_code; }
  private:
    KeyCode _key_code;
  };

  /* key released event */
  class KeyReleasedEvent : public Event {
  public:
    KeyReleasedEvent( KeyCode key_code ) :
      _key_code( key_code ) {}

    REGISTER_EVENT_TYPE( KeyReleased );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_INPUT | EVENT_CATEGORY_KEYBOARD );
    REGISTER_EVENT_CLONE( KeyReleasedEvent, _key_code );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name() << ": " << _key_code;
      return oss.str();
    }

    inline KeyCode key_code( void ) const { return _key_code; }
  private:
    KeyCode _key_code;
  };

  /* key released event */
  class KeyRepeatEvent : public Event {
  public:
    KeyRepeatEvent( KeyCode key_code ) :
      _key_code( key_code ) {}

    REGISTER_EVENT_TYPE( KeyRepeat );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_INPUT | EVENT_CATEGORY_KEYBOARD );
    REGISTER_EVENT_CLONE( KeyRepeatEvent, _key_code );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name() << ": " << _key_code;
      return oss.str();
    }

    inline KeyCode key_code( void ) const { return _key_code; }
  private:
    KeyCode  _key_code;
  };

  /* key typed event; can be used to input text */
  class KeyTypedEvent : public Event {
  public:
    KeyTypedEvent( uint32_t character ) :
      _charcode( character ) {}
    REGISTER_EVENT_TYPE( KeyTyped );
    REGISTER_EVENT_CATEGORY( EVENT_CATEGORY_INPUT | EVENT_CATEGORY_KEYBOARD );
    REGISTER_EVENT_CLONE( KeyTypedEvent, _charcode );
  public:
    virtual string to_string( void ) const override {
      std::ostringstream oss;
      oss << name() << ": " << (char)_charcode;
      return oss.str();
    }

    inline uint32_t charcode( void ) const { return _charcode; }
  private:
    uint32_t _charcode;
  };

}
