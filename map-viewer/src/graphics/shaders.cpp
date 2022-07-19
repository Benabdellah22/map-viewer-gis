#include <graphics/shaders.h>

#include <algorithm>

#include <glad/glad.h>

#include <utils/exception.h>
#include <utils/logger.h>
#include <utils/assert.h>

#include "typemap.h"

namespace mv {

  /* check shader compilation and link errors */
  static bool get_shader_status( GLuint id, GLenum type ) {
    MV_ASSERT( type == GL_COMPILE_STATUS || type == GL_LINK_STATUS );

    GLint    success;
    GLchar   info_log[1024];

    if ( type == GL_COMPILE_STATUS ) {
      glGetShaderiv( id, GL_COMPILE_STATUS, &success );
      if ( !success ) {
        glGetShaderInfoLog( id, sizeof( info_log ), NULL, info_log );
        LOG_ERROR( "{} shader compilation error: {}", type == GL_VERTEX_SHADER ? "vertex" : "fragment", info_log);
        THROW( "failed to compile OpenGL shader, see log for errors" );
      }
    } else {
      glGetProgramiv( id, GL_LINK_STATUS, &success );
      if ( !success ) {
        glGetProgramInfoLog( id, sizeof( info_log ), NULL, info_log );
        LOG_ERROR( "shader program link error: {}", info_log );
        THROW( "failed to link OpenGL shader, see log for errors" );
      }
    }

    return success == 0 ? false : true;
  }

  Shader::Shader( const string& vertex_shader_code, const string& fragment_shader_code ) {

    /* create vertex shader object */
    GLuint vertex_shader_id = glCreateShader( GL_VERTEX_SHADER );
    if ( vertex_shader_id == 0 )
      THROW( "vertex_shader_id to create OpenGL vertex shader object" );

    const char* vertex_shader_code_raw  = vertex_shader_code.c_str();
    GLsizei     vertex_shader_code_size = vertex_shader_code.size();

    /* provide shader code to compile vertex shader and check for errors */
    glShaderSource( vertex_shader_id, 1, &vertex_shader_code_raw, &vertex_shader_code_size );
    glCompileShader( vertex_shader_id );
    if ( !get_shader_status( vertex_shader_id, GL_COMPILE_STATUS ) )
      return;

    /* create fragment shader object */
    GLuint fragment_shader_id = glCreateShader( GL_FRAGMENT_SHADER );
    if ( fragment_shader_id == 0 )
      THROW( "failed to create OpenGL fragment shader object" );

    const char* fragment_shader_code_raw = fragment_shader_code.c_str();
    GLsizei     fragment_shader_code_size = fragment_shader_code.size();

    /* provide shader code to compile fragment shader and check for errors */
    glShaderSource( fragment_shader_id, 1, &fragment_shader_code_raw, &fragment_shader_code_size );
    glCompileShader( fragment_shader_id );
    if ( !get_shader_status( fragment_shader_id, GL_COMPILE_STATUS ) )
      return;

    /* create shader program object */
    _shader_program_id = glCreateProgram();
    if ( _shader_program_id == 0 )
      THROW( "failed to create OpenGL shader program object" );

    /* attach vertex and fragment shaders */
    glAttachShader( _shader_program_id, vertex_shader_id );
    glAttachShader( _shader_program_id, fragment_shader_id );
    glLinkProgram( _shader_program_id );

    /* check for link errors */
    if ( !get_shader_status( _shader_program_id, GL_LINK_STATUS ) )
      return;

    /* detach shaders and delete them */
    glDetachShader( _shader_program_id, vertex_shader_id );
    glDetachShader( _shader_program_id, fragment_shader_id );
    glDeleteShader( vertex_shader_id );
    glDeleteShader( fragment_shader_id );

    LOG_INFO( "created OpenGL shader program: id = {}", _shader_program_id );
  }

  Shader::~Shader( void ) {
    /* delete input layout if created */
    delete_input_layout();

    /* delete the shader program */
    LOG_INFO( "deleted OpenGL shader program: id = {}", _shader_program_id );
    glDeleteProgram( _shader_program_id );
  }

  void Shader::use( void ) {
    PROFILE_FUNCTION();

    /* bind the vertex array if created */
    bind_input_layout();

    /* bind the shader program to the pipeline */
    glUseProgram( _shader_program_id );
  }

  void Shader::unuse( void ) {
    /* unbind the vertex array if bound to the pipeline */
    unbind_input_layout();

    /* remove bound shader drom the pipeline */
    glUseProgram( 0 );
  }

  void Shader::set_mat4( const string& uniform_name, const glm::mat4& matrix ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniformMatrix4fv( location, 1, GL_FALSE, &matrix[0][0] );
  }

  void Shader::set_vec2( const string& uniform_name, const glm::vec2& vector ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniform2fv( location, 1, &vector[0] );
  }

  void Shader::set_vec3( const string& uniform_name, const glm::vec3& vector ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniform3fv( location, 1, &vector[0] );
  }

  void Shader::set_vec4( const string& uniform_name, const glm::vec4& vector ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniform4fv( location, 1, &vector[0] );
  }
  
  void Shader::set_ivec2( const string& uniform_name, const glm::ivec2& vector ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniform2iv( location, 1, &vector[0] );
  }

  void Shader::set_ivec3( const string& uniform_name, const glm::ivec3& vector ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniform3iv( location, 1, &vector[0] );
  }

  void Shader::set_ivec4( const string& uniform_name, const glm::ivec4& vector ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniform4iv( location, 1, &vector[0] );
  }

  void Shader::set_uint( const string& uniform_name, const uint32_t& value ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniform1ui( location, value );
  }

  void Shader::set_int( const string& uniform_name, const int32_t& value ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniform1i( location, value );
  }

  void Shader::set_int_array( const string& uniform_name, size_t size, const int32_t* value ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniform1iv( location, size, value );
  }

  void Shader::set_uint_array( const string& uniform_name, size_t size, const uint32_t* value ) const {
    PROFILE_FUNCTION();

    /* get uniform location */
    GLint location = get_uniform_location( uniform_name );

    /* check valid location */
    if ( location < 0 ) {
      LOG_ERROR( "no uniform with name `{}` in use of the shader", uniform_name );
      return;
    }

    /* set the uniform */
    glUniform1uiv( location, size, value );
  }

  void Shader::create_input_layout( const std::vector<Shader::InputLayoutDesc>& ild ) {
    PROFILE_FUNCTION();

    /* delete previous input layout if any */
    delete_input_layout();

    /* create the OpenGL vertex attribute object */
    glCreateVertexArrays( 1, &_vertex_attribute );
    if ( _vertex_attribute == 0 )
      THROW( "failed to create OpenGL vertex attribute object" );

    /* bind the vertex attribute object to capture state changes */
    bind_input_layout();

    for ( const auto& layout : ild ) {

      /* get the attribute location for the attribute name */
      GLint attrib_index = glGetAttribLocation( _shader_program_id, layout.attribute_name.c_str() );

      /* return if the location is invalid */
      if ( attrib_index == -1 ) {
        LOG_ERROR( "attribute name `{}` is provided a buffer but the shader does not accept any attribute with this name", layout.attribute_name.c_str() );
        continue;
      }

      /* get OpenGL data type */
      GLenum data_format = map( layout.data_type );

      /* number of elements in the data type */
      uint32_t num_elements = get_num_elements( layout.data_type );

      /* bind the vertex buffer */
      bind_vertex_buffer( layout.input_buffer_id );

      /* call the vertex attrib pointer to point to the data */
      if ( data_format == GL_FLOAT )
        glVertexAttribPointer( attrib_index, (GLint)num_elements, data_format, GL_FALSE, (GLsizei)layout.stride, (const void*)( (uintptr_t)layout.offset ) );
      else if ( data_format == GL_INT || data_format == GL_UNSIGNED_INT )
        glVertexAttribIPointer( attrib_index, (GLint)num_elements, data_format, (GLsizei)layout.stride, (const void*)( (uintptr_t)layout.offset ) );
      else
        LOG_ERROR( "invalid internal data type, please contact the developer :-)" );

      /* enable the location */
      glEnableVertexAttribArray( attrib_index );

      /* unbind the buffer */
      bind_vertex_buffer( 0 );
    }

    /* unbind the vertex array */
    unbind_input_layout();

  #if defined( DEBUG ) && 0
    LOG_INFO( "successfully setup shader input data" );

    LOG_DEBUG( "-------------- more details --------------" );

    for ( const auto& layout : ild ) {
      GLint attrib_index = glGetAttribLocation( _shader_program_id, layout.attribute_name.c_str() );
      LOG_DEBUG( "name: {}, locaton: {}, buffer id: {}, stride: {}, offset: {}",
        layout.attribute_name, attrib_index, layout.input_buffer_id, layout.stride, layout.offset );
    }

    LOG_DEBUG( "------------------------------------------" );
  #endif
  }

  void Shader::delete_input_layout( void ) {
    if ( _vertex_attribute != 0 )
      glDeleteVertexArrays( 1, &_vertex_attribute );
  }

  void Shader::bind_input_layout( void ) const {
    PROFILE_FUNCTION();
    glBindVertexArray( _vertex_attribute );
  }

  void Shader::unbind_input_layout( void ) const {
    PROFILE_FUNCTION();
    glBindVertexArray( 0 );
  }

  void Shader::bind_shader_storage_internal( const string& name, uint32_t buffer_id ) const {
    PROFILE_FUNCTION();

    /* check if the binding point is already set */
    auto it = _shader_storage_binding.find( name );

    /* binding point not set, set it */
    if ( it == _shader_storage_binding.end() ) {
      size_t block_index = _shader_storage_binding.size();

      /* the the binding index */
      GLuint binding_index = glGetProgramResourceIndex( _shader_program_id, GL_SHADER_STORAGE_BLOCK, name.c_str() );

      if ( binding_index == GL_INVALID_INDEX ) {
        LOG_WARN( "cannot find shader storage buffer with name `{}` in the shader", name );
        return;
      }

      /* the the block index */
      glShaderStorageBlockBinding( _shader_program_id, binding_index, block_index );

      /* store the block index so that we don't have to bind it again */
      _shader_storage_binding.insert( std::make_pair( name, block_index ) );

      /* point to the new block index */
      it = _shader_storage_binding.find( name );
    }

    /* bind the buffer to the pipeline */
    glBindBufferBase( GL_SHADER_STORAGE_BUFFER, it->second, buffer_id );
  }

  int32_t Shader::get_uniform_location( const string& name ) const {
    PROFILE_FUNCTION();

    /* check if the unifrom location is stored in the cache */
    auto it = _uniform_locations.find( name );

    /* get uniform location if not present in the cache */
    if ( it == _uniform_locations.end() ) {

      /* get uniform location */
      GLint uniform_location = glGetUniformLocation( _shader_program_id, name.c_str() );

      /* insert in the cache */
      _uniform_locations.insert( std::make_pair( name, uniform_location ) );

      /* return the location */
      return uniform_location;
    }

    /* return the location */
    return it->second;
  }

}
