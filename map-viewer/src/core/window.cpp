#include <core/window.h>
#include <utils/logger.h>
#include <utils/exception.h>

#include <GLFW/glfw3.h>

#include <base/keyboard-events.h>
#include <base/window-events.h>
#include <base/mouse-events.h>

namespace mv {

  /* GLFW uses function to emit event whenever they occur, we simply */
  /* capture those events and dispatch them in our application.      */
  static void window_close_callback ( ::GLFWwindow* window                                              );
  static void window_move_callback  ( ::GLFWwindow* window, int x, int y                                );
  static void window_size_callback  ( ::GLFWwindow* window, int x, int y                                );
  static void key_callback          ( ::GLFWwindow* window, int key, int scancode, int action, int mods );
  static void char_callback         ( ::GLFWwindow* window, unsigned int charcode                       );
  static void cursor_pos_callback   ( ::GLFWwindow* window, double x, double y                          );
  static void mouse_button_callback ( ::GLFWwindow* window, int button, int action, int mods            );
  static void mouse_scroll_callback ( ::GLFWwindow* window, double xoffset, double yoffset              );


  static void glfw_error_callback( int error, const char* desc ) {
    LOG_ERROR( "glfw error - {}", desc );
  }

  mv::Window::Window( uint32_t width, uint32_t height, const std::string& name ) {
    LOG_INFO( "Creating window with size: [{}, {}] and name: '{}'", width, height, name );

    /* set error callback */
    glfwSetErrorCallback( glfw_error_callback );

    /* initialize glfw */
    if ( glfwInit() != GLFW_TRUE )
      THROW( "failed to initialize glfw" );

    /* set opengl context version */
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    /* set multisample count */
    glfwWindowHint( GLFW_SAMPLES, 4 );

    /* create window */
    GLFWwindow* window = glfwCreateWindow( (int)width, (int)height, name.c_str(), nullptr, nullptr );

    /* throw exception if failed to create window */
    if ( !window )
      THROW( "failed to create glfw window" );

    /* set callback functions */

    /* assign user pointer to GLFW window */
    glfwSetWindowUserPointer  ( window, this                  );

    /* window */
    glfwSetWindowCloseCallback( window, window_close_callback );
    glfwSetWindowPosCallback  ( window, window_move_callback  );
    glfwSetWindowSizeCallback ( window, window_size_callback  );

    /* mouse */
    glfwSetCursorPosCallback  ( window, cursor_pos_callback   );
    glfwSetMouseButtonCallback( window, mouse_button_callback );
    glfwSetScrollCallback     ( window, mouse_scroll_callback );

    /* keyboard */
    glfwSetKeyCallback        ( window, key_callback          );
    glfwSetCharCallback       ( window, char_callback         );

    /* assign variables */
    _native_window_handle = (uintptr_t)window;
    _width = width;
    _height = height;

    LOG_INFO( "glfw window created successfully" );
  }

  mv::Window::~Window( void ) {
    GLFWwindow* window = (GLFWwindow*)_native_window_handle;
    glfwDestroyWindow( window );
    glfwTerminate();
    LOG_INFO( "glfw window destroyed successfully and glfw terminated" );
  }

  void mv::Window::poll_event( void ) {
    /* poll window events and dispatch them */
    glfwPollEvents();
  }

  void Window::bind_event_func( event_emit_func func ) {
    if ( func )
      _event_func = func;
    else
      LOG_ERROR( "binding invalid event function" );
  }

  /* glfw callback functions */

  event_emit_func get_emit_func( ::GLFWwindow* window ) {
    Window* user_pointer = (Window*)glfwGetWindowUserPointer( window );

    if ( !user_pointer ) {
      LOG_ERROR( "invalid user pointer; set the user pointer properly to get event callbacks" );
      return nullptr;
    }

    event_emit_func func = user_pointer->get_event_function();

    if ( !func ) {
      LOG_WARN( "the window has events but no callback function has been provided" );
      return nullptr;
    }

    return func;
  }

  /* glfw window callbacks */

  void window_close_callback( ::GLFWwindow* window ) {
    event_emit_func func = get_emit_func( window );

    if ( !func )
      return;

    WindowCloseEvent e( true );
    func( e );
  }

  void window_move_callback( ::GLFWwindow* window, int x, int y ) {
    event_emit_func func = get_emit_func( window );

    if ( !func )
      return;

    WindowMoveEvent e( (uint32_t)x, (uint32_t)y );
    func( e );
  }

  void window_size_callback( ::GLFWwindow* window, int x, int y ) {
    event_emit_func func = get_emit_func( window );
    Window* user_pointer = (Window*)glfwGetWindowUserPointer( window );

    if ( !func )
      return;

    user_pointer->_width  = (uint32_t)x;
    user_pointer->_height = (uint32_t)y;

    WindowResizeEvent e( (uint32_t)x, (uint32_t)y );
    func( e );
  }

  /* glfw key callbacks */

  void key_callback( ::GLFWwindow* window, int key, int scancode, int action, int mods ) {
    event_emit_func func = get_emit_func( window );

    if ( !func )
      return;

    switch ( action ) {
    case GLFW_PRESS:
    {
      KeyPressedEvent e( key );
      func( e );
      break;
    }
    case GLFW_REPEAT:
    {
      KeyRepeatEvent e( key );
      func( e );
      break;
    }
    case GLFW_RELEASE:
    {
      KeyReleasedEvent e( key );
      func( e );
      break;
    }
    default:
      LOG_WARN( "invlid glfw key action - {}", action );
      break;
    }
  }

  void char_callback( ::GLFWwindow* window, unsigned int charcode ) {
    event_emit_func func = get_emit_func( window );

    if ( !func )
      return;

    KeyTypedEvent e( charcode );
    func( e );
  }

  /* glfw mouse callbacks */

  static void cursor_pos_callback( ::GLFWwindow* window, double x, double y ) {
    event_emit_func func = get_emit_func( window );

    if ( !func )
      return;

    MouseMovedEvent e( x, y );
    func( e );
  }

  static void mouse_button_callback( ::GLFWwindow* window, int button, int action, int mods ) {
    event_emit_func func = get_emit_func( window );

    if ( !func )
      return;

    switch ( action )
    {
    case GLFW_PRESS:
    {
      MousePressedEvent e( button );
      func( e );
      break;
    }
    case GLFW_RELEASE:
    {
      MouseReleasedEvent e( button );
      func( e );
      break;
    }
    default:
      LOG_WARN( "invlid glfw mouse action - {}", action );
      break;
    }
  }

  static void mouse_scroll_callback( ::GLFWwindow* window, double xoffset, double yoffset ) {
    event_emit_func func = get_emit_func( window );

    if ( !func )
      return;

    MouseScrolledEvent e( xoffset, yoffset );
    func( e );
  }

}
