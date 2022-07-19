#pragma once

#include <mutex>
#include <future>

#include <imgui.h>

#include <core/layer.h>

#include <app/ortho-camera.h>
#include <app/editor-layer.h>
#include <app/map-layer.h>

#include <graphics/textures.h>
#include <tools/contentbrowser.h>

namespace mv {

  class ApplicationMenu : public Layer {
  public:
    ApplicationMenu( void );
    ~ApplicationMenu( void );
  public:
    /* use these functions if required */
    virtual void update( float delta_time ) override {}
    virtual void on_event( Event& e ) override {}

    /* imgui render function, put functions here, this is called after ImGui::BeginFrame */
    virtual void on_imgui_render( void ) override;

    inline void set_viewport_texture( Ref<Texture2D> texture ) { if ( texture ) _viewport_texture = texture; }
    inline void set_map_layer( std::vector<Ref<MapLayer>>* layers ) { _map_layers = layers; }
    inline void set_editor_layer( Ref<EditorLayer> layer ) { _editor_layer = layer; }
    inline void set_camera( Ref<OrthoCamera> camera ) { _camera = camera; }
    
    inline glm::uvec2 viewport_size( void ) const { return _viewport_size; }
    inline glm::ivec2 viewport_position( void ) const { return _viewport_position; }
    
    inline glm::vec4 view_mode_polygon_highligh_color( void ) const { return _view_mode_polygon_highligh_color; }
    inline glm::vec4 view_mode_line_highligh_color( void ) const { return _view_mode_line_highlight_color; }
    inline float view_mode_line_highlight_thickness( void ) const { return _viewmode_line_highlight_thickness; }

    /* get the selected layer */
    inline Ref<MapLayer> selected_layer( void ) const { return _selected_layer; }
  private:
    void init_dockspace( void );

    void draw_menubar( void );
    void draw_viewport( void );
    void draw_layer_menu( void );
    void draw_renderstate_menu( void );

    void draw_attributes( Ref<MapLayer> selected_layer );
    void draw_attibutes_menu( Ref<MapLayer> selected_layer );
    void draw_filter_menu( Ref<MapLayer> selected_layer );
  private:
    bool _show_menus = true;
    bool _show_demo_window = false;
    bool _show_implot_demo = false;
    bool _show_layer_menu = true;
    bool _show_renderstate_menu = true;
    bool _show_attributes_menu = true;
    bool _show_filter_menu = true;
    bool _show_browser = true;

    bool _viewport_focused = false;
    bool _viewport_hovered = false;

    glm::uvec2 _viewport_size = { 10, 10 };
    glm::ivec2 _viewport_position = { 0, 0 };
    Ref<Texture2D> _viewport_texture = nullptr;
    std::vector<Ref<MapLayer>>* _map_layers = nullptr;
    Ref<MapLayer> _selected_layer = nullptr;
    Ref<EditorLayer> _editor_layer = nullptr;
    Ref<OrthoCamera> _camera = nullptr;

    glm::vec4 _view_mode_polygon_highligh_color = glm::vec4( 1.00f, 0.95f, 0.00f, 0.87f );
    glm::vec4 _view_mode_line_highlight_color   = glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f );
    float _viewmode_line_highlight_thickness    = 3.0f;

    /* content browser */
    ContentBrowser _browser;

    /* internal stuff not to be exposed */
    std::unordered_map<string, ImGuiTextFilter> _attribute_filters;
    std::vector<polygon_id> _filtered_polygons;
    std::future<void> _filter_future;
    bool _refilter_attributes = true;
  };
}
