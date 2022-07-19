
#include <filesystem>
#include <algorithm>
#include <stack>

#include <types.h>

#include <imgui.h>
#include <tools/fontawesome.h>
#include <tools/icons.h>

namespace mv {

  class ContentBrowser {
  public:
    ContentBrowser( const string& root_dir = "" ) {
      _current_directory = root_dir;

      if ( !std::filesystem::exists( _current_directory ) )
        _current_directory = "";

      _current_directory = std::filesystem::canonical( _current_directory );
    }
  public:
    
    void load_fonts( void ) {
      /* load font for icons */
      ImGuiIO& io = ImGui::GetIO();
      
      static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
      ImFontConfig icons_config;

      ImFontConfig CustomFont;
      CustomFont.FontDataOwnedByAtlas = false;

      icons_config.MergeMode = false;
      icons_config.PixelSnapH = true;
      icons_config.OversampleH = 2.5;
      icons_config.OversampleV = 2.5;

      _font_awesome_large = io.Fonts->AddFontFromMemoryCompressedTTF( font_awesome_data, font_awesome_size, 40.0f, &icons_config, icons_ranges );
      _font_awesome_small = io.Fonts->AddFontFromMemoryCompressedTTF( font_awesome_data, font_awesome_size, 16.0f, &icons_config, icons_ranges );
    }

    void draw( void ) {

      if ( ImGui::Begin( "Content Browser" ) ) {

        explorer_pane();

      }
      ImGui::End();

    }

    void explorer_pane( void ) {

      if ( ImGui::BeginChild( "browser pane", ImVec2( 0, 0 ), true ) ) {

        /* navigation buttons */
        {
          ImGui::PushFont( _font_awesome_small );

          if ( ImGui::Button( ICON_FA_ARROW_LEFT ) ) {
            backtrack();
          }

          ImGui::SameLine();

          if ( ImGui::Button( ICON_FA_ARROW_RIGHT ) ) {
            fronttrack();
          }

          ImGui::SameLine();

          if ( ImGui::Button( ICON_FA_ARROW_UP ) ) {
            auto parent_path = _current_directory.parent_path();
            if ( !parent_path.empty() )
              change_path( parent_path );
          }

          ImGui::PopFont();
        }

        // ImGui::Text( _current_directory.u8string().c_str() );

        ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
        ImGui::PushStyleColor( ImGuiCol_Border, ImVec4( 0, 0, 0, 0 ) );

        auto root_dir = _current_directory.root_directory();
        auto root_name = _current_directory.root_name();
        std::filesystem::path recursive_path = root_dir;

        for ( auto path : _current_directory ) {
          if ( path.empty() ) continue;
          if ( path == root_dir ) continue;
          if ( path != root_name )
            recursive_path /= path;

          ImGui::SameLine( 0.0f, 0.0f );

          if ( ImGui::Button( path.u8string().c_str() ) ) {
            change_path( recursive_path );
            break;
          }

          ImGui::SameLine( 0.0f, 0.0f );
          ImGui::Text( "/" );
        }

        ImGui::PopStyleColor( 2 );

        ImGui::Separator();

        std::filesystem::directory_iterator iter( _current_directory );

        static float thumbnail_size = 64.0f;
        static float min_padding = 10.0f;

        ImVec2 available_region = ImGui::GetContentRegionAvail();

        int32_t num_columns = available_region.x / ( thumbnail_size + min_padding );

        num_columns = std::clamp( num_columns, 1, 64 );

        if ( ImGui::BeginTable( "content browser panel", num_columns, ImGuiTableFlags_ScrollY ) ) {
          for ( const auto& item : iter ) {
            auto& item_path = item.path();

            ImGui::PushID( item_path.u8string().c_str() );

            ImGui::TableNextColumn();

            // ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
            // ImGui::PushStyleColor( ImGuiCol_Border, ImVec4( 0, 0, 0, 0 ) );

            const char* icon = nullptr;

            if ( item.is_directory() )
              icon = ICON_FA_FOLDER;
            else {
              if ( item.path().extension() == ".geojson" )
                icon = ICON_FA_FILE_CODE;
              else
                icon = ICON_FA_FILE_ALT;
            }

            ImGui::PushFont( _font_awesome_large );

            if ( ImGui::Button( icon, ImVec2( thumbnail_size, thumbnail_size ) ) ) {
              if ( item.is_directory() )
                change_path( item_path );
            }
            
            ImGui::PopFont();

            if ( ImGui::IsItemHovered() && !item.is_directory() ) {
              if ( ImGui::BeginDragDropSource( ImGuiDragDropFlags_None ) ) {
                string filename = item_path.u8string();
                ImGui::SetDragDropPayload( "DND_FILE_CB", filename.data(), filename.size() + 1 );
                ImGui::Text( filename.c_str() );
                ImGui::EndDragDropSource();
              }
            }

            // ImGui::PopStyleColor( 1 );

            ImVec2 button_size = ImGui::GetItemRectSize();

            ImGui::PushTextWrapPos( ImGui::GetCursorPosX() + button_size.x );
            ImVec2 text_size = ImGui::CalcTextSize( item_path.filename().u8string().c_str() );

            if ( text_size.x < button_size.x )
              ImGui::SetCursorPosX( ImGui::GetCursorPosX() + ( button_size.x - text_size.x ) * 0.5f );

            ImGui::Text( item_path.filename().u8string().c_str() );
            ImGui::PopTextWrapPos();

            ImGui::PopID();
          }

          ImGui::EndTable();
        }
      }

      ImGui::EndChild();

    }
  private:
    void change_path( std::filesystem::path path, bool clear_front = true ) {
      _back.push( _current_directory );

      _current_directory = path;
      _current_directory = std::filesystem::absolute( _current_directory );

      if ( clear_front )
        _front = std::stack<std::filesystem::path>();
    }

    void backtrack( void ) {
      if ( _back.empty() ) return;

      _front.push( _current_directory );

      change_path( _back.top(), false );
      _back.pop();
      _back.pop();
    }

    void fronttrack( void ) {
      if ( _front.empty() ) return;

      change_path( _front.top(), false );
      _front.pop();
    }
  private:
    std::filesystem::path _current_directory;
    ImFont* _font_awesome_small = nullptr;
    ImFont* _font_awesome_large = nullptr;

    std::stack<std::filesystem::path> _back;
    std::stack<std::filesystem::path> _front;
  };

}

