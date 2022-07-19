#pragma once

#include <glad/glad.h>

#include <utils/exception.h>

#include <graphics/renderstate.h>
#include <graphics/textures.h>
#include <graphics/buffers.h>

namespace mv {

  static GLenum map( Topology topology ) {
		switch ( topology )	{
		case Topology::Point:                 return GL_POINTS;
		case Topology::Line:								  return GL_LINES;
		case Topology::LineStrip:						  return GL_LINE_STRIP;
		case Topology::LineAdjacent:				  return GL_LINES_ADJACENCY;
		case Topology::LineStripAdjacent:		  return GL_LINE_STRIP_ADJACENCY;
		case Topology::Triangle:						  return GL_TRIANGLES;
		case Topology::TriangleStrip:				  return GL_TRIANGLE_STRIP;
		case Topology::TriangleAdjacent:		  return GL_TRIANGLES_ADJACENCY;
		case Topology::TriangleStripAdjacent: return GL_TRIANGLE_STRIP_ADJACENCY;
		default:
			break;
		}

    THROW( "invalid primitive topology" );
    return GL_INVALID_ENUM;
  }

	static GLenum map( VertexDataType data_type ) {
		switch ( data_type )	{
		case VertexDataType::Float:
		case VertexDataType::Float2:
		case VertexDataType::Float3:
		case VertexDataType::Float4: return GL_FLOAT;
		case VertexDataType::Int:
		case VertexDataType::Int2:
		case VertexDataType::Int3:
		case VertexDataType::Int4:   return GL_INT;
		case VertexDataType::UInt:
		case VertexDataType::UInt2:
		case VertexDataType::UInt3:
		case VertexDataType::UInt4:  return GL_UNSIGNED_INT;
		default:
			break;
		}

    THROW( "invalid vertex data type" );
    return GL_INVALID_ENUM;
	}

	static GLenum map( TextureFormat format ) {
		switch ( format ) 	{
		case TextureFormat::rgba8:   return GL_RGBA8;
		case TextureFormat::r8ui:    return GL_R8UI;
		case TextureFormat::rg8ui:   return GL_RG8UI;
		case TextureFormat::r32ui:   return GL_R32UI;
		case TextureFormat::rg32ui:  return GL_RG32UI;
		case TextureFormat::rgba32f: return GL_RGBA32F;
		case TextureFormat::Invalid:
		default:
			break;
		}

		THROW( "invalid texture format" );
		return GL_INVALID_ENUM;
	}

}
