#include <app/editor-layer.h>

#include <app/immgfx.h>

#include <utils/logger.h>
#include <core/input.h>

namespace mv {

  EditorLayer::EditorLayer( void ) : Layer( "editor layer" ) {
    PROFILE_FUNCTION();
    Immgfx::create_instance();
  }

  EditorLayer::~EditorLayer( void ) {
    PROFILE_FUNCTION();
    Immgfx::destory_instance();
  }

  void EditorLayer::start( Ref<MapLayer> map, polygon_id id, Ref<OrthoCamera> camera ) {
    PROFILE_FUNCTION();

    /* check valid parameters */
    if ( camera == nullptr || map == nullptr || !map->has( id ) ) {
      LOG_ERROR( "invalid parameters to start editing" );
      return;
    }

    /* store them in intenal variables */
    _current_map = map;
    _current_geom = map->get_geometry( id );
    _current_camera = camera;

    _is_editing = true;
  }

  void EditorLayer::end( void ) {
    PROFILE_FUNCTION();

    if ( !_is_editing ) return;

    /* sync GPU memory for the edited geometry */
    _current_map->update( _current_geom->id() );

    /* invalidate internal variables */
    _current_map = nullptr;
    _current_geom = nullptr;
    _current_camera = nullptr;

    _is_editing = false;
  }

  void EditorLayer::set_size( glm::uvec2 size ) {
    PROFILE_FUNCTION();

    _window_size = size;

    Immgfx::ref().resize( size );
  }

  void EditorLayer::update( float delta_time ) {
    PROFILE_FUNCTION();

    /* handle vertex dragging */
    if ( _is_over_vertex && Input::is_mouse_button_down( MOUSE_BUTTON_LEFT ) ) {
      glm::vec2 delta = Input::mouse_position_delta();

      /* scale delta position based on camera z value */
      glm::vec3 current_pos = _current_geom->get_position( _highlight_vertex );
      current_pos.x += delta.x * _current_camera->get_position().z;
      current_pos.y -= delta.y * _current_camera->get_position().z;

      /* move the vertex */
      _current_geom->update( _highlight_vertex, current_pos );
    }

    /* handle vertex deletion */
    if ( _is_over_vertex && Input::is_key_down( KEY_D ) && Input::is_mouse_button_pressed( MOUSE_BUTTON_LEFT ) ) {
      _current_geom->remove( _highlight_vertex );
    }

    /* handle vertex addition */
    if ( _is_over_edge && Input::is_key_down( KEY_A ) && Input::is_mouse_button_pressed( MOUSE_BUTTON_LEFT ) ) {
      glm::vec3 mouse_pos = glm::vec3( _current_camera->get_mouse_world_pos(), 0.0f );

      _current_geom->insert( _highlight_edge + 1, mouse_pos );
    }

  }

  void EditorLayer::render( void ) {
    PROFILE_FUNCTION();

    vertex_index vert = 0;
    vertex_index edge = 0;

    /* set point and line sizes */
    RenderState::ref().set_point_size( _point_size );
    RenderState::ref().set_line_thickenss( _line_thickness );

    /* bind framebuffer to capture vertex ids */
    Immgfx::ref().bind();

    /* begin rendering for edges */
    Immgfx::ref().begin();

    /* this vertex id is used to keep a track of the vertex pair submitted */
    size_t vertex_id = 0;
    for ( const auto& polygon : *_current_geom ) {

      for ( const auto& sub_polygon : polygon ) {
        for ( size_t i = 0; i < sub_polygon.vertices(); i++ ) {
          uint32_t current = i;
          uint32_t next = i + 1;

          if ( i == sub_polygon.vertices() - 1 )
            next = 0;

          if ( _is_over_edge && vertex_id == _highlight_edge ) {
            Immgfx::ref().push_vertex( sub_polygon.get_position( current ), _line_highlight_color );
            Immgfx::ref().push_vertex( sub_polygon.get_position( next ), _line_highlight_color );
          }           else {
            Immgfx::ref().push_vertex( sub_polygon.get_position( current ), _line_color );
            Immgfx::ref().push_vertex( sub_polygon.get_position( next ), _line_color );
          }

          vertex_id++;
        }
      }
    }

    /* draw edges */
    Immgfx::ref().draw( _current_camera, Topology::Line );

    /* get the edge index */
    edge = Immgfx::ref().get_primitive_id( Input::mouse_position().x, _window_size.y - Input::mouse_position().y );

    /* begin rendering for vertices */
    Immgfx::ref().begin();

    /* submit vertices and also handle highlight */
    for ( size_t i = 0; i < _current_geom->vertices(); i++ ) {
      if ( _is_over_vertex && i == _highlight_vertex )
        Immgfx::ref().push_vertex( _current_geom->get_position( i ), _point_highlight_color );
      else
        Immgfx::ref().push_vertex( _current_geom->get_position( i ), _point_color );
    }

    /* draw the vertices */
    Immgfx::ref().draw( _current_camera, Topology::Point );

    /* get the vertex index */
    vert = Immgfx::ref().get_primitive_id( Input::mouse_position().x, _window_size.y - Input::mouse_position().y );

    /* extract vertex id at mouse position to highlight */
    _highlight_vertex = vert;
    if ( _highlight_vertex > 0 ) {
      _is_over_vertex = true;
      _highlight_vertex--;
    }
    else {
      _is_over_vertex = false;
    }

    /* extract edge id at mouse position if a vertex is not hovered */
    if ( !_is_over_vertex ) {
      _highlight_edge = edge;
      if ( _highlight_edge > 0 ) {
        _is_over_edge = true;
        _highlight_edge--;
        _highlight_edge /= 2;
      }
      else {
        _is_over_edge = false;
      }
    }
    else {
      _is_over_edge = false;
    }

    /* unbind and draw contents of framebuffer to the screen */
    Immgfx::ref().unbind();
  }

}
