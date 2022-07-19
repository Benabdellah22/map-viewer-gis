#pragma once

#include <vector>

#include <graphics/textures.h>

#include <types.h>
#include <utils/ref.h>
#include <core/math.h>

#include <debug/profiler.h>

namespace mv {

  class Framebuffer : public SharedObject {
  public:
    Framebuffer( bool allocate_depth_buffer = false, uint32_t width = 0u, uint32_t height = 0u );
    ~Framebuffer( void );
  public:

    /* Bind the framebuffer, all subsiquent draw */
    /* calls will draw to the bound framebuffer. */
    void bind( void ) const;

    /* Unbind framebuffer, all subsiquent draw     */
    /* calls will draw to the default framebuffer. */
    void unbind( void ) const;

    /* clear the framebuffer */
    void clear( void ) const;

    /* set color attachment */
    void set_color_attachment( Ref<Texture2D> texture, uint32_t attachment_index );

    /* copy framebuffer data to the screen; only the buffer with attachment_index will be copied */
    void blit_to_screen( uint32_t attachment_index );

    /* read the pixel color at x, y */
    uint32_t read_pixel( uint32_t attachment_index, uint32_t x, uint32_t y );

    /* read two component unsigned integer pixel color at x, y */
    glm::uvec2 read_pixel_2u( uint32_t attachment_index, uint32_t x, uint32_t y );

    /* read float pixel color at x, y */
    glm::vec4 read_float_pixel( uint32_t attachment_index, uint32_t x, uint32_t y );

    /* getter functions */

    /* return native framebuffer id */
    inline uint32_t id( void ) const { return _framebuffer; }

    /* return width: will return 0 if no depth buffer is allocated */
    inline uint32_t width( void ) const { return _width; }

    /* return height: will return 0 if no depth buffer is allocated */
    inline uint32_t height( void ) const { return _height; }
  private:
    /* OpenGL framebuffer id */
    uint32_t _framebuffer = 0;

    /* OpenGL depth buffer storage */
    uint32_t _depth_buffer = 0;

    /* width of the depth buffer */
    uint32_t _width = 0;

    /* height of the depth buffer ( or number of rows ) */
    uint32_t _height = 0;

    /* ------- internal stuff ------- */
    std::vector<uint32_t> _attachment_slots;
    mutable bool _rebuild_draw_buffers = false;
  };

}
