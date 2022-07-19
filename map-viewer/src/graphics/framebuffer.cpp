#include <graphics/framebuffer.h>

#include <glad/glad.h>

#include <utils/exception.h>
#include <utils/logger.h>
#include <utils/assert.h>

namespace mv {

  Framebuffer::Framebuffer( bool allocate_depth_buffer, uint32_t width, uint32_t height ) {
    PROFILE_FUNCTION();

    if ( allocate_depth_buffer && ( width == 0 || height == 0 ) )
      THROW( "cannot create framebuffer with 0 width or height while allocating depth buffer" );

    /* create the OpenGL framebuffer object */
    glCreateFramebuffers( 1, &_framebuffer );
    if ( _framebuffer == 0 )
      THROW( "failed to create OpenGL framebuffer object" );

    /* allocate depth buffer if required */
    if ( allocate_depth_buffer ) {

      /* create renderbuffer */
      glCreateRenderbuffers( 1, &_depth_buffer );
      if ( _depth_buffer == 0 )
        THROW( "failed to create OpenGL framebuffer's depth buffer renderbuffer" );

      /* allocate memory for depth buffer */
      glNamedRenderbufferStorage( _depth_buffer, GL_DEPTH_COMPONENT24, width, height );

      /* attach renderbuffer to framebufer */
      glNamedFramebufferRenderbuffer( _framebuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depth_buffer );

      /* assign internal data */
      _width = width;
      _height = height;
    }

    LOG_INFO( "created opengl framebuffer: id = {}", _framebuffer );
  }

  Framebuffer::~Framebuffer( void ) {
    PROFILE_FUNCTION();

    LOG_INFO( "deleted opengl framebuffer: id = {}", _framebuffer );
    glDeleteFramebuffers( 1, &_framebuffer );

    /* delete depth buffer if created */
    if ( _depth_buffer != 0 ) {
      glDeleteRenderbuffers( 1, &_depth_buffer );
    }
  }

  void Framebuffer::bind( void ) const {
    PROFILE_FUNCTION();

    if ( _rebuild_draw_buffers ) {
      std::vector<GLenum> draw_buffer( _attachment_slots.size() );

      for ( size_t i = 0; i < _attachment_slots.size(); i++ )
        draw_buffer[i] = GL_COLOR_ATTACHMENT0 + _attachment_slots[i];

      glNamedFramebufferDrawBuffers( _framebuffer, draw_buffer.size(), draw_buffer.data() );
      _rebuild_draw_buffers = false;
    }

    glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer );
  }

  void Framebuffer::unbind( void ) const {
    PROFILE_FUNCTION();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  }

  void Framebuffer::clear( void ) const {
    PROFILE_FUNCTION();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  }

  void Framebuffer::set_color_attachment( Ref<Texture2D> texture, uint32_t attachment_index ) {
    PROFILE_FUNCTION();

    MV_ASSERT( texture );

    /* attach the texture to the framebuffer */
    glNamedFramebufferTexture( _framebuffer, GL_COLOR_ATTACHMENT0 + attachment_index, texture->id(), 0 );

    /* insert attachment index in the list */
    if ( std::find( _attachment_slots.begin(), _attachment_slots.end(), attachment_index ) == _attachment_slots.end() ) {
      _attachment_slots.push_back( attachment_index );
      _rebuild_draw_buffers = true;
    }
  }

  void Framebuffer::blit_to_screen( uint32_t attachment_index ) {
    PROFILE_FUNCTION();

    /* bind the source and target buffer */
    glBindFramebuffer( GL_READ_FRAMEBUFFER, _framebuffer );
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

    /* enable attachment to copy from and copy to */
    glReadBuffer( GL_COLOR_ATTACHMENT0 + attachment_index );

    /* copy the buffer */
    glBlitFramebuffer( 0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST );

    /* unbind buffers */
    glReadBuffer( GL_COLOR_ATTACHMENT0 );
    glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
  }

  uint32_t Framebuffer::read_pixel( uint32_t attachment_index, uint32_t x, uint32_t y ) {
    PROFILE_FUNCTION();

    /* bind the framebuffer for reading pixel */
    glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer );

    uint32_t pixel_data = 0;

    /* read the data from attaachment */
    glReadBuffer( GL_COLOR_ATTACHMENT0 + attachment_index );
    glReadPixels( x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel_data );

    /* restore original state; might need to remove this (xD removed it cause it unbind the buffer in consicutive draw calls) */
    // glReadBuffer( GL_COLOR_ATTACHMENT0 );
    // glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return pixel_data;
  }

  glm::uvec2 Framebuffer::read_pixel_2u( uint32_t attachment_index, uint32_t x, uint32_t y ) {
    PROFILE_FUNCTION();

    /* bind the framebuffer for reading pixel */
    glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer );

    glm::uvec2 pixel_data( 0 );

    /* read the data from attaachment */
    glReadBuffer( GL_COLOR_ATTACHMENT0 + attachment_index );
    glReadPixels( x, y, 1, 1, GL_RG_INTEGER, GL_UNSIGNED_INT, &pixel_data[0] );

    /* restore original state; might need to remove this */
    glReadBuffer( GL_COLOR_ATTACHMENT0 );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return pixel_data;
  }

  glm::vec4 Framebuffer::read_float_pixel( uint32_t attachment_index, uint32_t x, uint32_t y ) {
    PROFILE_FUNCTION();

    /* bind the framebuffer for reading pixel */
    glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer );

    glm::vec4 pixel_data( 0.0f );

    /* read the data from attaachment */
    glReadBuffer( GL_COLOR_ATTACHMENT0 + attachment_index );
    glReadPixels( x, y, 1, 1, GL_RGBA, GL_FLOAT, &pixel_data );

    /* restore original state; might need to remove this */
    glReadBuffer( GL_COLOR_ATTACHMENT0 );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return pixel_data;
  }

}
