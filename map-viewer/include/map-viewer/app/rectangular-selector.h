#pragma once

#include <core/math.h>
#include <core/input.h>

#include <utils/logger.h>

namespace mv {

  class RectangularSelector {
  public:
    void start( void ) {
      _start = Input::mouse_position();
      _end = _start;
      _is_dragging = true;
    }

    void update( void ) {
      _delta = Input::mouse_position() - _end;
      if ( _delta.x != 0 ) {
        _delta.x < 0.0f ? _delta.x = -1 : _delta.x = 1;
      }
      if ( _delta.y != 0 ) {
        _delta.y < 0.0f ? _delta.y = -1 : _delta.y = 1;
      }
      _end += _delta;
    }

    void end( void ) {
      _end = { 0.0f, 0.0f };
      _is_dragging = false;
    }

    inline bool is_dragging( void ) const { return _is_dragging; }

    inline glm::vec2 top_left( void ) const { return _start; }
    inline glm::vec2 bottom_right( void ) const { return _end; }
    inline glm::vec2 size( void ) const { return _end - _start; }

    inline glm::vec3 horizontal( void ) { return glm::vec3( _end.x, _start.y, _end.y ); }
    inline glm::vec3 vertical( void ) { return glm::vec3( _end.y, _start.x, _end.x ); }
  private:
    glm::vec2 _start  = { 0.0f, 0.0f };
    glm::vec2 _end    = { 0.0f, 0.0f };
    glm::vec2 _delta  = { 0.0f, 0.0f };

    bool _is_dragging = false;
  };

}
