
#include <utils/logger.h>
#include <utils/file-reader.h>
#include <utils/exception.h>

#include <core/window.h>
#include <core/timer.h>
#include <core/input.h>

#include <graphics/renderstate.h>
#include <graphics/buffers.h>
#include <graphics/shaders.h>
#include <graphics/textures.h>
#include <graphics/framebuffer.h>

#include <app/rectangular-selector.h>
#include <app/ortho-camera.h>
#include <app/editor-layer.h>
#include <app/map-layer.h>
#include <app/immgfx.h>
#include <app/menu.h>

#include <base/window-events.h>

#include <core/core.h>

#include <debug/profiler.h>


using namespace mv;

constexpr char vs[] = R"(
  #version 460 core

  layout ( location = 0 ) in vec3  a_position;
  layout ( location = 1 ) in uvec2 a_id;

  uniform mat4 projection;
  uniform mat4 view;

  flat out uint pass_polygon_id;

  void main() {
    pass_polygon_id = a_id.x;
    gl_Position = projection * view * vec4( a_position, 1.0f );
  }
)";

constexpr char ps[] = R"(
  #version 460 core

  layout( location = 0 ) out vec4 frag_color;
  layout( location = 1 ) out vec4 polygon_id;

  uniform vec4 color;
  uniform vec4 highlight_color;

  uniform int rectangular_selection;
  uniform ivec2 mouse_pos;
  uniform ivec3 horizontal;
  uniform ivec3 vertical;

  uniform int highlight_num;
  uniform uint highlight[64];

  uniform int is_selected;

  layout ( std430 ) volatile buffer polygon_ids {
    int written_elements;
    int polygon_ids_buffer[];
  };

  flat in uint pass_polygon_id;

  void main() {

    frag_color = vec4( color );

    if ( is_selected != 1 ) {
      return;
    }

    int x = int( gl_FragCoord.x - 0.5f );
    int y = int( gl_FragCoord.y - 0.5f );
    
    if ( rectangular_selection != 1 ) {
      if ( x == mouse_pos.x && y == mouse_pos.y && pass_polygon_id != 0 && written_elements < polygon_ids_buffer.length() ) {
        int index = atomicAdd( written_elements, 1 );
        atomicAdd( polygon_ids_buffer[index], int( pass_polygon_id ) );
      }
    } else {
      /* horizontal */

      ivec2 hy = ivec2( min( horizontal.y, horizontal.z ), max( horizontal.y, horizontal.z ) );

      if ( x == horizontal.x && y >= hy.x && y <= hy.y &&
           pass_polygon_id != 0 && written_elements < polygon_ids_buffer.length() ) {
        int index = atomicAdd( written_elements, 1 );
        atomicAdd( polygon_ids_buffer[index], int( pass_polygon_id ) );
      }

      /* vertical */

      ivec2 vx = ivec2( min( vertical.y, vertical.z ), max( vertical.y, vertical.z ) );

      if ( y == vertical.x && x >= vx.x && x <= vx.y &&
           pass_polygon_id != 0 && written_elements < polygon_ids_buffer.length() ) {
        int index = atomicAdd( written_elements, 1 );
        atomicAdd( polygon_ids_buffer[index], int( pass_polygon_id ) );
      }
    }

    int i = 0;
    for ( i = 0; i < highlight_num; i++ ) {
      if ( pass_polygon_id == highlight[i] ) {
        frag_color = highlight_color;
      }
    }
  }
)";

constexpr int MAX_ARRAY_SIZE = 4096;

struct PolygonIDBuffer {
  int32_t written_elements            = 0;
  int32_t polygon_ids[MAX_ARRAY_SIZE] = {};
};

Ref<ApplicationMenu> menu;

class Templayer : public Layer {
public:
  Templayer( void ) : Layer( "temporary render layer" ) {
    _color_buffer = new Texture2D( Window::ref().width(), Window::ref().height(), TextureFormat::rgba8 );
    _polygon_id_buffer = new Texture2D( Window::ref().width(), Window::ref().height(), TextureFormat::rgba32f );

    _ssbo = new ShaderStorageBuffer<PolygonIDBuffer>( nullptr );

    _framebuffer = new Framebuffer( true, Window::ref().width(), Window::ref().height() );
    _framebuffer->set_color_attachment( _color_buffer, 0 );
    _framebuffer->set_color_attachment( _polygon_id_buffer, 1 );

    _map_layers.push_back( new MapLayer( "../../../../json/PROV.geojson" ) );

    _ss = new Shader( vs, ps );
    
    Box bbox = _map_layers[0]->bounding_box();

    auto c = bbox.center();
    c.z = 400.0f;

    _camera = new OrthoCamera();
    _camera->set_position( c );

    _editor_layer = new EditorLayer();

    menu->set_viewport_texture( _color_buffer );
    menu->set_map_layer( &_map_layers );
    menu->set_editor_layer( _editor_layer );
    menu->set_camera( _camera );
  }

public:
  virtual void update( float delta_time ) override {
    PROFILE_FUNCTION();

    Ref<MapLayer> selected_layer = menu->selected_layer();
    
    glm::uvec2 viewport_pos = menu->viewport_position();
    Input::set_viewport_position( viewport_pos );

    _camera->update();

    if ( _editor_layer->is_editing() )
      _editor_layer->update( delta_time );
    else if ( selected_layer != nullptr ) {
      if ( Input::is_mouse_button_pressed( MOUSE_BUTTON_LEFT ) || _selector.is_dragging() ) {
        PolygonIDBuffer buff = ( *_ssbo )[0];

        if ( !Input::is_key_down( KEY_LEFT_CONTROL ) )
          selected_layer->clear_selected();

        buff.written_elements = std::clamp( buff.written_elements, 0, MAX_ARRAY_SIZE );
        for ( int32_t i = 0; i < buff.written_elements; i++ ) {
          selected_layer->revert_select( buff.polygon_ids[i] );
        }
      }
    }

    if ( Input::is_key_pressed( KEY_O ) ) {
      if ( _editor_layer->is_editing() )
        _editor_layer->end();
      else if ( selected_layer != nullptr && selected_layer->selected_geometries().size() > 0 )
        _editor_layer->start( selected_layer, selected_layer->selected_geometries()[0], _camera );
    }

    /* dragging */
    // {
    //   if ( _selector.is_dragging() )
    //     _selector.update();
    //   if ( Input::is_mouse_button_pressed( MOUSE_BUTTON_LEFT ) ) {
    //     if ( selected_layer )
    //       selected_layer->clear_selected();
    //     _selector.start();
    //   }
    //   if ( Input::is_mouse_button_released( MOUSE_BUTTON_LEFT ) )
    //     _selector.end();
    // }

    glm::uvec2 viewport_size = { _framebuffer->width(), _framebuffer->height() };
    if ( viewport_size != menu->viewport_size() ) {
      window_resize( WindowResizeEvent( menu->viewport_size().x, menu->viewport_size().y ) );
    }

    if ( _editor_layer->is_editing() )
      menu->set_viewport_texture( Immgfx::ref().color_attachment() );
    else
      menu->set_viewport_texture( _color_buffer );
  }
  
  void base_render( void ) {
    PROFILE_FUNCTION();

    _framebuffer->bind();
    _framebuffer->clear();

    for ( auto it = _map_layers.rbegin(); it < _map_layers.rend(); it++ ) {
      if ( !(*it)->is_hidden() )
        render_layer( *it );
    }

    _framebuffer->unbind();
  }

  void render_layer( Ref<MapLayer> layer ) {
    PROFILE_FUNCTION();

    bool layer_selected = layer == menu->selected_layer();

    if ( layer->should_draw_triangles() ) {

      RenderState::ref().push_topology( Topology::Triangle );
      {
        _ss->set_input_buffers( layer->vertex_buffer() );
        _ss->use();

        /* temporary draw function */
        _ss->set_mat4( "projection", _camera->projection() );
        _ss->set_mat4( "view", _camera->view() );
        _ss->set_vec4( "color", layer->get_polygon_fill_color() );
        _ss->set_vec4( "highlight_color", menu->view_mode_polygon_highligh_color() );
        
        _ss->set_ssbo( "polygon_ids", _ssbo );

        if ( _selector.is_dragging() ) {
          glm::ivec2 top_left = _selector.top_left();
          glm::ivec2 bottom_right = _selector.bottom_right();
          int height = _framebuffer->height();

          top_left.y = height - top_left.y;
          bottom_right.y = height - bottom_right.y;

          glm::ivec3 horizontal = _selector.horizontal();
          glm::ivec3 vertical = _selector.vertical();

          horizontal.y = height - horizontal.y;
          horizontal.z = height - horizontal.z;

          vertical.x = height - vertical.x;

          _ss->set_int( "rectangular_selection", (int)1 );
          _ss->set_ivec3( "horizontal", horizontal );
          _ss->set_ivec3( "vertical", vertical );
        }
        else {
          _ss->set_int( "rectangular_selection", (int)0 );
          _ss->set_ivec2( "mouse_pos", { (int32_t)Input::mouse_position().x, (int32_t)_framebuffer->height() - (int32_t)Input::mouse_position().y } );
        }

        auto& selected = layer->selected_geometries();

        _ss->set_int( "highlight_num", (int)selected.size() );
        _ss->set_uint_array( "highlight", selected.size(), selected.data() );

        _ss->set_int( "is_selected", layer_selected ? 1 : 0 );

        RenderState::ref().draw( layer->vertex_buffer()->array_size() );

        if ( layer_selected ) {
          _ssbo->retrieve();
        }

      }
      RenderState::ref().pop_topology();

    }

    if ( layer->should_draw_outlines() ) {

      RenderState::ref().push_topology( Topology::Line );
      {
        RenderState::ref().set_line_thickenss( layer->get_line_thickness() );

        _ss->set_input_buffers( layer->outline_buffer() );
        _ss->use();

        _ss->set_mat4( "projection", _camera->projection() );
        _ss->set_mat4( "view", _camera->view() );
        _ss->set_vec4( "color", layer->get_line_color() );

        RenderState::ref().draw( layer->outline_buffer()->array_size() );
      }
      {
        auto& selected = layer->selected_geometries();

        if ( selected.size() > 0 ) {
          RenderState::ref().set_line_thickenss( menu->view_mode_line_highlight_thickness() );
          _ss->set_vec4( "color", menu->view_mode_line_highligh_color() );

          for ( const auto& id : selected ) {
            auto& [offset, size] = layer->get_outline_indices( id );
            RenderState::ref().draw( size, offset );
          }
        }

      }
      RenderState::ref().pop_topology();

    }

    if ( _selector.is_dragging() ) {
      Immgfx::ref().begin();

      glm::vec2 top_left = _selector.top_left();
      glm::vec2 bottom_right = _selector.bottom_right();
      float height = (float)_framebuffer->height();

      top_left.y = height - top_left.y;
      bottom_right.y = height - bottom_right.y;

      Immgfx::ref().push_vertex( glm::vec3( top_left.x, top_left.y, 0.0f ), glm::vec4( 0.5f ) );
      Immgfx::ref().push_vertex( glm::vec3( bottom_right.x, top_left.y, 0.0f ), glm::vec4( 0.5f ) );
      Immgfx::ref().push_vertex( glm::vec3( top_left.x, bottom_right.y, 0.0f ), glm::vec4( 0.5f ) );

      Immgfx::ref().push_vertex( glm::vec3( top_left.x, bottom_right.y, 0.0f ), glm::vec4( 0.5f ) );
      Immgfx::ref().push_vertex( glm::vec3( bottom_right.x, top_left.y, 0.0f ), glm::vec4( 0.5f ) );
      Immgfx::ref().push_vertex( glm::vec3( bottom_right.x, bottom_right.y, 0.0f ), glm::vec4( 0.5f ) );

      Immgfx::ref().draw( glm::ortho( 0.0f, (float)_framebuffer->width(), 0.0f, height ), glm::mat4( 1.0f ), Topology::Triangle );
    }

  }

  virtual void render( void ) override {
    PROFILE_FUNCTION();

    if ( _editor_layer->is_editing() )
      _editor_layer->render();
    else
      base_render();
  }

  void on_event( Event& e ) override {
    EventDispatcher dispatcher( e );

    // dispatcher.dispatch<WindowResizeEvent>( BIND_EVENT_FN( Templayer::window_resize ) );
  }
public:
  bool window_resize( WindowResizeEvent& e ) {
    _camera->resize( glm::vec2( e.x(), e.y() ) );
    _editor_layer->set_size( glm::uvec2( e.x(), e.y() ) );

    RenderState::ref().pop_viewport();
    RenderState::ref().push_viewport( { 0, 0, e.x(), e.y(), 0.0f, 1.0f } );

    _color_buffer = new Texture2D( e.x(), e.y(), TextureFormat::rgba8 );
    _polygon_id_buffer = new Texture2D( e.x(), e.y(), TextureFormat::r32ui );

    _framebuffer = new Framebuffer( true, e.x(), e.y() );
    _framebuffer->set_color_attachment( _color_buffer, 0 );
    _framebuffer->set_color_attachment( _polygon_id_buffer, 1 );

    return false;
  }
private:
  Ref<EditorLayer> _editor_layer;

  Ref<OrthoCamera> _camera;

  std::vector<Ref<MapLayer>> _map_layers;

  Ref<Shader> _ss;
  Ref<ShaderStorageBuffer<PolygonIDBuffer>> _ssbo;

  Ref<Texture2D>   _color_buffer;
  Ref<Texture2D>   _polygon_id_buffer;
  Ref<Framebuffer> _framebuffer;

  RectangularSelector _selector;
};

int main( int argc, char* argv[] ) {

  PROFILE_BEGIN_SESSION( "map viewer", "profiler_results.json" );

  try {
    Core::create_instance( 1280, 720, "Cartographic Map Viewer" );

    menu = new ApplicationMenu();

    Core::ref().push_layer( new Templayer() );
    Core::ref().push_layer( menu );
    Core::ref().run();

    menu = nullptr;

    Core::destory_instance();
  } catch ( const std::exception& e ) {
    LOG_CRITICAL( "runtime exception: {}", e.what() );
  }

  PROFILE_END_SESSION();

  return 0;
}
