#pragma once

#include <vector>
#include <typeinfo>
#include <any>
#include <unordered_map>

#include <types.h>
#include <utils/ref.h>
#include <core/math.h>
#include <graphics/buffers.h>

namespace mv {

  /* [Note]: In comments and other places `vertex array` and  */
  /*         `input layout` is used and they mean the same    */
  /*         thing. OpenGL use `vertex array` whereas directx */
  /*         use `input layout` for shader input.             */

  /* Represents a shader object which contains a combination of  */
  /* vertex and fragment shaders. The shader program can be used */
  /* to specify vertex transformation and fragment colors in the */
  /* pipeline.                                                   */
  class Shader : public SharedObject {
  private:
    struct InputLayoutDesc {
      string         attribute_name;
      VertexDataType data_type;
      uint32_t       input_buffer_id;
      uint32_t       offset;
      size_t         stride;
    };
  public:
    Shader( const string& vertex_shader_code, const string& fragment_shader_code );
    ~Shader( void );
  public:
    /* bind the shader program to the pipeline */
    void use( void );

    /* unbind the shader program from the pipeline */
    void unuse( void );

    /* Since input layout is dependent on the shader it makes    */
    /* much more sense to insert input data to the shader rather */
    /* than creating input layout separately and binding it to   */
    /* the pipeline. Currently whenever this function is called  */
    /* a new input layout is created, but, that can easily be    */
    /* improved by using a hash table to store the input layout. */
    template<typename... Args>
    void set_input_buffers( Args... buffers );

    /* below are the functions to set uniform */
    void set_mat4( const string& uniform_name, const glm::mat4& matrix ) const;
    void set_vec2( const string& uniform_name, const glm::vec2& vector ) const;
    void set_vec3( const string& uniform_name, const glm::vec3& vector ) const;
    void set_vec4( const string& uniform_name, const glm::vec4& vector ) const;
    void set_ivec2( const string& uniform_name, const glm::ivec2& vector ) const;
    void set_ivec3( const string& uniform_name, const glm::ivec3& vector ) const;
    void set_ivec4( const string& uniform_name, const glm::ivec4& vector ) const;
    void set_uint( const string& uniform_name, const uint32_t& value ) const;
    void set_int( const string& uniform_name, const int32_t& value ) const;
    void set_int_array( const string& uniform_name, size_t size, const int32_t* value ) const;
    void set_uint_array( const string& uniform_name, size_t size, const uint32_t* value ) const;

    /* bind the shader storage buffer to the block with `storage_name` */
    template<typename T, size_t S>
    void set_ssbo( const string& storage_name, Ref<ShaderStorageBuffer<T, S>> ssbo ) const;
  private:
    template<typename T>
    void build_input_layout_desc( std::vector<Shader::InputLayoutDesc>& ild, T& buffer );

    /* create input layout aka vertex attribute for shader input */
    void create_input_layout( const std::vector<Shader::InputLayoutDesc>& ild );

    /* delete the previously created input layout */
    void delete_input_layout( void );

    /* bind the input layout to the pipeline */
    void bind_input_layout( void ) const;

    /* unbind the input layout from the pipeline */
    void unbind_input_layout( void ) const;

    /* bind the shader storage object to the pipeline */
    void bind_shader_storage_internal( const string& name, uint32_t buffer_id ) const;

    /* get uniform location this also store the locations to avoid */
    /* glGetUniformLocation calls which slows the process a lot    */
    int32_t get_uniform_location( const string& name ) const;
  private:
    /* OpenGL shader program id, should probably in a abstract  */
    /* class but since we only have OpenGL I'm not gonna bother */
    /* and simply put it here.                                  */
    uint32_t _shader_program_id = 0;

    /* vertex attribute that define the input layout  */
    uint32_t _vertex_attribute  = 0;

    /* Vertex buffers that currently in use of the shader, this */
    /* is just here to ensure that the buffers are not released */
    /* while still in use of the shader.                        */
    std::vector<std::any> _set_buffers;

    /* Shader Storage Buffer binding, this is to speed up the   */
    /* binding process on subsiquient calls to bind the buffer. */
    mutable std::unordered_map<string, uint32_t> _shader_storage_binding;

    /* This is to store uniform locations and speed up the process */
    mutable std::unordered_map<string, int32_t> _uniform_locations;
  };


  template<typename T>
  void Shader::build_input_layout_desc( std::vector<Shader::InputLayoutDesc>& ild, T& buffer ) {
    auto data_type = buffer.data_type_tuple();

    std::apply( [&]( auto&&... args ) -> void {
      ( ( ild.push_back( { args.attribute_name(), args.attribute_type(), buffer.buffer_id(), buffer.get_offset( args ), buffer.vertex_size() } ) ), ... );
    }, data_type );

  }

  template<typename... Args>
  void Shader::set_input_buffers( Args... buffers ) {
    PROFILE_FUNCTION();

    /* generate the input layout desc array to be passed to internal API */
    std::vector<Shader::InputLayoutDesc> input_layout_desc;
    ( build_input_layout_desc( input_layout_desc, *buffers ), ... );

    /* remove previous buffer references if any */
    _set_buffers.clear();

    /* hold references to buffer so they do not get deallocated */
    ( _set_buffers.push_back( buffers ), ... );

    /* create the internal input layout desc */
    create_input_layout( input_layout_desc );
  }

  template<typename T, size_t S>
  inline void Shader::set_ssbo( const string& storage_name, Ref<ShaderStorageBuffer<T, S>> ssbo ) const {
    bind_shader_storage_internal( storage_name, ssbo->buffer_id() );
  }

}
