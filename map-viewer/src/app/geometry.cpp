#include <app/geometry.h>

#include <limits>

#include <utils/logger.h>
#include <utils/assert.h>

namespace mv {

  /* ---------- SUB POLYGON ---------- */

  SubPolygon::SubPolygon( void ) {} /* do nothing */

  SubPolygon::~SubPolygon( void ) {} /* do nothing */

  void SubPolygon::update( vertex_index index, const glm::vec3& position ) {

    /* return if the index is invalid */
    if ( index >= _vertices ) {
      LOG_ERROR( "the sub-polygon only has {} vertices, cannot update vertex at index: {}", _vertices, index );
      return;
    }

    /* update the position at specified index */
    _positions[index] = position;
    _is_updated = true;
  }

  void SubPolygon::insert( vertex_index offset, const glm::vec3& position ) {

    /* return if the offset is invalid */
    if ( offset > _vertices ) {
      LOG_ERROR( "the sub-polygon only has {} vertices, cannot insert vertex at offset: {}", _vertices, offset );
      return;
    }

    /* insert a new vertex at offset */
    _positions.insert( _positions.begin() + offset, position );

    /* update the size of vertices */
    _vertices++;
    _is_updated = true;
  }

  void SubPolygon::push_back( const glm::vec3& position ) {

    _positions.push_back( position );

    /* update the size of vertices */
    _vertices++;
    _is_updated = true;
  }

  void SubPolygon::remove( vertex_index index ) {

    /* return if the index is invalid */
    if ( index >= _vertices ) {
      LOG_ERROR( "the sub-polygon only has {} vertices, cannot remove vertex at index: {}", _vertices, index );
      return;
    }

    /* remove the vertex at index */
    _positions.erase( _positions.begin() + index );

    /* update the size of vertices */
    _vertices--;
    _is_updated = true;
  }

  glm::vec3 SubPolygon::get_position( vertex_index index ) const {

    /* return if the index is invalid */
    if ( index >= _vertices ) {
      THROW( "failed to get sub-polygon position at invalid index; index out of bounds" );
    }

    return _positions[index];
  }

  /* ---------- POLYGON ---------- */

  Polygon::Polygon( void ) {} /* do nothing */

  Polygon::~Polygon( void ) {} /* do nothing */

  void Polygon::update( vertex_index index, const glm::vec3& position ) {

    /* return if the index is invalid */
    if ( index >= _vertices ) {
      LOG_ERROR( "the polygon only has {} vertices, cannot update vertex at index: {}", _vertices, index );
      return;
    }

    /* iterate over all the sub-polygons to find the correct index */
    for ( auto& sub_polygon : _sub_polygons ) {
      if ( index >= sub_polygon.vertices() ) {
        index -= sub_polygon.vertices();
        continue;
      }

      /* update the correct index vertex for the sub-polygon */
      sub_polygon.update( index, position );
      break;
    }
  }

  void Polygon::insert( vertex_index offset, const glm::vec3& position ) {

    /* return if the index is invalid */
    if ( offset > _vertices ) {
      LOG_ERROR( "the polygon only has {} vertices, cannot insert vertex at offset: {}", _vertices, offset );
      return;
    }

    /* iterate over all the sub-polygons to find the correct index */
    for ( auto& sub_polygon : _sub_polygons ) {
      if ( offset > sub_polygon.vertices() ) {
        offset -= sub_polygon.vertices();
        continue;
      }

      /* insert the vertex at the correct index for the sub-polygon */
      sub_polygon.insert( offset, position );
      break;
    }

    /* update the number of vertices */
    _vertices++;
  }

  void Polygon::remove( vertex_index index ) {

    /* return if the index is invalid */
    if ( index >= _vertices ) {
      LOG_ERROR( "the polygon only has {} vertices, cannot remove vertex at index: {}", _vertices, index );
      return;
    }

    /* iterate over all the sub-polygons to find the correct index */
    for ( auto& sub_polygon : _sub_polygons ) {
      if ( index >= sub_polygon.vertices() ) {
        index -= sub_polygon.vertices();
        continue;
      }

      /* update the correct index vertex for the sub-polygon */
      sub_polygon.remove( index );
      break;
    }

    /* update the number of vertices */
    _vertices--;
  }

  void Polygon::push_back( const SubPolygon& sub_polygon ) {

    /* do not add empty sub-polygon */
    if ( sub_polygon.vertices() == 0 ) {
      LOG_WARN( "trying to insert empty sub-polygon; cannot continute" );
      return;
    }

    /* inser the sub-polygon to the list */
    _sub_polygons.push_back( sub_polygon );

    /* update the number of vertices */
    _vertices += sub_polygon.vertices();
  }

  glm::vec3 Polygon::get_position( vertex_index index ) const {

    if ( index >= _vertices ) {
      THROW( "failed to get polygon position at invalid index; index out of bounds" );
    }

    /* iterate over all the sub-polygons to find the correct index */
    for ( auto& sub_polygon : _sub_polygons ) {
      if ( index >= sub_polygon.vertices() ) {
        index -= sub_polygon.vertices();
        continue;
      }

      /* update the correct index vertex for the sub-polygon */
      return sub_polygon.get_position( index );
      break;
    }

    /* to remove warning; this should never be executed */
    THROW( "fatal error" );
    return glm::vec3( 0.0f );
  }

  /* ---------- GEOMETRY ---------- */

  Geometry::Geometry( void ) : _id( 0 ) {}

  Geometry::~Geometry( void ) {} /* do nothing */

  void Geometry::update( vertex_index index, const glm::vec3& position ) {

    /* return if the index is invalid */
    if ( index >= _vertices ) {
      LOG_ERROR( "the polygon only has {} vertices, cannot update vertex at index: {}", _vertices, index );
      return;
    }

    /* iterate over all the polygons to find the correct index */
    for ( auto& polygon : _polygons ) {
      if ( index >= polygon.vertices() ) {
        index -= polygon.vertices();
        continue;
      }

      /* update the correct index vertex for the polygon */
      polygon.update( index, position );
      break;
    }
  }

  void Geometry::insert( vertex_index offset, const glm::vec3& position ) {

    /* return if the index is invalid */
    if ( offset > _vertices ) {
      LOG_ERROR( "the polygon only has {} vertices, cannot insert vertex at offset: {}", _vertices, offset );
      return;
    }

    /* iterate over all the polygons to find the correct index */
    for ( auto& polygon : _polygons ) {
      if ( offset > polygon.vertices() ) {
        offset -= polygon.vertices();
        continue;
      }

      /* insert the vertex at the correct index for the polygons */
      polygon.insert( offset, position );
      break;
    }

    /* update the number of vertices */
    _vertices++;
  }

  void Geometry::remove( vertex_index index ) {

    /* return if the index is invalid */
    if ( index >= _vertices ) {
      LOG_ERROR( "the polygon only has {} vertices, cannot remove vertex at index: {}", _vertices, index );
      return;
    }

    /* iterate over all the polygons to find the correct index */
    for ( auto& polygon : _polygons ) {
      if ( index >= polygon.vertices() ) {
        index -= polygon.vertices();
        continue;
      }

      /* update the correct index vertex for the polygons */
      polygon.remove( index );
      break;
    }

    /* update the number of vertices */
    _vertices--;
  }

  void Geometry::push_back( const Polygon& polygon ) {

    /* do not add empty polygon */
    if ( polygon.vertices() == 0 ) {
      LOG_WARN( "trying to insert empty sub-polygon; cannot continute" );
      return;
    }

    /* insert the polygon to the list */
    _polygons.push_back( polygon );

    /* update the number of vertices */
    _vertices += polygon.vertices();
  }

  glm::vec3 Geometry::get_position( vertex_index index ) const {

    if ( index >= _vertices ) {
      THROW( "failed to get polygon position at invalid index; index out of bounds" );
    }

    /* iterate over all the polygons to find the correct index */
    for ( auto& polygon : _polygons ) {
      if ( index >= polygon.vertices() ) {
        index -= polygon.vertices();
        continue;
      }

      /* update the correct index vertex for the polygon */
      return polygon.get_position( index );
      break;
    }

    /* to remove warning; this should never be executed */
    THROW( "fatal error" );
    return glm::vec3( 0.0f );
  }

  void Geometry::compute_bbox( void ) {

    /* create an empty bounding box */
    _bounding_box = Box();

    /* includ all the vertices to the bounding box */
    for ( const auto& polygon : _polygons ) {
      for ( const auto& sub_polygon : polygon ) {
        for ( const auto& position : sub_polygon ) {
          _bounding_box.include( position );
        }
      }
    }
  }

}
