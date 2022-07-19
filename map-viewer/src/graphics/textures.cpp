#include <graphics/textures.h>

#include <utils/exception.h>
#include <utils/logger.h>

#include "typemap.h"

namespace mv {

  Texture2D::Texture2D( uint32_t width, uint32_t height, TextureFormat format ) {
    PROFILE_FUNCTION();

    if ( width == 0 || height == 0 )
      THROW( "cannot create texture with 0 width or height" );

    if ( format == TextureFormat::Invalid )
      THROW( "invalid texture format; cannot create texture" );

    /* create the OpenGL texture object */
    glCreateTextures( GL_TEXTURE_2D, 1, &_texture_id );
    if ( _texture_id == 0 )
      THROW( "failed to create OpenGL texture object" );

    /* bind the texture to allocate memory and change sampler state */
    glBindTexture( GL_TEXTURE_2D, _texture_id );

    /* apply linear filtering */
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    /* repeat the texture for wrapping */
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT );

    /* allocate memory for the texture */
    glTexStorage2D( GL_TEXTURE_2D, 1, map( format ), width, height );

    /* unbind the texture */
    glBindTexture( GL_TEXTURE_2D, 0 );

    /* assign the internal data */
    _width = width;
    _height = height;
    _format = format;

    LOG_INFO( "created OpenGL texture of size {}x{}: id = {}", _width, _height, _texture_id );
  }

  Texture2D::~Texture2D( void ) {
    PROFILE_FUNCTION();

    LOG_INFO( "deleted OpenGL texture: id = {}", _texture_id );
    glDeleteTextures( 1, &_texture_id );
  }

  void Texture2D::bind( uint32_t slot ) const {
    PROFILE_FUNCTION();

    glActiveTexture( GL_TEXTURE0 + slot );
    glBindTexture( GL_TEXTURE_2D, _texture_id );
  }

  void Texture2D::clear( uint32_t value ) {
    glClearTexImage( _texture_id, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &value );
  }

}
