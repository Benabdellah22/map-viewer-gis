#pragma once

#include <vector>

#include <base/mouse-events.h>
#include <base/keyboard-events.h>

#include <core/math.h>

namespace mv {

  /* The input class is completely static so that we do */
  /* not have to do Input::ref().is_key_down again and  */
  /* again. It can simply be Input::is_key_down.        */

  class Input {
  public:
    /* These to functions must be called at the start of */
    /* the runtime loop in order to update the internal  */
    /* state of this class.                              */

    /* update the internal state */
    static void update( void );

    /* capture events */
    static void on_event( Event& e );
  public:
    /* input checking functions */

    /* is key just pressed */
    static bool is_key_pressed( KeyCode key_code );

    /* is key just released */
    static bool is_key_released( KeyCode key_code );

    /* is key down */
    static bool is_key_down( KeyCode key_code );

    /* is key up */
    static bool is_key_up( KeyCode key_code );

    /* is mouse button pressed */
    static bool is_mouse_button_pressed( MouseCode mouse_code );

    /* is mouse button pressed */
    static bool is_mouse_button_released( MouseCode mouse_code );

    /* is mouse button down */
    static bool is_mouse_button_down( MouseCode mouse_code );
    
    /* is the mouse double clicked */
    static bool is_mouse_double_clicked( MouseCode mouse_code );

    /* get mouse cursor and scroll wheel state */
    
    /* get mouse position */
    static glm::vec2 mouse_position( void );

    /* get mouse position in normalized device coordinate */
    static glm::vec2 mouse_position_ndc( void );

    /* get mouse delta position */
    static glm::vec2 mouse_position_delta( void );

    /* get mouse scroll delta */
    static glm::vec2 mouse_scroll_delta( void );

    /* set viewport position to get relative mouse position */
    static void set_viewport_position( glm::vec2 topleft );
  private:
    /* event functions */
    static bool key_down( KeyPressedEvent& e );
    static bool key_up( KeyReleasedEvent& e );
    static bool mouse_down( MousePressedEvent& e );
    static bool mouse_up( MouseReleasedEvent& e );
    static bool mouse_moved( MouseMovedEvent& e );
    static bool mouse_scrolled( MouseScrolledEvent& e );
  private:
    /* current and previous frame's keyboard state */
    static std::vector<bool> _keyboard_state;
    static std::vector<bool> _previous_keyboard_state;

    /* current and previous frame's mouse state */
    static std::vector<bool> _mouse_state;
    static std::vector<bool> _previous_mouse_state;
    static std::vector<bool> _mouse_double_click;

    /* time when the mouse was pressed, used to compute double click */
    static std::vector<float> _mouse_pressed_time;

    /* current and previous frame's mouse position */
    static glm::vec2 _mouse_position;
    static glm::vec2 _previous_mouse_position;

    /* mouse scrolled delta, it is directly provided */
    /* by GLFW so no previous state in this case     */
    static glm::vec2 _mouse_scroll_delta;

    /* make mouse position relative to a viewport, useful when drawing */
    /* in a smaller area and want relative mouse positions throughout  */
    static glm::vec2 _viewport_position_topleft;
  };

}
