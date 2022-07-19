#include <core/input.h>

#include <cstring>

#include <core/window.h>
#include <core/timer.h>

namespace mv {

  #define DOUBLE_CLICK_DELAY 0.2f

  /* static variable definitions */
  std::vector<bool> Input::_keyboard_state( KEY_MAX );
  std::vector<bool> Input::_previous_keyboard_state( KEY_MAX );
  std::vector<bool> Input::_mouse_state( MOUSE_BUTTON_MAX );
  std::vector<bool> Input::_previous_mouse_state( MOUSE_BUTTON_MAX );
  std::vector<bool> Input::_mouse_double_click( MOUSE_BUTTON_MAX );

  std::vector<float> Input::_mouse_pressed_time( MOUSE_BUTTON_MAX );

  glm::vec2 Input::_mouse_position = { 0.0f, 0.0f };
  glm::vec2 Input::_previous_mouse_position = { 0.0f, 0.0f };
  glm::vec2 Input::_mouse_scroll_delta = { 0.0f, 0.0f };

  glm::vec2 Input::_viewport_position_topleft = { 0.0f, 0.0f };

  void Input::update( void ) {
    /* assign the previous keyboard and mouse states */
    std::move( _keyboard_state.begin(), _keyboard_state.end(), _previous_keyboard_state.begin() );
    std::move( _mouse_state.begin(), _mouse_state.end(), _previous_mouse_state.begin() );

    _previous_mouse_position = _mouse_position;
    _mouse_scroll_delta = glm::vec2( 0.0f );
  }

  void Input::on_event( Event& e ) {
    EventDispatcher dispatcher( e );

    /* handle the events */
    dispatcher.dispatch<KeyPressedEvent>( Input::key_down );
    dispatcher.dispatch<KeyReleasedEvent>( Input::key_up );
    dispatcher.dispatch<MousePressedEvent>( Input::mouse_down );
    dispatcher.dispatch<MouseReleasedEvent>( Input::mouse_up );
    dispatcher.dispatch<MouseMovedEvent>( Input::mouse_moved );
    dispatcher.dispatch<MouseScrolledEvent>( Input::mouse_scrolled );
  }

  bool Input::is_key_pressed( KeyCode key_code ) {
    return _keyboard_state[key_code] && !_previous_keyboard_state[key_code];
  }

  bool Input::is_key_released( KeyCode key_code ) {
    return !_keyboard_state[key_code] && _previous_keyboard_state[key_code];
  }

  bool Input::is_key_down( KeyCode key_code ) {
    return _keyboard_state[key_code];
  }

  bool Input::is_key_up( KeyCode key_code ) {
    return !_keyboard_state[key_code];
  }

  bool Input::is_mouse_button_pressed( MouseCode mouse_code ) {
    return _mouse_state[mouse_code] && !_previous_mouse_state[mouse_code];
  }

  bool Input::is_mouse_button_released( MouseCode mouse_code ) {
    return !_mouse_state[mouse_code] && _previous_mouse_state[mouse_code];
  }

  bool Input::is_mouse_button_down( MouseCode mouse_code ) {
    return _mouse_state[mouse_code];
  }

  bool Input::is_mouse_double_clicked( MouseCode mouse_code ) {
    return _mouse_double_click[mouse_code] && is_mouse_button_pressed( mouse_code );
  }

  glm::vec2 Input::mouse_position( void ) {
    return _mouse_position - _viewport_position_topleft;
  }

  glm::vec2 Input::mouse_position_ndc( void ) {
    uint32_t width = Window::ref().width();
    uint32_t height = Window::ref().height();

    /* get pixel size in normalized device coordinate */
    glm::vec2 ndc = { 1.0f / width, 1.0f / height };

    /* get mouse position in normalized device coordinate */
    ndc.x *= _mouse_position.x;
    ndc.y *= _mouse_position.y;

    /* The center is currently at top left so we transform   */
    /* the space to bring {0, 0} at the center. We also      */
    /* transform the coords to [-1, 1]. Also, y is inverted. */
    ndc.x =  2.0f * ndc.x - 1.0f;
    ndc.y = -2.0f * ndc.y + 1.0f;

    return ndc;
  }

  glm::vec2 Input::mouse_position_delta( void ) {
    return _mouse_position - _previous_mouse_position;
  }

  glm::vec2 Input::mouse_scroll_delta( void ) {
    return _mouse_scroll_delta;
  }

  void Input::set_viewport_position( glm::vec2 topleft ) {
    _viewport_position_topleft = topleft;
  }

  bool Input::key_down( KeyPressedEvent& e ) {
    if ( e.key_code() >= KEY_MAX ) return false;
    _keyboard_state[e.key_code()] = true;
    return false;
  }

  bool Input::key_up( KeyReleasedEvent& e ) {
    if ( e.key_code() >= KEY_MAX ) return false;
    _keyboard_state[e.key_code()] = false;
    return false;
  }

  bool Input::mouse_down( MousePressedEvent& e ) {
    if ( e.mouse_code() >= MOUSE_BUTTON_MAX ) return false;
    _mouse_state[e.mouse_code()] = true;

    /* handle double click */
    float previous_time = _mouse_pressed_time[e.mouse_code()];
    _mouse_pressed_time[e.mouse_code()] = Timer::time();

    /* check the time between two consicutive presses is less than the delay */
    if ( _mouse_pressed_time[e.mouse_code()] - previous_time < DOUBLE_CLICK_DELAY ) {
      _mouse_double_click[e.mouse_code()] = _mouse_double_click[e.mouse_code()] ? false : true;
    } else {
      _mouse_double_click[e.mouse_code()] = false;
    }

    return false;
  }

  bool Input::mouse_up( MouseReleasedEvent& e ) {
    if ( e.mouse_code() >= MOUSE_BUTTON_MAX ) return false;
    _mouse_state[e.mouse_code()] = false;
    return false;
  }

  bool Input::mouse_moved( MouseMovedEvent& e ) {
    _mouse_position = { e.x(), e.y() };
    return false;
  }

  bool Input::mouse_scrolled( MouseScrolledEvent& e ) {
    _mouse_scroll_delta = { e.x(), e.y() };
    return false;
  }

}
