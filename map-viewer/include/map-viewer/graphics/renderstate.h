#pragma once

#include <stack>
#include <unordered_map>

#include <core/math.h>
#include <graphics/context.h>

namespace mv {

  /* RenderState can be used to modify the fixed function */
  /* pipeline in the graphics driver. For the most part   */
  /* it uses stack so that if a function is modified many */
  /* times in a loop, after popping the previous state is */
  /* restored. This is necessary if there are multiple    */
  /* passes in a rendering pipeline.                      */

  /* valid topologies */
  enum class Topology {
    Point,
    Line,
    LineStrip,
    LineAdjacent,
    LineStripAdjacent,
    Triangle,
    TriangleStrip,
    TriangleAdjacent,
    TriangleStripAdjacent,
  };

  /* viewport defines the drawing area */
  struct Viewport {
    uint32_t top_left_x, top_left_y;
    uint32_t width, height;
    float    min_depth, max_depth;
  };

  struct Stats {
    uint32_t num_triangles = 0;
    uint32_t num_lines     = 0;
    uint32_t num_points    = 0;
    uint32_t draw_calls    = 0;
  };

  class RenderState : public Singleton<RenderState> {
    REGISTER_SINGLETON_CLASS( RenderState );
  protected:
    RenderState( void );
    ~RenderState( void );
  public:
    /* set clear color, all framebuffers are cleared using */
    /* this color by default                               */
    void set_clear_color( glm::vec4 clear_color );

    /* set thickness of lines drawn by OpenGL */
    void set_line_thickenss( float thickness );

    /* set the point size drawn by OpenGL */
    void set_point_size( float size );

    /* used to reset stats */
    void update( void );

    /* push and pop topology */
    void push_topology( Topology topology );
    void pop_topology( void );

    /* push and pop topology */
    void push_viewport( Viewport viewport );
    void pop_viewport( void );

    /* draw call */
    void draw( uint32_t num_vertices, uint32_t offset = 0 );
    void draw_index( uint32_t index_count, uint32_t offset = 0 );

    /* return stats */
    inline Stats stats( void ) const { return _stats; }
  private:
    /* internal helper functions */
    void set_viewport( Viewport viewport );
  private:
    std::stack<Topology> _topologies;
    std::stack<Viewport> _viewports;

    Stats _stats;
  };

}
