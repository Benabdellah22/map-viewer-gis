#include <graphics/imguicontext.h>

#include <core/window.h>
#include <utils/logger.h>
#include <utils/exception.h>
#include <graphics/context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_glfw.cpp>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3.cpp>

#include <misc/cpp/imgui_stdlib.cpp>

#include <implot.h>

namespace mv {

  ImGuiContext::ImGuiContext( void ) {
    /* get the glfw window handle to initialize imgui */
    GLFWwindow* window = (GLFWwindow*)Window::ref().window_handle();

    /* create imgui context */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    /* create implot context */
    ImPlot::CreateContext();

    /* enable docking */
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    /* enable dark theme */
    ImGui::StyleColorsDark();

    /* initialize imgui glfw and OpenGL backend */
    if ( !ImGui_ImplGlfw_InitForOpenGL( window, true ) )
      THROW( "failed to initialize imgui glfw" );

    constexpr char glsl_version[] = "#version 460 core";
    if ( !ImGui_ImplOpenGL3_Init( glsl_version ) )
      THROW( "failed to initialize imgui opengl" );

    LOG_INFO( "created imgui context" );
  }

  ImGuiContext::~ImGuiContext( void ) {
    /* shutdown backends */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    /* destroy the context */
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    LOG_INFO( "deleted imgui context" );
  }

  void ImGuiContext::begin_frame( void ) {
    /* backend new frame */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    /* imgui new frame */
    ImGui::NewFrame();
  }

  void ImGuiContext::end_frame( void ) {

  #ifdef DEBUG
    /* we don't want debug messages from ImGui so we disable OpenGL debug output */
    glDisable( GL_DEBUG_OUTPUT );
  #endif

    /* render imgui data */
    ImGui::Render();

    /* render using backend */
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

  #ifdef DEBUG
    /* enable OpenGL debug output */
    glEnable( GL_DEBUG_OUTPUT );
  #endif

  }

  void ImGuiContext::on_event( Event& e ) {
    if ( !_block_events ) return;

    EventDispatcher dispatcher( e );

    static auto consume_event = [] ( Event& e ) -> bool {
      return true;
    };

    ImGuiIO& io = ImGui::GetIO();

    if ( e.category() & EVENT_CATEGORY_KEYBOARD &&
         io.WantCaptureKeyboard &&
         ( e.type() != EventType::KeyReleased ) )
      dispatcher.dispatch<Event>( consume_event );

    if ( e.category() & EVENT_CATEGORY_MOUSE &&
         io.WantCaptureMouse &&
         ( e.type() != EventType::MouseReleased ) )
      dispatcher.dispatch<Event>( consume_event );
  }

}
