#include <app/menu.h>

#include <core/timer.h>
#include <graphics/renderstate.h>
#include <graphics/imguicontext.h>

#include "imgui_style.inl"

#include <misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>
#include <implot.h>

namespace mv {

  ApplicationMenu::ApplicationMenu( void ) : Layer( "application menu layer" ), _browser( "../../../../" ) {
    set_imgui_font( "../../../../fonts/LatoLatin-Regular.ttf" );
    set_imgui_style_silver();

    _browser.load_fonts();
  }

  ApplicationMenu::~ApplicationMenu( void ) {}

  void ApplicationMenu::on_imgui_render( void ) {
    draw_menubar();
    init_dockspace();
    draw_viewport();

    if ( Input::is_key_pressed(KEY_TAB) ) {
        _show_menus = !_show_menus;

        if (_show_menus == false) {
            _show_layer_menu = true;
            _show_renderstate_menu = true;
            _show_attributes_menu = true;
            _show_browser = true;
            //_show_filter_menu = true;
            _show_menus = false;
        }
        else {
            _show_demo_window = false;
            _show_layer_menu = false;
            _show_renderstate_menu = false;
            _show_attributes_menu = false;
            _show_filter_menu = false;
            _show_browser = false;
            _show_menus = true;
        }
    }

    if ( _show_layer_menu)
      draw_layer_menu();

    if ( _show_renderstate_menu)
      draw_renderstate_menu();

    if ( _show_attributes_menu )
      draw_attributes( _selected_layer );

    if ( _show_demo_window )
      ImGui::ShowDemoWindow( &_show_demo_window );

    if ( _show_implot_demo )
      ImPlot::ShowDemoWindow( &_show_implot_demo );

    if (_show_browser)
    _browser.draw();

  }

  void ApplicationMenu::init_dockspace( void ) {
    /* implimentation taken directly from imgui demo program */

    static bool fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

    if ( fullscreen ) {
      const ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos( viewport->WorkPos );
      ImGui::SetNextWindowSize( viewport->WorkSize );
      ImGui::SetNextWindowViewport( viewport->ID );
      ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
      ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
      window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
      window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if ( dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode )
      window_flags |= ImGuiWindowFlags_NoBackground;

    if ( !opt_padding )
      ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
    ImGui::Begin( "DockSpace Demo", (bool*)0, window_flags );
    if ( !opt_padding )
      ImGui::PopStyleVar();

    ImGuiIO& io = ImGui::GetIO();
    if ( io.ConfigFlags & ImGuiConfigFlags_DockingEnable ) {
      ImGuiID dockspace_id = ImGui::GetID( "MyDockSpace" );
      ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), dockspace_flags );
    }
    if ( fullscreen )
      ImGui::PopStyleVar( 2 );

    ImGui::End();
  }

  void ApplicationMenu::draw_menubar( void ) {
    if ( ImGui::BeginMainMenuBar() ) {
      if ( ImGui::BeginMenu( "Misc" ) ) {
        ImGui::MenuItem( "Show Demo Window", "", &_show_demo_window );
        ImGui::MenuItem( "Show ImPlot Demo", "", &_show_implot_demo );
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
  }

  void ApplicationMenu::draw_viewport( void ) {
    ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0, 0, 0, 1 ) );
    ImGui::PushStyleColor( ImGuiCol_ChildBg, ImVec4( 0, 0, 0, 1 ) );
    if ( ImGui::Begin( "Viewport", (bool*)0, ImGuiWindowFlags_MenuBar || ImGuiWindowFlags_NoTitleBar ) ) {

      _viewport_focused = ImGui::IsWindowFocused();
      _viewport_hovered = ImGui::IsWindowHovered();

      ImGuiContext::ref().block_events( !_viewport_hovered );

      ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
      ImVec2 viewport_position = ImGui::GetCursorScreenPos();
      _viewport_size = { (uint32_t)viewport_panel_size.x, (uint32_t)viewport_panel_size.y };
      _viewport_position = { viewport_position.x, viewport_position.y };

      if ( _viewport_texture )
        ImGui::Image( (ImTextureID)_viewport_texture->id(), viewport_panel_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 } );

      /* popup to select one polygon from multiple selected */
      if ( _selected_layer != nullptr ) {
        if ( Input::is_mouse_button_pressed( MOUSE_BUTTON_RIGHT ) && _selected_layer->selected_geometries().size() > 1 ) {
          ImGui::OpenPopup( "Polygon Selection" );
        }

        if ( ImGui::BeginPopup( "Polygon Selection", ImGuiWindowFlags_AlwaysAutoResize ) ) {
          for ( const auto& geom : _selected_layer->selected_geometries() ) {
            ImGui::PushID( geom );
            if ( ImGui::Button( ( "ID: " + std::to_string( geom ) ).c_str() ) ) {
              _selected_layer->clear_selected();
              _selected_layer->select( geom );
              ImGui::PopID();
              ImGui::CloseCurrentPopup();
              break;
            }
            ImGui::PopID();
          }
          ImGui::EndPopup();
        }
      }

    }
    ImGui::PopStyleColor( 2 );
    ImGui::End();
  }

  void ApplicationMenu::draw_layer_menu( void ) {
    
    if ( ImGui::Begin( "Layers", &_show_layer_menu ) ) {

      /* drag drop files */
      ImVec2 cursor = ImGui::GetCursorPos();
      ImGui::Dummy( ImGui::GetContentRegionAvail() );
      ImGui::SetCursorPos( cursor );

      if ( ImGui::BeginDragDropTarget() ) {
        if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "DND_FILE_CB" ) ) {
          string file_path = (const char*)payload->Data;
          Ref<MapLayer> layer = new MapLayer( file_path );
          if ( !layer->empty() ) {
            _map_layers->push_back( layer );
          }
        }
        ImGui::EndDragDropTarget();
      }

      {
        /* TEMPORARY */
        static string path = "";

        ImGui::InputText( "Path", &path );

        if ( ImGui::Button( "Load Layer" ) ) {
          Ref<MapLayer> layer = new MapLayer("../../../../json/" + path + ".geojson" );

          if ( !layer->empty() ) {
            _map_layers->push_back( layer );
          }

        }
        ImGui::SameLine();

        if ( ImGui::Button( "Set Styles" ) ) {
            //change backgroud color
          RenderState::ref().set_clear_color( glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

          //set all layer styles by default
          for ( size_t n = 0; n < _map_layers->size(); n++ ) {
            Ref<MapLayer> item = ( *_map_layers )[n];

            string curName = item->name().c_str();

            if ( curName == "PR" ) {
              item->set_polygon_fill_color( glm::vec4( 0.000f, 0.446f, 1.000f, 0.142f ) );
              item->set_line_color( glm::vec4( 0.000f, 0.579f, 0.825f, 0.147f ) );
            }
            if ( curName == "PH" ) {
              item->set_polygon_fill_color( glm::vec4( 1.000, 0.824, 0.137, 0.043 ) );
              item->set_line_color( glm::vec4( 1.000, 0.823, 0.133, 0.085 ) );
            }
            if ( curName == "PROV" ) {
              item->set_polygon_fill_color( glm::vec4( 1.000f, 1.000f, 1.000f, 0.00f ) );
              item->set_line_color( glm::vec4( 1.000, 0.000, 0.000, 1.000 ) );
              item->set_line_thickness( 2.0f );
              item->set_draw_triangles( false );
            }
            if ( curName == "MUN" ) {
              item->set_polygon_fill_color( glm::vec4( 1.000f, 1.000f, 1.000f, 0.00f ) );
              item->set_line_color( glm::vec4( 1.000, 0.569, 0.000, 1.000 ) );
              item->set_line_thickness( 1.5f );
              item->set_draw_triangles( false );
            }
            if ( curName == "LUG" ) {
              item->set_polygon_fill_color( glm::vec4( 1.000f, 1.000f, 1.000f, 0.00f ) );
              item->set_line_color( glm::vec4( 0.00, 0.631, 1.00, 1.000 ) );
              item->set_draw_triangles( false );
            }
          }
        }
        ImGui::Separator();
      }

      auto layer_props = [&] ( Ref<MapLayer> item ) -> void {

        if ( ImGui::BeginPopup( "Layer Properties", ImGuiWindowFlags_AlwaysAutoResize ) ) {
          ImGui::Text( "Layer Name: %s", item->name().c_str() );
          ImGui::Separator();

          glm::vec4 fill_color = item->get_polygon_fill_color();
          glm::vec4 line_color = item->get_line_color();
          float thickness = item->get_line_thickness();
          bool draw_triangles = item->should_draw_triangles();
          bool draw_outlines = item->should_draw_outlines();

          if ( ImGui::ColorEdit4( "Fill Color", &fill_color[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ) ) {
            item->set_polygon_fill_color( fill_color );
          }

          if ( ImGui::ColorEdit4( "Line Color", &line_color[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ) ) {
            item->set_line_color( line_color );
          }

          if ( ImGui::SliderFloat( "Line Thickness", &thickness, 1.0f, 10.0f ) ) {
            item->set_line_thickness( thickness );
          }

          if ( ImGui::Checkbox( "Draw Triangles", &draw_triangles ) ) {
            item->set_draw_triangles( draw_triangles );
          }

          if ( ImGui::Checkbox( "Draw Outlines", &draw_outlines ) ) {
            item->set_draw_outlines( draw_outlines );
          }

          ImGui::Separator();

          if ( ImGui::Button( "Close" ) ) {
            ImGui::CloseCurrentPopup();
          }

          ImGui::EndPopup();
        }

      };

      /* if the layer should be deleted this index is greater than -1 */
      int32_t index_to_delete = -1;

      for ( size_t n = 0; n < _map_layers->size(); n++ ) {
        Ref<MapLayer> item = (*_map_layers)[n];

        ImGui::PushID( n );

        bool hidden = !item->is_hidden();
        if ( ImGui::Checkbox( "", &hidden ) ) {
          item->set_hidden( !hidden );
        }

        ImGui::SameLine();

        bool selected = _selected_layer == item;
        if ( ImGui::Selectable( item->name().c_str(), &selected ) ) {

          /* clear selection and filter when selecting a new layer */
          if ( _selected_layer != nullptr ) {
            _selected_layer->clear_selected();

            for ( auto& filter : _attribute_filters )
              filter.second.Clear();
          }

          if ( _selected_layer != item )
            _selected_layer = item;
          else
            _selected_layer = nullptr;

          _refilter_attributes = true;
        }

        if ( ImGui::IsItemActive() ) {
          if ( ImGui::BeginDragDropSource( ImGuiDragDropFlags_None ) ) {
            uint32_t index = n;
            ImGui::SetDragDropPayload( "DND_LAYER_DRAG", &index, sizeof( index ) );
            ImGui::EndDragDropSource();
          }
        }

        if ( ImGui::BeginDragDropTarget() ) {
          if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "DND_LAYER_DRAG" ) ) {
            uint32_t index = *( (uint32_t*)payload->Data );
            if ( index >= 0 && index < _map_layers->size() ) {
              ( *_map_layers )[n] = ( *_map_layers )[index];
              ( *_map_layers )[index] = item;
            }
          }
          ImGui::EndDragDropTarget();
        }

        if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
          ImGui::OpenPopup( "Layer Properties" );

        if ( ImGui::IsItemHovered() && ImGui::IsMouseClicked( ImGuiMouseButton_Right ) )
          ImGui::OpenPopup( "Layer Options" );

        layer_props( item );

        if ( ImGui::BeginPopup( "Layer Options", ImGuiWindowFlags_AlwaysAutoResize ) ) {
          if ( ImGui::Button( "Delete" ) ) {
            index_to_delete = n;
            ImGui::CloseCurrentPopup();
          }
          if ( ImGui::Button( "Export Selection" ) ) {
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
        }

        ImGui::PopID();
      }

      if ( index_to_delete != -1 && index_to_delete < _map_layers->size() ) {
        _map_layers->erase( _map_layers->begin() + index_to_delete );
      }
    }
    ImGui::End();

  }

  void ApplicationMenu::draw_renderstate_menu( void ) {
    if ( ImGui::Begin( "Viewport Settings", &_show_renderstate_menu ) ) {
      if ( ImGui::CollapsingHeader( "Renderstate Perfomance" ) ) {
        Stats stat = RenderState::ref().stats();
        time_type dt = Timer::delta_time();

        ImGui::Text( "Performance Stats" );
        ImGui::Separator();

        static const uint32_t max_fps_records = 1024;
        static std::vector<uint32_t> fps( max_fps_records );

        std::rotate( fps.begin(), fps.begin() + 1, fps.end() );
        fps.back() = Timer::last_frame_fps();

        if ( ImPlot::BeginPlot( "FPS Graph", nullptr, nullptr, ImVec2( -1, 0 ), 0,  ImPlotAxisFlags_AutoFit ) ) {
          ImPlot::PlotLine<uint32_t>( "FPS Graph", fps.data(), fps.size(), 1.0f );
          ImPlot::EndPlot();
        }

        ImGui::Separator();
        ImGui::Text( "FPS: %f", Timer::fps() );
        ImGui::Text( "Delta Time: %f", dt );
        ImGui::Text( "Draw Calls: %u", stat.draw_calls );

        ImGui::Indent();

        ImGui::Text( "Lines: %u", stat.num_lines );
        ImGui::Text( "Triangles: %u", stat.num_triangles );
        ImGui::Text( "Points: %u", stat.num_points );

        ImGui::Unindent();

        bool vsync = GraphicsContext::ref().get_vsync();

        if ( ImGui::Checkbox( "VSync", &vsync ) ) {
          GraphicsContext::ref().set_vsync( vsync );
        }
        ImGui::NewLine();
      }

      if ( ImGui::CollapsingHeader( "Global Rendering Constants" ) ) {
        static glm::vec4 clear_color( 0.0f, 0.0f, 0.0f, 1.0f );

        if ( ImGui::ColorEdit4( "Background Color", &clear_color[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ) ) {
          RenderState::ref().set_clear_color( clear_color );
        }
        ImGui::NewLine();
        ImGui::Separator();
        ImGui::Text( "Selection Mode" );
        ImGui::ColorEdit4( "Fill Color", &_view_mode_polygon_highligh_color[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs );
        ImGui::ColorEdit4( "Line Color", &_view_mode_line_highlight_color[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs );
        ImGui::SliderFloat( "Line Thickness", &_viewmode_line_highlight_thickness, 1.0f, 10.0f );

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::Text( "Editor Mode" );
        
        if ( _editor_layer == nullptr ) {
          ImGui::End();
          return;
        }

        glm::vec4 point_color = _editor_layer->get_point_color();
        glm::vec4 point_highlight_color = _editor_layer->get_point_highlight_color();
        glm::vec4 line_color = _editor_layer->get_line_color();
        glm::vec4 line_highlight_color = _editor_layer->get_line_highlight_color();

        float point_size = _editor_layer->get_point_size();
        float line_thickness = _editor_layer->get_line_thickness();

        if ( ImGui::ColorEdit4( "Line Color", &line_color[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ) )
          _editor_layer->set_line_color( line_color );

        if ( ImGui::ColorEdit4( "Line Highlight Color", &line_highlight_color[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ) )
          _editor_layer->set_line_highlight_color( line_highlight_color );

        if ( ImGui::SliderFloat( "Line Thickness", &line_thickness, 1.0f, 30.0f ) )
          _editor_layer->set_line_thickness( line_thickness );
        ImGui::Separator();
        if ( ImGui::ColorEdit4( "Point Color", &point_color[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ) )
          _editor_layer->set_point_color( point_color );

        if ( ImGui::ColorEdit4( "Point Highlight Color", &point_highlight_color[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs ) )
          _editor_layer->set_point_highlight_color( point_highlight_color );

        if ( ImGui::SliderFloat( "Point Size", &point_size, 1.0f, 30.0f ) )
          _editor_layer->set_point_size( point_size );

        ImGui::NewLine();
      }

      if ( ImGui::CollapsingHeader( "Visual Interface" ) ) {
        static uint32_t style = 1;
        ImGui::Separator();
        ImGui::Text( "ImGui Styles" );

        if ( ImGui::RadioButton( "Dark Blue", style == 0 ) && style != 0 ) {
          set_imgui_style_dark();
          style = 0;
        }
        if ( ImGui::RadioButton( "Dark Shadow", style == 2 ) && style != 2 ) {
          set_imgui_style_dark_shadow();
          style = 2;
        }

        if ( ImGui::RadioButton( "Light", style == 1 ) && style != 1 ) {
          set_imgui_style_light();
          style = 1;
        }

        if ( ImGui::RadioButton( "Silver", style == 3 ) && style != 3 ) {
          set_imgui_style_silver();
          style = 3;
        }
      }
    }
    ImGui::End();
  }

  void ApplicationMenu::draw_attributes( Ref<MapLayer> selected_layer ) {
    
    if ( selected_layer ) {
      /* lambda to get filter based on attribute name */
      static auto push_filter = [&] ( string attribute_name ) -> void
      {
        auto it = _attribute_filters.find( attribute_name );
        if ( it == _attribute_filters.end() )
          _attribute_filters.insert( std::make_pair( attribute_name, ImGuiTextFilter() ) );
      };

      /* insert all the filters for the layer if not inserted */
      for ( const auto& prop : ( *selected_layer->begin() )->get_props() ) {
        push_filter( prop.first );
      }
    }

    if ( _show_filter_menu )
      draw_filter_menu( selected_layer );

    if ( _show_attributes_menu )
      draw_attibutes_menu( selected_layer );

  }

  void ApplicationMenu::draw_attibutes_menu( Ref<MapLayer> selected_layer ) {
    if ( ImGui::Begin( "Attribute List", &_show_attributes_menu ) ) {

      if ( selected_layer == nullptr || selected_layer->empty() ) {
        ImGui::End();
        return;
      }

      ImGui::Button( "Edit Attributes" );
      ImGui::SameLine();
      ImGui::Button( "Save Changes" );
      ImGui::SameLine();
      if ( ImGui::Button( "Filter Data" ) ) {
        _show_filter_menu = !_show_filter_menu;
      }

      /* table style */
      static ImGuiTableFlags table_flags =
          ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti |
          ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY |
          ImGuiTableFlags_ScrollX | ImGuiTableFlags_SizingFixedFit;// | ImGuiTableFlags_Resizable;

      //freeze cols and rows
      static int freeze_cols = 0;
      static int freeze_rows = 1;

      /* number of columns to be drawn */
      uint32_t columns = ( *selected_layer->begin() )->get_props().size();

      if ( ImGui::BeginTable( " TableAttList", columns, table_flags, ImVec2( 0.0f, 0.5f ) ) ) {
        
        //freeze cols and rows
        ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);

        /* draw the columns */
        Ref<Geometry> first = *selected_layer->begin();
        for ( const auto& prop : first->get_props() )
          ImGui::TableSetupColumn( prop.first.c_str(), ImGuiTableColumnFlags_WidthFixed );

        /* create the header row */
        ImGui::TableHeadersRow();

        /* ensure that the filtering is done */
        _filter_future.wait();

        /* draw the attributes */
        ImGuiListClipper clipper;
        clipper.Begin( _filtered_polygons.size() );

        while ( clipper.Step() ) {
          for ( int32_t row = clipper.DisplayStart; row < clipper.DisplayEnd; row++ ) {
            ImGui::TableNextRow();
            uint32_t column = 0;
            bool selected = selected_layer->is_selected( _filtered_polygons[row] );
            ImGui::PushID( row );

            for ( const auto& prop : selected_layer->get_geometry( _filtered_polygons[row] )->get_props() ) {
              ImGui::TableSetColumnIndex( column );

              /* create the row selectable */
              if ( column == 0 ) {
                if ( ImGui::Selectable( prop.second.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns ) ) {
                  if ( !ImGui::GetIO().KeyCtrl )
                    selected_layer->clear_selected();

                  if ( !selected )
                    selected_layer->select( _filtered_polygons[row] );
                  else
                    selected_layer->deselect( _filtered_polygons[row] );
                }

                if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) ) {
                  /* get the bounding box of the clicked polygon */
                  auto fly_to_geom = selected_layer->get_geometry( _filtered_polygons[row] );
                  Box bb = fly_to_geom->bbox();

                  glm::vec3 center = bb.center();
                  glm::vec3 size   = bb.size();

                  glm::vec2 camera_size = _camera->get_size();

                  center.z = size.x > size.y ? size.x / camera_size.x : size.y / camera_size.y;

                  _camera->set_target( center );
                }

              } else {
                ImGui::Text( prop.second.c_str() );
              }
              column++;
            }

            ImGui::PopID();
          }
        }

        ImGui::EndTable();
      }

    }
    ImGui::End();
  }

  void ApplicationMenu::draw_filter_menu( Ref<MapLayer> selected_layer ) {
    static bool filter_selected = false;

    if ( ImGui::Begin( "Filter by Attributes", &_show_filter_menu ) ) {

      if ( selected_layer == nullptr || selected_layer->empty() ) {
        ImGui::End();
        return;
      }

      ImGui::NewLine();

      if ( ImGui::Checkbox( "Filter Selected", &filter_selected ) ) {
        _refilter_attributes = true;
      }

      ImGui::Separator();

      /* draw the filters */
      for ( const auto& prop : ( *selected_layer->begin() )->get_props() ) {
        if ( _attribute_filters[prop.first].Draw( prop.first.c_str() ) ) {
          _refilter_attributes = true;
        }
      }

    }
    ImGui::End();

    static auto filter_async = [&] ( std::vector<polygon_id>* ids, Ref<MapLayer> selected_layer, const bool filter_selected_only ) -> void {

      if ( filter_selected_only && selected_layer->selected_geometries().size() > 0 ) {
        for ( const auto& geometry : selected_layer->selected_geometries() ) {
          bool filter_pass = true;

          for ( const auto& prop : selected_layer->get_geometry( geometry )->get_props() ) {
            if ( !_attribute_filters[prop.first].PassFilter( prop.second.c_str() ) )
              filter_pass = false;
          }

          if ( filter_pass )
            ids->push_back( geometry );
        }
      } else {
        for ( const auto& geometry : *selected_layer ) {
          bool filter_pass = true;

          for ( const auto& prop : geometry->get_props() ) {
            if ( !_attribute_filters[prop.first].PassFilter( prop.second.c_str() ) )
              filter_pass = false;
          }

          if ( filter_pass )
            ids->push_back( geometry->id() );
        }
      }
      
    };

    if ( selected_layer == nullptr ) return;
    
    if ( _refilter_attributes ) {
      _filtered_polygons.clear();
      _filter_future = std::async( std::launch::async, filter_async, &_filtered_polygons, selected_layer, filter_selected );

      _refilter_attributes = false;
    }
  }
}
