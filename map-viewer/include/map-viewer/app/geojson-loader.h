#pragma once

#include <vector>
#include <array>

#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <utils/logger.h>
#include <utils/file-reader.h>

#include <app/earcut.h>
#include <app/geometry.h>

namespace mv {

  static Ref<Geometry> process_multi_polygon( rapidjson::Value& coordinates ) {
    using namespace rapidjson;

    /* create the geometry object */
    Ref<Geometry> geometry = new Geometry();

    /* check if the list of array has a array type */
    if ( coordinates.GetType() != kArrayType ) {
      LOG_ERROR( "the list of coordinates does not have array type; cannot proceed" );
      return {};
    }

    /* number of polygons, these polygons may have sub polygons as well */
    uint32_t num_polygons = coordinates.GetArray().Size();

    /* process the polygons */
    for ( uint32_t i = 0; i < num_polygons; i++ ) {

      /* current polygon */
      Polygon polygon;

      /* In `MultiPolygon` the polygons can have sub polygons and the polygons are laid as list */
      /* of sub polygons ( even if there is only 1 ). So the type must be array.                */
      if ( coordinates[i].GetType() != kArrayType ) {
        LOG_ERROR( "in `MultiPolygon` the polygon must have a list of subpolygons but none is found; cannot proceed" );
        continue;
      }

      /* sub polygons in a multi polygon */
      uint32_t sub_polygons = coordinates[i].GetArray().Size();

      /* process sub polygons */
      for ( uint32_t j = 0; j < sub_polygons; j++ ) {
        SubPolygon sub_polygon;
        Value& polygon_coords = coordinates[i][j];

        /* all sub polygons must have array of coordinates */
        if ( polygon_coords.GetType() != kArrayType ) {
          LOG_ERROR( "polygons must have a array of coordinates but none is found; cannot proceed" );
          continue;
        }

        /* get the number of coordinates */
        uint32_t num_coordinates = polygon_coords.GetArray().Size();

        /* process the coordinates, ignore the last coordinate because it is same as the first */
        for ( uint32_t k = 0; k < num_coordinates - 1; k++ ) {

          /* coordinates must be array of two numbers */
          if ( polygon_coords[k].GetType() != kArrayType ) {
            LOG_ERROR( "coordinates must be laid as an array of numbers, the type does not match; cannot proceed" );
            continue;
          }

          /* coordinate must be a array of two numbers */
          if ( polygon_coords[k].GetArray().Size() != 2 ) {
            LOG_ERROR( "each coordinate must have 2 numbers laid in a array, invalid number of coordinates; cannot proceed" );
            continue;
          }

          /* get the actual coordinates; finally :-) */
          float x = polygon_coords[k][0].GetFloat();
          float y = polygon_coords[k][1].GetFloat();

          /* push them in the list of coordinate for further processing */
          sub_polygon.push_back( { x, y, 0.0f } );

        }

        /* push the sub polygon to a list of polygons */
        polygon.push_back( sub_polygon );
      }
      
      geometry->push_back( polygon );

    #if 0
      /* After the gigantic loop above we will get a list of polygons with a list of coordinates. */
      /* OpenGL cannot render arbitrary polygons so it must be converted to triangles now.        */
      std::vector<uint32_t> indices = mapbox::earcut<uint32_t>( polygons );

      /* use indices to build vertices array */
      for ( size_t i = 0; i < indices.size(); i++ ) {
        glm::vec3 vertex( 0.0f );
        int32_t index = indices[i];

        /* The index returned by `earcut` assume that the vertices     */
        /* are laid in a single array rather than an array of polygons */
        /* so if any index is greater than the current polygon's       */
        /* coordinate size than it can be any of the other polygons.   */
        for ( const auto& polygon : polygons ) {
          if ( index > polygon.size() - 1 ) {
            index -= polygon.size();
            continue;
          }

          /* index must never be negative */
          if ( index < 0 ) {
            LOG_ERROR( "invalid index after earcut" );
          }

          /* assign the vertex position leaving z coordinate to 0.0f */
          vertex.x = polygon[(size_t)index][0];
          vertex.y = polygon[(size_t)index][1];
          vertex.z = 0.0f;

          /* break the loop since we have found our index */
          break;
        }

        /* assign the vertex in the vector */
        vertices.push_back( vertex );

        /* include in the bounding box */
        bbox.include( vertex );
      }

      /* store outlines */
      for ( const auto& polygon : polygons ) {
        for ( size_t i = 0; i < polygon.size() - 1; i++ ) {
          outlines.push_back( glm::vec3( polygon[i][0], polygon[i][1], 0.0f ) );
          outlines.push_back( glm::vec3( polygon[i + 1][0], polygon[i + 1][1], 0.0f ) );
        }
      }

    #endif
    }
    
    /* return the geometry object */
    geometry->compute_bbox();
    return geometry;
  }

  static properties process_properties( rapidjson::Value& polygon_properties ) {
    using namespace rapidjson;

    properties props;

    /* check that the list of properties is an object type */
    if ( polygon_properties.GetType() != kObjectType ) {
      LOG_ERROR( "the list of properties does not have object type; cannot proceed" );
      return {};
    }

    /* process properties */
    for ( auto& property = polygon_properties.MemberBegin(); property < polygon_properties.MemberEnd(); property++ ) {
      Value& key = property->name;
      Value& val = property->value;

      std::string key_string;
      std::string val_string;

      /* convert the key and value to string for storing */
      if ( !key.IsString() ) {
        rapidjson::StringBuffer key_buffer;
        rapidjson::PrettyWriter<StringBuffer> key_writer( key_buffer );
        key.Accept( key_writer );

        key_string = key_buffer.GetString();
      } else {
        key_string = key.GetString();
      }

      if ( !val.IsString() ) {
        rapidjson::StringBuffer val_buffer;
        rapidjson::PrettyWriter<StringBuffer> val_writer( val_buffer );
        val.Accept( val_writer );

        val_string = val_buffer.GetString();
      }
      else {
        val_string = val.GetString();
      }
      
      /* insert in the hash table */
      props.insert( std::make_pair( key_string, val_string ) );
    }

    /* return the list of properties */
    return props;
  }

  static std::vector<Ref<Geometry>> load_geojson_to_buffer( const string& filename ) {
    using namespace rapidjson;

    std::vector<Ref<Geometry>> polygons;

    /* read the file */
    DataFile file( filename );
    if ( !file )
      return {};

    /* parse the json using rapidjson */
    Document d;
    d.Parse( file.content().c_str() );

    /* the first entry must be an array */
    if ( d.GetType() != kArrayType ) {
      LOG_ERROR( "the root geojson must be a single object with a array type: {}", filename );
      return {};
    }

    uint32_t feature_size = d.GetArray().Size();

    for ( uint32_t i = 0; i < feature_size; i++ ) {

      /* check the type member */
      if ( !d[i].HasMember( "type" ) ) {
        LOG_ERROR( "a geojson object does not have a `type` member; cannot proceed" );
        continue;
      }

      /* check the `type` member's value type */
      Value& type = d[i]["type"];
      if ( type.GetType() != kStringType ) {
        LOG_ERROR( "the value of `type` member must be a string; cannot proceed" );
        continue;
      }

      /* handle the type accordingly */
      if ( type.GetString() == std::string( "Feature" ) ) {
        /* the type of the object is feature */

        /* [TODO]: Handle features. */
        /* check the `properties member` */
        if ( !d[i].HasMember( "properties" ) ) {
          LOG_ERROR( "a geojson object does not have a `properties` member; cannot proceed" );
          continue;
        }

        /* get properties for this polygon */
        properties props = process_properties( d[i]["properties"] );

        /* check the `geometry` member */
        if ( !d[i].HasMember( "geometry" ) ) {
          LOG_ERROR( "a geojson object does not have a `geometry` member; cannot proceed" );
          continue;
        }

        /* check the `type` of `geometry` */
        if ( !d[i]["geometry"].HasMember( "type" ) ) {
          LOG_ERROR( "a geojson object does not define the `type` of `geometry`; cannot proceed" );
          continue;
        }
        
        Value& geomerty_type = d[i]["geometry"]["type"];
        if ( geomerty_type.GetType() != kStringType ) {
          LOG_ERROR( "the geometry type is defined but the value is not a string; cannot proceed" );
          continue;
        }

        if ( geomerty_type.GetString() == std::string( "MultiPolygon" ) ) {
          auto geom = process_multi_polygon( d[i]["geometry"]["coordinates"] );
          geom->set_props( props );
          polygons.push_back( geom );
        } else {
          LOG_ERROR( "invalid `geometry` in geojson or geometry type not yet imeplemented: {}", geomerty_type.GetString() );
          /* [TODO]: Handle other types. */
        }

      } else {
        LOG_ERROR( "invalid `type` in geojson or type handler not yet implimented: {}", type.GetString() );
        /* [TODO]: Handle other types. */
      }

    }

    /* return the vertex buffer */
    return polygons;
  }

}
