#pragma once

#include <list>
#include <vector>
#include <unordered_map>
#include <types.h>

#include <utils/ref.h>
#include <graphics/buffers.h>

#include <app/geometry.h>

namespace mv {

  /* glsl vertex attribute, this one represent polygon id */
  /* only one component is used for now so I may remove   */
  /* the other later.                                     */
  struct PolygonIDAttribute {
    static VertexDataType attribute_type( void ) { return VertexDataType::UInt2; }
    static string attribute_name( void ) { return "a_id"; }
    uint32_t data[2];
  };

  /* Map layer is a data structure to hold all the polygons in a */
  /* geojson file.  */
  class MapLayer : public SharedObject {
    struct Vertex {
      glm::vec3  position;
      glm::uvec2 polygon_id;
    };
  public:
    MapLayer( const string& filename );
    ~MapLayer( void );
  public:
    /* select the geometry with `id` */
    void select( polygon_id id );

    /* deselect the geometry if selected */
    void deselect( polygon_id id );

    /* select the `id` if not selected otherwise deselect it */
    void revert_select( polygon_id id );

    /* is the polygon with `id` selected */
    bool is_selected( polygon_id id );

    /* clear all selected geometries */
    void clear_selected( void );

    /* return the selected geometries */
    const std::vector<polygon_id>& selected_geometries( void ) const;

    /* check weather the layer has geometry with `id` */
    bool has( polygon_id id ) const;

    /* return the geometry with `id` */
    Ref<Geometry> get_geometry( polygon_id id ) const;

    /* return the bounding box for the layer */
    Box bounding_box( void ) const;

    /* update the geometry with `id`, call this after editing geometry */
    void update( polygon_id id );

    std::tuple<uint32_t, uint32_t> get_outline_indices( polygon_id id ) const;

    /* is the layer empty */
    inline bool empty( void ) const { return _geometries.empty(); }

    /* return the vertex buffer */
    inline Ref<VertexBuffer<Position3, PolygonIDAttribute>> vertex_buffer( void ) const { return _consolidated_vertices; }

    /* return the outline buffer */
    inline Ref<VertexBuffer<Position3>> outline_buffer( void ) const { return _consolidated_outlines; }

    /* return the name */
    inline string name( void ) const { return _layer_name; }

    /* set and get hidden */
    inline void set_hidden( bool hidden ) { _hidden = hidden; }
    inline bool is_hidden( void ) const { return _hidden; }

    /* set and get colors */
    inline void set_polygon_fill_color( glm::vec4 color ) { _fill_color = color; }
    inline glm::vec4 get_polygon_fill_color( void ) const { return _fill_color; }

    inline void set_line_color( glm::vec4 color ) { _line_color = color; }
    inline glm::vec4 get_line_color( void ) const { return _line_color; }

    /* set and get thickness */
    inline void set_line_thickness( float thickness ) { _line_thickness = thickness; }
    inline float get_line_thickness( void ) const { return _line_thickness; }

    /* set and should draw triangles */
    inline void set_draw_triangles( bool draw ) { _draw_triangles = draw; }
    inline bool should_draw_triangles( void ) const { return _draw_triangles; }

    /* set and should draw outlines */
    inline void set_draw_outlines( bool draw ) { _draw_outlines = draw; }
    inline bool should_draw_outlines( void ) const { return _draw_outlines; }

    /* iterators to iterate geometries */
    inline std::vector<Ref<Geometry>>::iterator begin( void ) { return _geometries.begin(); }
    inline std::vector<Ref<Geometry>>::iterator end( void ) { return _geometries.end(); }
    
    inline std::vector<Ref<Geometry>>::const_iterator begin( void ) const { return _geometries.begin(); }
    inline std::vector<Ref<Geometry>>::const_iterator end( void ) const { return _geometries.end(); }

    /* return the number of geometries in the layer */
    inline uint32_t num_geometries( void ) const { return _geometries.size(); }

    /* get the geometries vector */
    inline std::vector<Ref<Geometry>>& get_geometries( void ) { return _geometries; }
  private:
    /* return the triangulated vertex positions for the geometry */
    std::vector<Vertex> make_polygon_buffer( Ref<Geometry> geometry );

    /* return the connected outline positions for the geometry */
    std::vector<glm::vec3> make_outline_buffer( Ref<Geometry> geometry );

    /* triangulate a geometry and return the vertices */
    std::vector<vertex_index> triangulate( Ref<Geometry> geometry );
  private:
    /* hash map for all the geometries in the layer */
    std::vector<Ref<Geometry>> _geometries;

    /* selected polygons in the geometry */
    std::vector<polygon_id> _selected_geometries;

    /* size and offset occupied by each geometry in the below vertex buffers */
    std::vector<std::pair<size_t, size_t>> _buffer_sizes;
    std::vector<std::pair<size_t, size_t>> _buffer_offsets;

    /* single vertex buffer for all the vertices, also this is triangulated */
    Ref<VertexBuffer<Position3, PolygonIDAttribute>> _consolidated_vertices;

    /* single vertex buffer for all the outlines */
    Ref<VertexBuffer<Position3>> _consolidated_outlines;

    /* name of the layer */
    string _layer_name;

    /* weather the layer is hidden */
    bool _hidden = false;

    /* polygon fill color for layer */
    glm::vec4 _fill_color = { 0.0f, 1.0f, 0.0f, 0.5f };

    /* line color for layer */
    glm::vec4 _line_color = { 0.0f, 0.0f, 0.0f, 0.5f };

    /* thickness of line for layer */
    float _line_thickness = 1.0f;

    /* draw triangles or not */
    bool _draw_triangles = true;

    /* draw outlines or not */
    bool _draw_outlines = true;

    /* internal id */
    polygon_id _unique_id = 0;
  };

}
