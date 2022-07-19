#pragma once

namespace mv {

  /* use to assign unique bit to each event category */
  #define BIT( X ) ( 1 << X )

  /* Different event types that can be dispatched */
  /* in the application.                          */
  enum class EventType {
    /* keyboard events */
    KeyPressed,
    KeyReleased,
    KeyRepeat,
    KeyTyped,

    /* mouse events */
    MousePressed,
    MouseReleased,
    MouseMoved,
    MouseScrolled,

    /* window events */
    WindowClose,
    WindowResize,
    WindowMove
  };

  /* Event categories, each event must have at least one */
  /* category and can have as many as required.          */
  enum EventCategory {
    EVENT_CATEGORY_NON         = 0,

    EVENT_CATEGORY_INPUT       = BIT( 0 ),
    EVENT_CATEGORY_KEYBOARD    = BIT( 1 ),
    EVENT_CATEGORY_MOUSE       = BIT( 1 ),
    EVENT_CATEGORY_WINDOW      = BIT( 1 ),
    EVENT_CATEGORY_APPLICATION = BIT( 1 ),
  };

}
