#pragma once

#include <core/layer.h>

#include <app/immgfx.h>
#include <app/map-layer.h>
#include <app/ortho-camera.h>

namespace mv {

  /* This is used to edit polygons, this one uses a */
  /* different Immgfx renderer to reduce complexity */
  /* while editing polygons.                        */
  class EditorLayer : public Layer {
  public:
    EditorLayer( void );
    ~EditorLayer( void );
  public:
    /* start editing the polygon with `id` in the `map` */
    void start( Ref<MapLayer> map, polygon_id id, Ref<OrthoCamera> camera );

    /* end editing, this update the map and sync with memory */
    void end( void );

    /* set the size of the editor layer */
    void set_size( glm::uvec2 size );

    /* call this every frame to update internals */
    virtual void update( float delta_time ) override;

    /* call this to render the editing polygon */
    virtual void render( void ) override;

    /* To check for editing; by rule `on_update` and `on_render`   */
    /* should only be called while editing, they won't do anything */
    /* if there is no polygon being edited.                        */
    inline bool is_editing( void ) const { return _is_editing; }

    /* set and get line thickness */
    inline float get_line_thickness( void ) const { return _line_thickness; }
    inline void set_line_thickness( float thickness ) { _line_thickness = thickness; }

    /* set and get point size */
    inline float get_point_size( void ) const { return _point_size; }
    inline void set_point_size( float size ) { _point_size = size; }

    /* set and get line color */
    inline glm::vec4 get_line_color( void ) const { return _line_color; }
    inline void set_line_color( glm::vec4 color ) { _line_color = color; }

    /* set and get point color */
    inline glm::vec4 get_point_color( void ) const { return _point_color; }
    inline void set_point_color( glm::vec4 color ) { _point_color = color; }

    /* set and get line highlight color */
    inline glm::vec4 get_line_highlight_color( void ) const { return _line_highlight_color; }
    inline void set_line_highlight_color( glm::vec4 color ) { _line_highlight_color = color; }

    /* set and get point highlight color */
    inline glm::vec4 get_point_highlight_color( void ) const { return _point_highlight_color; }
    inline void set_point_highlight_color( glm::vec4 color ) { _point_highlight_color = color; }
  private:
    bool _is_editing = false;

    Ref<OrthoCamera> _current_camera;
    Ref<MapLayer>    _current_map;
    Ref<Geometry>    _current_geom;

    float _line_thickness = 3.0f;
    float _point_size     = 8.0f;

    glm::vec4 _line_color  = { 1.0f, 0.0f, 0.0f, 1.0f };
    glm::vec4 _point_color = { 1.0f, 0.0f, 0.0f, 1.0f };

    glm::vec4 _line_highlight_color  = { 0.0f, 0.019f, 1.0f, 1.0f };
    glm::vec4 _point_highlight_color = { 0.0f, 0.019f, 1.0f, 1.0f };
    
    /* internal variables */
    bool _is_over_vertex = false;
    bool _is_over_edge   = false;

    vertex_index _highlight_vertex = 0;
    vertex_index _highlight_edge   = 0;

    glm::uvec2 _window_size = { 0, 0 };
  };
  
}
