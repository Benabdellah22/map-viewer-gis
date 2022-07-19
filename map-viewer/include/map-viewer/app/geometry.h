#pragma once

#include <vector>
#include <unordered_map>

#include <core/math.h>
#include <graphics/buffers.h>

#include <app/earcut.h>

namespace mv {

  /* a unique id to represent the polygon, a multi-polygon */
  /* is also treated as a polygon and given only one id.   */
  using polygon_id = uint32_t;

  /* index of vertex, these are unique to geometry but may */
  /* repeat across different geometries                    */
  using vertex_index = uint32_t;

  /* hash table of properties */
  using properties = std::unordered_map<string, string>;

  /* In a multi-polygon there are multiple polygons and each */
  /* polygon can have multiple sub polygons, for example a   */
  /* polygon with a cutout in between has one sub polygon.   */
  class SubPolygon {
  public:
    SubPolygon( void );
    ~SubPolygon( void );
  public:
    /* update vertex at `index` */
    void update( vertex_index index, const glm::vec3& position );

    /* insert vertex after `offset` */
    void insert( vertex_index offset, const glm::vec3& position );

    /* push a vertex at the back of the list */
    void push_back( const glm::vec3& position );

    /* delete vertex at `index` */
    void remove( vertex_index offset );

    /* return the position of vertex located at `index` */
    glm::vec3 get_position( vertex_index index ) const;

    /* return the number of vertices in the sub-polygon */
    inline uint32_t vertices( void ) const { return _vertices; }

    /* Iterators to iterate over all the coordinates in the polygon, note */
    /* that we do not have non-const iterator because we don't want these */
    /* to be able to update positions, we have separate functions for     */
    /* updating, inserting and deleting vertices.                         */
    inline std::vector<glm::vec3>::const_iterator begin( void ) const { return _positions.begin(); }
    inline std::vector<glm::vec3>::const_iterator end( void ) const { return _positions.end(); }

    /* return the list of polygons */
    inline const std::vector<glm::vec3>& positions( void ) const { return _positions; }

    /* set the state of updated */
    inline void set_updated( bool updated ) { _is_updated = updated; }
  private:
    /* array to hold all the positions */
    std::vector<glm::vec3> _positions;

    /* number of vertices in the sub-polygon */
    uint32_t _vertices = 0;

    /* is the position updated, deleted, or is a new one added */
    bool _is_updated = false;
  };

  /* A multi-polygon consist of multiple polygons, this class  */
  /* represent exactly that polygon. A polygon may or may not  */
  /* have more than one sub-polygons but is guarenteed to have */
  /* one sub-polygon.                                          */
  class Polygon {
  public:
    Polygon( void );
    ~Polygon( void );
  public:
    /* update vertex at `index`, remember here index is the  */
    /* combined index of all the vertices in the sub-polygon */
    void update( vertex_index index, const glm::vec3& position );

    /* insert vertex after `offset` */
    void insert( vertex_index offset, const glm::vec3& position );

    /* delete vertex at `index` */
    void remove( vertex_index index );

    /* push a entire sub-polygon at the back of the list */
    void push_back( const SubPolygon& sub_polygon );

    /* return the position of vertex located at `index` */
    glm::vec3 get_position( vertex_index index ) const;

    /* return the combined number of vertices in all the sub-polygon */
    inline uint32_t vertices( void ) const { return _vertices; }

    /* Iterators to iterate over all the sub-polygon in the polygon */
    inline std::vector<SubPolygon>::const_iterator begin( void ) const { return _sub_polygons.begin(); }
    inline std::vector<SubPolygon>::const_iterator end( void ) const { return _sub_polygons.end(); }

    /* return the list of all the sub-polygons */
    inline const std::vector<SubPolygon>& sub_polygons( void ) const { return _sub_polygons; }
  private:
    /* array of sub-polygons in the polygon */
    std::vector<SubPolygon> _sub_polygons;

    /* number of vertices in the polygon, this is the sum */
    /* of vertices of all the sub-polygons                */
    uint32_t _vertices = 0;
  };

  class Geometry : public SharedObject {
  public:
    Geometry( void );
    ~Geometry( void );
  public:
    /* update vertex at `index`, remember here index is the  */
    /* combined index of all the vertices in the polygons    */
    void update( vertex_index index, const glm::vec3& position );

    /* insert vertex after `offset` */
    void insert( vertex_index offset, const glm::vec3& position );

    /* delete vertex at `index` */
    void remove( vertex_index index );

    /* push a entire polygon at the back of the list */
    void push_back( const Polygon& polygon );

    /* return the position of vertex located at `index` */
    glm::vec3 get_position( vertex_index index ) const;

    /* compute bounding box, it is not computed by default */
    void compute_bbox( void );

    /* Iterators to iterate over all the polygon in the polygon */
    inline std::vector<Polygon>::const_iterator begin( void ) const { return _polygons.begin(); }
    inline std::vector<Polygon>::const_iterator end( void ) const { return _polygons.end(); }

    /* return the combined number of vertices in all the sub-polygon */
    inline uint32_t vertices( void ) const { return _vertices; }

    /* return the bounding box for the geometry */
    inline Box bbox( void ) const { return _bounding_box; }

    /* return the unique id allocated for the polygon */
    inline polygon_id id( void ) const { return _id; }

    /* set and get properties */
    inline const properties& get_props( void ) const { return _properties; }
    inline void set_props( properties props ) { _properties = props; }

    /* Do not use this function to set id, it is called by map layer to track */
    /* the polygon internally. Setting ids manually will result in undefined  */
    /* behaviour and may even crash the program.                              */
    inline void set_id( polygon_id id ) { _id = id; }
  private:
    /* array of polygons in the geometry */
    std::vector<Polygon> _polygons;

    /* number of vertices in the geometry, this is the sum */
    /* of vertices of all the polygons.                    */
    uint32_t _vertices = 0;

    /* unique id for the geometry */
    polygon_id _id;

    /* bounding box of the geometry */
    Box _bounding_box;

    /* properties of the current geometry */
    properties _properties;
  };

}
