#include <app/immgfx.h>

#include <core/window.h>

namespace mv {

  constexpr char immgfx_vs[] = R"(
    #version 460 core
  
    layout ( location = 0 ) in vec3 a_position;
    layout ( location = 1 ) in vec4 a_color;
  
    uniform mat4 projection;
    uniform mat4 view;
  
    out vec4 pass_color;
    flat out uint vertex_id;

    void main() {
      vertex_id = uint( gl_VertexID );
      pass_color = a_color;
      gl_Position = projection * view * vec4( a_position, 1.0f );
    }
  )";

  constexpr char immgfx_fs[] = R"(
    #version 460 core
  
    layout( location = 0 ) out vec4 frag_color;
    layout( location = 1 ) out uint polygon_id;
  
    in vec4 pass_color;

    flat in uint vertex_id;

    void main() {
      polygon_id = vertex_id + 1;
      frag_color = pass_color;
    }
  )";

  Immgfx::Immgfx( void ) {
    PROFILE_FUNCTION();

    /* create shader */
    _shader = new Shader( immgfx_vs, immgfx_fs );

    /* create capture framebuffer and attach color attachments */
    uint32_t w = Window::ref().width();
    uint32_t h = Window::ref().height();

    _framebuffer = new Framebuffer( true, w, h );
    _color_attachment = new Texture2D( w, h, TextureFormat::rgba8 );
    _primitive_id_attachment = new Texture2D( w, h, TextureFormat::r32ui );

    _framebuffer->set_color_attachment( _color_attachment, 0 );
    _framebuffer->set_color_attachment( _primitive_id_attachment, 1 );

    _vertices = new VertexBuffer<Position3, Color4>( MAX_VERTICES, nullptr );
  }

  Immgfx::~Immgfx( void ) {} /* do nothing */

  void Immgfx::bind( void ) {
    PROFILE_FUNCTION();

    /* bind and clear framebuffer */
    _framebuffer->bind();
    _framebuffer->clear();
  }

  void Immgfx::unbind( void ) {
    PROFILE_FUNCTION();

    /* unbind */
    _framebuffer->unbind();
  }

  void Immgfx::begin( void ) {
    /* clear the id texture with 0 regarless of clearColor */
    _primitive_id_attachment->clear( 0 );

    /* map the vertex buffer to modify it */
    _vertices->map();
  }

  void Immgfx::draw( Ref<OrthoCamera> camera, Topology topology ) {
    PROFILE_FUNCTION();

    /* unmap the vertex buffer before drawing to upload data to GPU */
    _vertices->unmap();

    /* draw the contents using topology */
    RenderState::ref().push_topology( topology );
    {
      _shader->set_input_buffers( _vertices );
      _shader->use();

      _shader->set_mat4( "projection", camera->projection() );
      _shader->set_mat4( "view", camera->view() );

      RenderState::ref().draw( _vertex_index );
    }
    RenderState::ref().pop_topology();

    _vertex_index = 0;
  }

  void Immgfx::draw( const glm::mat4& projection, const glm::mat4& view, Topology topology ) {
    PROFILE_FUNCTION();

    /* unmap the vertex buffer before drawing to upload data to GPU */
    _vertices->unmap();

    /* draw the contents using topology */
    RenderState::ref().push_topology( topology );
    {
      _shader->set_input_buffers( _vertices );
      _shader->use();

      _shader->set_mat4( "projection", projection );
      _shader->set_mat4( "view", view );

      RenderState::ref().draw( _vertex_index );
    }
    RenderState::ref().pop_topology();

    _vertex_index = 0;
  }

  void Immgfx::push_vertex( glm::vec3 position, glm::vec4 color ) {
    struct pc {
      Position3 pos;
      Color4 col;
    };

    /* push the vertex position along with color */
    pc p;
    p.pos = { position.x, position.y, position.z };
    p.col = { color.r, color.g, color.b, color.a };

    auto buff = _vertices->mapped_buffer();
    memcpy( buff + _vertices->vertex_size() * _vertex_index, &p, _vertices->vertex_size() );

    _vertex_index++;
  }

  uint32_t Immgfx::get_primitive_id( uint32_t x, uint32_t y ) {
    PROFILE_FUNCTION();

    /* get primitive id at x, y. the id is writting using gl_VertexID */
    return _framebuffer->read_pixel( 1, x, y );
  }

  void Immgfx::resize( glm::uvec2 new_size ) {
    PROFILE_FUNCTION();

    /* resize the framebuffer and it's color attachments */
    uint32_t w = new_size.x;
    uint32_t h = new_size.y;

    _framebuffer = new Framebuffer( true, w, h );
    _color_attachment = new Texture2D( w, h, TextureFormat::rgba8 );
    _primitive_id_attachment = new Texture2D( w, h, TextureFormat::r32ui );

    _framebuffer->set_color_attachment( _color_attachment, 0 );
    _framebuffer->set_color_attachment( _primitive_id_attachment, 1 );
  }

}
