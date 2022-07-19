#include <graphics/renderstate.h>

#include <glad/glad.h>
#include <utils/logger.h>

#include "typemap.h"

namespace mv {

  RenderState::RenderState( void ) {
    LOG_INFO( "render state created" );

    /* push a default viewport */
    push_viewport( { 0, 0, 0, 0, 0.0f, 0.0f } );

    /* push a default topology */
    push_topology( Topology::Point );
  }

  RenderState::~RenderState( void ) {

    /* pop deafult viewport and topology */
    pop_viewport();
    pop_topology();

    LOG_INFO( "render state destroyed" );
  }

  void RenderState::set_clear_color( glm::vec4 clear_color ) {
    glClearColor( clear_color.x, clear_color.y, clear_color.z, clear_color.w );
  }

  void RenderState::set_line_thickenss( float thickness ) {
    if ( thickness < 0.0f ) return;
    glLineWidth( thickness );
  }

  void RenderState::set_point_size( float size ) {
    if ( size < 0.0f ) return;
    glPointSize( size );
  }

  void RenderState::update( void ) {
    _stats = Stats();
  }

  void RenderState::push_topology( Topology topology ) {
    /* push the topology to the stack */
    _topologies.push( topology );
  }

  void RenderState::pop_topology( void ) {
    /* if there are topologies in the stack */
    /* remove the top one                   */
    if ( _topologies.size() > 0 ) {
      _topologies.pop();
    } else {
      LOG_WARN( "called pop_topology without any active topology in the stack it can be the case that pop_topology is called more number of times than push_topology" );
    }
  }

  void RenderState::push_viewport( Viewport viewport ) {
    PROFILE_FUNCTION();

    /* push the viewport to the stack */
    _viewports.push( viewport );

    /* set the viewport as active */
    set_viewport( viewport );
  }

  void RenderState::pop_viewport( void ) {
    PROFILE_FUNCTION();

    /* if there are viewports in the stack */
    /* remove the top one                  */
    if ( _viewports.size() > 0 ) {
      _viewports.pop();
    }

    if ( _viewports.size() > 0 ) {
      set_viewport( _viewports.top() );
    } else {
      LOG_WARN( "called pop_viewport without any active viewport in the stack it can be the case that pop_viewport is called more number of times than push_viewport" );
    }
  }

  void RenderState::draw( uint32_t num_vertices, uint32_t offset ) {
    PROFILE_FUNCTION();

    if ( _topologies.top() == Topology::Line )
      _stats.num_lines += num_vertices / 2;
    if ( _topologies.top() == Topology::Triangle )
      _stats.num_triangles += num_vertices / 3;
    if ( _topologies.top() == Topology::Point )
      _stats.num_points += num_vertices;

    _stats.draw_calls++;

    glDrawArrays( map( _topologies.top() ), offset, (GLsizei)num_vertices );
    glFlush();
  }

  void RenderState::draw_index( uint32_t index_count, uint32_t offset ) {
    PROFILE_FUNCTION();

    glDrawElements( map( _topologies.top() ), index_count, GL_UNSIGNED_INT, (void*)( offset * sizeof( GLuint ) ) );
    glFlush();
  }

  void RenderState::set_viewport( Viewport viewport ) {
    PROFILE_FUNCTION();

    /* set the viewport and depth range */
    glViewport( viewport.top_left_x, viewport.top_left_y, viewport.width, viewport.height );
    glDepthRange( viewport.min_depth, viewport.max_depth );
  }

}
