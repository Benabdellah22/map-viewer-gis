#pragma once

#include <utils/singleton.h>

#include <graphics/buffers.h>
#include <graphics/shaders.h>
#include <graphics/framebuffer.h>
#include <graphics/renderstate.h>

#include <app/ortho-camera.h>

#include <debug/profiler.h>

namespace mv {

  #define MAX_VERTICES 262144 /* 4096 * 64 */

  /* Immgfx is short of immediate mode gfx, the renderer build */
  /* vertex data each frame and draw them together.            */
  class Immgfx : public Singleton<Immgfx> {
    REGISTER_SINGLETON_CLASS( Immgfx );
  protected:
    Immgfx( void );
    ~Immgfx( void );
  public:
    /* bind the framebuffers to capture color and polygon id */
    void bind( void );

    /* unbind the framebuffer and tranfer it's color data to screen */
    void unbind( void );

    /* begin drawing, this simple map the vertex buffer */
    void begin( void );

    /* draw the submitted vertices with the camera and topology */
    void draw( Ref<OrthoCamera> camera, Topology topology );

    /* draw the submitted vertices with the matrices and topology */
    void draw( const glm::mat4& projection, const glm::mat4& view, Topology topology );

    /* submit vertex to the renderer */
    void push_vertex( glm::vec3 position, glm::vec4 color );

    /* after draw you can call this to get primitve id at x, y */
    uint32_t get_primitive_id( uint32_t x, uint32_t y );

    /* resize the framebuffer */
    void resize( glm::uvec2 new_size );

    /* return the color attachment */
    inline Ref<Texture2D> color_attachment( void ) const { return _color_attachment; }
  private:
    Ref<Shader> _shader;

    Ref<VertexBuffer<Position3, Color4>> _vertices;

    Ref<Framebuffer> _framebuffer;
    Ref<Texture2D>   _color_attachment;
    Ref<Texture2D>   _primitive_id_attachment;

    size_t _vertex_index = 0;
  };

}
