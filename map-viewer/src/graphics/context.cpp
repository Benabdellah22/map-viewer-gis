#include <graphics/context.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace mv {

  static void GLAPIENTRY opengl_callback(
    GLenum        source,
    GLenum        type,
    GLuint        id,
    GLenum        severity,
    GLsizei       length,
    const GLchar* message,
    const void* user_param ) {

    const char* source_string = nullptr;
    const char* type_string = nullptr;

#define GL_CASE_ENUM_TO_STRING( E, V ) case E: V = #E; break

    switch ( source ) {
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_SOURCE_API, source_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_SOURCE_WINDOW_SYSTEM, source_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_SOURCE_SHADER_COMPILER, source_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_SOURCE_THIRD_PARTY, source_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_SOURCE_APPLICATION, source_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_SOURCE_OTHER, source_string );
    default:
      LOG_WARN( "opengl - invalid source in debug callback {}", source );
      break;
    }

    switch ( type ) {
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_TYPE_ERROR, type_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, type_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, type_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_TYPE_PORTABILITY, type_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_TYPE_PERFORMANCE, type_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_TYPE_MARKER, type_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_TYPE_PUSH_GROUP, type_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_TYPE_POP_GROUP, type_string );
      GL_CASE_ENUM_TO_STRING( GL_DEBUG_TYPE_OTHER, type_string );
    default:
      LOG_WARN( "opengl - invalid type in debug callback {}", type );
      break;
    }

    #define GL_LOG_ERROR( F ) F( "opengl - error callback [source = {}, type = {}, id = {}]: {}", source_string, type_string, id, message );

    switch ( severity ) {
    case GL_DEBUG_SEVERITY_HIGH:         GL_LOG_ERROR( LOG_CRITICAL ); break;
    case GL_DEBUG_SEVERITY_MEDIUM:       GL_LOG_ERROR( LOG_ERROR    ); break;
    case GL_DEBUG_SEVERITY_LOW:          GL_LOG_ERROR( LOG_WARN     ); break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: break; // GL_LOG_ERROR( LOG_INFO     ); break;
    default:                             GL_LOG_ERROR( LOG_INFO     ); break;
    }

  }

  GraphicsContext::GraphicsContext( uintptr_t native_window_handle ) {
    LOG_INFO( "creating opengl context" );

    GLFWwindow* window = (GLFWwindow*)native_window_handle;
    MV_ASSERT( window != nullptr );

    /* create opengl context using the convenient glfw function */
    glfwMakeContextCurrent( window );

    /* load opengl function addresses, alternative to this is */
    /* to load function by first loading `opengl32.dll` and   */
    /* then acquire addressed using GetProcAddress. Since it  */
    /* is a tedious process, we use GLAD to do that.          */
    if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
      THROW( "failed to accquire opengl function addresses | failed to initialize glad" );

  #ifdef DEBUG
    /* enable debug output */
    glEnable( GL_DEBUG_OUTPUT );
    glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
    glDebugMessageCallback( opengl_callback, nullptr );
  #endif

    /* enable VSync, to disable simply replace with */
    /* glfwSwapInterval( 0 ); */
    glfwSwapInterval( 1 );

    /* enable msaa */
    glEnable( GL_MULTISAMPLE );

    /* enable blending */
    glEnable( GL_BLEND );

    /* set the blend equation */
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    /* set clear color */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* log some useful information */
    LOG_INFO ( "opengl context created successfully"      );
    LOG_DEBUG( "Version : {}", glGetString( GL_VERSION  ) );
    LOG_DEBUG( "Renderer: {}", glGetString( GL_RENDERER ) );
    LOG_DEBUG( "Vendor  : {}", glGetString( GL_VENDOR   ) );

    /* assign variables */
    _native_window_handle = native_window_handle;

    /* TEMP */
    glPointSize( 10.0f );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  }

  GraphicsContext::~GraphicsContext( void ) {
    LOG_INFO( "opengl context destroyed" );
  }

  void GraphicsContext::swap_buffers( void ) {
    PROFILE_FUNCTION();
    GLFWwindow* window = (GLFWwindow*)_native_window_handle;
    glfwSwapBuffers( window );
  }

  void GraphicsContext::clear_buffer( void ) {
    PROFILE_FUNCTION();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
  }

  void GraphicsContext::resize_buffer( uint32_t width, uint32_t height ) {
    MV_ASSERT( width != 0 && height != 0 );
    glViewport( 0, 0, width, height );
    LOG_DEBUG( "default framebuffer size updated - [{}, {}]", width, height );
  }

  void GraphicsContext::set_vsync( bool enable ) {
    if ( enable )
      glfwSwapInterval( 1 );
    else
      glfwSwapInterval( 0 );

    _vsync = enable;
  }

  bool GraphicsContext::get_vsync( void ) const {
    return _vsync;
  }

}
