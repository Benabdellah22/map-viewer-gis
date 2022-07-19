#include <app/map-layer.h>

#include <array>

#include <app/earcut.h>
#include <app/geojson-loader.h>

namespace mv {

  MapLayer::MapLayer( const string& filename ) {
    PROFILE_FUNCTION();

    /* load geometries from geojson */
    _geometries = load_geojson_to_buffer( filename );

    /* return if not able to load the file */
    if ( _geometries.empty() ) {
      return;
    }

    /* set the ids */
    for ( auto& geom : _geometries ) {
      /* yes the first geometry will have id = 1, this is to */
      /* make 0 as invalid id                                */
      geom->set_id( ++_unique_id );
    }

    /* combined vertices and outlines for all the polygons in the file */
    std::vector<MapLayer::Vertex> combined_vertices;
    std::vector<glm::vec3> combined_outlines;

    for ( const auto& geometry : _geometries ) {
      /* compute triangulated vertices and outlines for the geometry */
      auto& vertices = make_polygon_buffer( geometry );
      auto& outlines = make_outline_buffer( geometry );

      _buffer_offsets.push_back( std::make_pair( combined_vertices.size(), combined_outlines.size() ) );

      /* insert the triangle and outline vertices in the combined list */
      combined_vertices.insert( combined_vertices.end(), vertices.begin(), vertices.end() );
      combined_outlines.insert( combined_outlines.end(), outlines.begin(), outlines.end() );

      /* store the sizes for later use */
      _buffer_sizes.push_back( std::make_pair( vertices.size(), outlines.size() ) );
    }

    /* create vertex buffers for both triangles and outlines */
    _consolidated_vertices = new VertexBuffer<Position3, PolygonIDAttribute>( combined_vertices.size(), combined_vertices.data() );
    _consolidated_outlines = new VertexBuffer<Position3>( combined_outlines.size(), combined_outlines.data() );

    /* assign the name using filename */
    _layer_name = filename;
    std::replace( _layer_name.begin(), _layer_name.end(), '\\', '/' );
    _layer_name = _layer_name.substr( _layer_name.find_last_of( '/' ) + 1, _layer_name.find_last_of( '.' ) - _layer_name.find_last_of( '/' ) - 1 );
  }

  MapLayer::~MapLayer( void ) {} /* do nothing */

  void MapLayer::select( polygon_id id ) {
    PROFILE_FUNCTION();

    /* push the id in the list of selected geometries */
    if ( !is_selected( id ) )
      _selected_geometries.push_back( id );
  }

  void MapLayer::deselect( polygon_id id ) {
    PROFILE_FUNCTION();

    /* remove the geometry if present in the list */
    auto it = std::find( _selected_geometries.begin(), _selected_geometries.end(), id );
    if ( it != _selected_geometries.end() )
      _selected_geometries.erase( it );
  }

  void MapLayer::revert_select( polygon_id id ) {
    PROFILE_FUNCTION();

    /* if not selected then select otherwise remove selected */
    if ( !is_selected( id ) )
      _selected_geometries.push_back( id );
    else
      deselect( id );
  }

  bool MapLayer::is_selected( polygon_id id ) {
    PROFILE_FUNCTION();

    /* find the `id` */
    return std::find( _selected_geometries.begin(), _selected_geometries.end(), id ) !=
           _selected_geometries.end();
  }

  void MapLayer::clear_selected( void ) {
    PROFILE_FUNCTION();

    /* deselect all the geometries */
    _selected_geometries.clear();
  }

  const std::vector<polygon_id>& MapLayer::selected_geometries( void ) const {
    PROFILE_FUNCTION();

    /* return the list of selected geometries */
    return _selected_geometries;
  }

  bool MapLayer::has( polygon_id id ) const {
    PROFILE_FUNCTION();

    /* check that the index is valid and then the id is valid */
    return id > 0 && id <= _unique_id;
  }

  Ref<Geometry> MapLayer::get_geometry( polygon_id id ) const {
    PROFILE_FUNCTION();

    /* if it is end then `id` not found */
    if ( !has( id ) ) {
      LOG_ERROR( "failed to find geometry with id: {}", id );
      return nullptr;
    }

    /* return the geometry if found */
    return _geometries[id - 1];
  }

  Box MapLayer::bounding_box( void ) const {
    PROFILE_FUNCTION();

    Box bb;

    /* compute the comnbined bounding box */
    for ( const auto& geom : _geometries )
      bb.unify( geom->bbox() );

    return bb;
  }

  void MapLayer::update( polygon_id id ) {
    PROFILE_FUNCTION();

    /* find geometry with `id` */
    Ref<Geometry> geom = get_geometry( id );
    if ( geom == nullptr )
      return;

    /* compute triangulated vertices and outlines */
    std::vector<MapLayer::Vertex> vertices = make_polygon_buffer( geom );
    std::vector<glm::vec3> outlines = make_outline_buffer( geom );

    /* compute the offset where the vertices and outlines for */
    /* this geometry is located at                            */
    size_t polygon_offset = _buffer_offsets[id - 1].first;
    size_t outline_offset = _buffer_offsets[id - 1].second;

    /* replace the memory with the new triangles and outlines */
    _consolidated_vertices->replace( polygon_offset, _buffer_sizes[id - 1].first, vertices.size(), vertices.data() );
    _consolidated_outlines->replace( outline_offset, _buffer_sizes[id - 1].second, outlines.size(), outlines.data() );

    /* update the sizes */
    _buffer_sizes[id - 1] = std::make_pair( vertices.size(), outlines.size() );

    /* update the offsets */
    for ( polygon_id i = id - 1; i < _buffer_offsets.size(); i++ ) {
      _buffer_offsets[i] = std::make_pair( polygon_offset, outline_offset );

      polygon_offset += _buffer_sizes[i].first;
      outline_offset += _buffer_sizes[i].second;
    }
  }

  std::tuple<uint32_t, uint32_t> MapLayer::get_outline_indices( polygon_id id ) const {
    PROFILE_FUNCTION();

    /* return 0 if the id is not present in the layer */
    if ( !has( id ) ) return { 0, 0 };

    return { _buffer_offsets[id - 1].second, _buffer_sizes[id - 1].second };
  }

  std::vector<MapLayer::Vertex> MapLayer::make_polygon_buffer( Ref<Geometry> geometry ) {

    /* triangulated coordinates */
    std::vector<MapLayer::Vertex> vertices;

    polygon_id id = geometry->id();

    /* triangulate the geometry */
    std::vector<vertex_index> indices = triangulate( geometry );

    /* improve performance */
    vertices.reserve( indices.size() );

    /* simply push the vertices at index */
    for ( const auto& index : indices ) {
      vertices.push_back( MapLayer::Vertex{ geometry->get_position( index ), { id, 0 } } );
    }

    /* return the vertices */
    return vertices;
  }

  std::vector<glm::vec3> MapLayer::make_outline_buffer( Ref<Geometry> geometry ) {

    /* connected outlines */
    std::vector<glm::vec3> outlines;

    /* simply connect the vertices to create outline */
    for ( const auto& polygon : *geometry ) {
      for ( const auto& sub_polygon : polygon ) {
        for ( size_t i = 0; i < sub_polygon.vertices(); i++ ) {
          outlines.push_back( sub_polygon.get_position( i ) );

          /* the last vertex will connect with the first */
          if ( i < sub_polygon.vertices() - 1 )
            outlines.push_back( sub_polygon.get_position( i + 1 ) );
          else
            outlines.push_back( sub_polygon.get_position( 0 ) );
        }
      }
    }

    /* return the list of outlines */
    return outlines;
  }

  std::vector<vertex_index> MapLayer::triangulate( Ref<Geometry> geometry ) {

    /* triangulated indices */
    std::vector<vertex_index> triangulated_indices;

    /* offset for each sub-polygon */
    size_t index_offset = 0;

    for ( const auto& polygon : *geometry ) {
      /* 2d coordinate */
      using Point = std::array<float, 2>;

      /* a collection of sub-polygons */
      std::vector<std::vector<Point>> polygons;

      /* size of positions array */
      size_t positions_size = 0;

      for ( const auto& sub_polygon : polygon ) {
        /* array to hold positions in the sub-polygon */
        std::vector<Point> positions;
        positions.reserve( sub_polygon.vertices() );

        /* poppulate the array */
        for ( const auto& position : sub_polygon ) {
          positions.push_back( { position.x, position.y } );
          positions_size++;
        }

        /* insert in the list of sub-polygons */
        polygons.push_back( positions );
      }

      /* triangulate */
      std::vector<vertex_index> indices = mapbox::earcut<vertex_index>( polygons );

      /* push the indices in the combined list of indices */
      for ( const auto& index : indices ) {
        triangulated_indices.push_back( index + index_offset );
      }

      /* increase offset for next sub-polygon */
      index_offset += positions_size;
    }

    /* return triangulated indices */
    return triangulated_indices;
  }

}
