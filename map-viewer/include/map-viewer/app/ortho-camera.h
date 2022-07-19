#pragma once

#include <core/camera.h>
#include <core/window.h>
#include <core/input.h>

namespace mv {

  /* [TODO]: Move implementation to cpp file */

  class OrthoCamera : public Camera {
  public:
    OrthoCamera( void ) {
      /* get the window size */
      _width = (float)Window::ref().width();
      _height = (float)Window::ref().height();
    }

    ~OrthoCamera( void ) {}
  public:
    /* return the projection matrix */
    virtual glm::mat4 projection( void ) const override {
      glm::mat4 projection = glm::mat4( 1.0f );

      /* create the projection matrix */
      glm::vec2 view_area = glm::vec2( _width * _position.z, _height * _position.z );
      projection = glm::ortho( -view_area.x / 2, view_area.x / 2, -view_area.y / 2, view_area.y / 2 );

      return projection;
    }

    /* return the camera view matrix */
    virtual glm::mat4 view( void ) const override {
      glm::mat4 view = glm::mat4( 1.0f );
      view = glm::translate( view, glm::vec3( -_position.x, -_position.y, 0.0f ) );
      return view;
    }

    /* A generic update function that is called every */
    /* frame, you can do anything you want in this.   */
    virtual void update( void ) override {
      /* handle mouse movement */
      glm::vec2 mouse_pos_delta = Input::mouse_position_delta();

      if ( Input::is_mouse_button_down( MOUSE_BUTTON_MIDDLE ) ) {
        float factor = _position.z * _mouse_move_sensitivity;

        _target.x += -mouse_pos_delta.x * factor;
        _target.y +=  mouse_pos_delta.y * factor;
      }

      /* handle mouse scrolling */
      glm::vec2 mouse_scroll_delta = Input::mouse_scroll_delta();

      if ( glm::abs( mouse_scroll_delta.y ) > 0.0f ) {
        float old_z = _target.z;

        _target.z += glm::mix( 0.0f, _target.z, -mouse_scroll_delta.y ) / _zoom_sensitivity;

        /* While zooming in the dist between camera and focus point is */
        /* larger than dist while zooming out, that is why zooming in  */
        /* is faster than zooming out from the same point. To fix this */
        /* we simply reduce negative velocity to it's positive counter */
        /* part.                                                       */
        if ( _target.z < 0.0f ) {
          _target.z -= _target.z / ( _zoom_sensitivity + 1 );
        }

        /* zoom to where the mouse is pointing */
        {
          float delta = old_z - _target.z;

          glm::vec2 mouse_position = Input::mouse_position();
          mouse_position.x -= _width / 2.0f;
          mouse_position.y = ( _height / 2.0f ) - mouse_position.y;

          float x_offset = delta * mouse_position.x;
          float y_offset = delta * mouse_position.y;

          _target.x += x_offset;
          _target.y += y_offset;
        }

      }
      if (Input::is_key_down(KEY_F5)) {
        _target = glm::vec3(377777.00f, 2077777.00f, 300.0f);
      }
      if (Input::is_mouse_double_clicked(MOUSE_BUTTON_LEFT)) {
        glm::vec3 mouse_pos = glm::vec3(get_mouse_world_pos(), 0.0f);
        _target = glm::vec3(mouse_pos.x, mouse_pos.y, _position.z / 5.0f);
      }
      /* seek the target */
      {
        _position = glm::lerp( _position, _target, _deacceleration_panning );
      }

    }

    /* called in case the window is resized */
    virtual void resize( glm::vec2 new_size ) override {
      _width = new_size.x;
      _height = new_size.y;
    }

    void set_position( glm::vec3 position ) {
      /* set the camera's position */
      _position = position;
      _target = position;
    }

    void set_target( glm::vec3 position ) {
      /* set the target's position */
      _target = position;
    }

    inline glm::vec3 get_position( void ) const {
      return _position;
    }

    inline glm::vec2 get_mouse_world_pos( void ) const {
      /* get mouse position in normalized device coordinates */
      glm::vec2 mouse_position = Input::mouse_position();
      mouse_position.x -= _width / 2.0f;
      mouse_position.y = ( _height / 2.0f ) - mouse_position.y;

      /* translate using mouse x and y */
      glm::vec2 camera_pos_xy = glm::vec2( _position.x, _position.y );

      /* scale using z coordinate */
      return camera_pos_xy + ( glm::vec2( mouse_position ) * _position.z );
    }

    inline glm::vec2 get_size( void ) const { return { _width, _height }; }
  private:
    // /* top left coordinate of the viewport, this should be */
    // /* zero if you are drawing on the entire screen        */
    // glm::vec2 _viewport_origin = { 0.0f, 0.0f };

    /* width and height of the window */
    float _width = 0.0f, _height = 0.0f;

    /* camera's world position */
    glm::vec3 _position   = glm::vec3( 0.0f, 0.0f, 1.0f );

    /* pre-computed projection matrix */
    glm::mat4 _projection = glm::mat4( 1.0f );

    /* the camera continuously seek _target `position` */
    glm::vec3 _target     = _position;

    /* deacceleration strength panning */
    float _deacceleration_panning = 0.09f;

    /* some values to tweak movement */

    /* factor multiplied with mouse delta value while panning */
    float _mouse_move_sensitivity = 3.0f;

    /* zoom sensitivity */
    float _zoom_sensitivity = 5.0f;
  };

}
