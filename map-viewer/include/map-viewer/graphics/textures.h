#pragma once

#include <types.h>
#include <utils/ref.h>

namespace mv {

  /* Valid texture formats */
  enum class TextureFormat {
    Invalid, /* invalid texture format, used to initialize variable             */
    rgba8,   /* 4 channels with 8-bit per channel, a total of 32-bits per pixel */

    r8ui,    /* single channel with 8-bit unsigned integer data                 */
    rg8ui,   /* two channel with 8-bit unsigned integer data                    */
    r32ui,   /* single channel with 32-bit unsigned integer data                */
    rg32ui,  /* two channel with 32-bit unsigned integer data                   */

    rgba32f, /* 4 channels with 32-bit floating point per channel               */
  };

  class Texture2D : public SharedObject {
  public:
    Texture2D( uint32_t width, uint32_t height, TextureFormat format );
    ~Texture2D( void );
  public:

    /* bind the texture to the pipeline at a specific slot */
    void bind( uint32_t slot = 0 ) const;

    /* clear the texture with value */
    void clear( uint32_t value );

    /* getter functions */

    /* return native texture id */
    inline uint32_t id( void ) const { return _texture_id; }

    /* return width */
    inline uint32_t width( void ) const { return _width; }

    /* return height */
    inline uint32_t height( void ) const { return _height; }

    /* return format */
    inline TextureFormat format( void ) const { return _format; }
  private:
    /* OpenGL texture id */
    uint32_t _texture_id = 0;

    /* width of the texture */
    uint32_t _width = 0;

    /* height of the texture ( or number of rows ) */
    uint32_t _height = 0;

    /* abstract format */
    TextureFormat _format;
  };

}
