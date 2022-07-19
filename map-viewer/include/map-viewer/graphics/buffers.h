#pragma once

#include <tuple>
#include <typeinfo>
#include <vector>

#include <types.h>
#include <utils/ref.h>
#include <utils/exception.h>
#include <utils/logger.h>

#include <debug/profiler.h>

namespace mv {

  /* valid data type for vertex attribute */
  enum class VertexDataType {
    /* all data types are 4 bytes */
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    UInt,
    UInt2,
    UInt3,
    UInt4
  };

  /* return the size in bytes for the data type */
  size_t get_size_in_bytes( VertexDataType type );

  /* returns the number of elements in the data type */
  uint32_t get_num_elements( VertexDataType type );

  /* This structure defines an attribute that can be accessed   */
  /* inside the shader. You can define your own attribute by    */
  /* creating a struct and defining the two static functions.   */
  /* The only restriction is on the data type that can be used  */
  /* inside the shader. As long as `attribute_type` is valid it */
  /* can used. Also the name of the attribute must match the    */
  /* name in the shader.                                        */
  struct Position3 {
    static VertexDataType attribute_type( void ) { return VertexDataType::Float3; }
    static string attribute_name( void ) { return "a_position"; }
    float data[3] = {};
  };

  /* This is another attribute, notice the two functions and */
  /* their respective outputs.                               */
  struct Color4 {
    static VertexDataType attribute_type( void ) { return VertexDataType::Float4; }
    static string attribute_name( void ) { return "a_color"; }
    float data[4] = {};
  };

  /* OpenGL function to handle a vertex buffer. The only */
  /* reason that it is here is because I don't want to   */
  /* include OpenGL function header in this header file. */
  /* They are automatically called from the respective   */
  /* classes so no need to call them by yourself.        */
  uint32_t create_vertex_buffer( size_t size_in_bytes, const void* data );
  void delete_vertex_buffer( uint32_t buffer_id );
  void bind_vertex_buffer( uint32_t buffer_id );
  void* map_vertex_buffer( uint32_t buffer_id, size_t size );
  void unmap_vertex_buffer( uint32_t buffer_id );
  void update_vertex_buffer_element( uint32_t buffer_id, size_t offset, size_t size, const void* data );
  uint32_t insert_in_vertex_buffer( uint32_t buffer_id, size_t offset, size_t size, const void* data );
  uint32_t replace_vertex_buffer_memory( uint32_t buffer_id, size_t offset, size_t old_size, size_t new_size, const void* data );

  /* Vertex buffer with variable number of arguments for */
  /* the vertex attributes. By using this we don't have  */
  /* to worry about the input layout.                    */
  template<typename... Args>
  class VertexBuffer : public SharedObject {
  public:
    VertexBuffer( size_t array_size, const void* data );
    ~VertexBuffer( void );
  public:
    /* Bind the vertex buffer to the pipeline, you won't need  */
    /* to call this function manually, the shader will do this */
    /* internally.                                             */
    void bind( void );

    /* Set data at index. If `map` is called then the data is */
    /* directly added to the CPU buffer and updated during    */
    /* `unmap` call.                                          */
    void set( size_t index, std::tuple<Args...> data );

    /* insert data at offset */
    void insert( size_t offset, std::tuple<Args...> data );

    /* Replace elements with new memory, this also increase */
    /* buffer size if the new size is more than the old.    */
    void replace( size_t offset, size_t elements_to_remove, size_t num_elements, void* data );

    /* map the buffer to CPU memory for quick update */
    void map( void );

    /* unmap the buffer and update GPU memory */
    void unmap( void );

    /* return the offset for a data type in the vertex */
    template<typename T>
    uint32_t get_offset( T& type );

    /* return the number of vertices in the buffer */
    inline size_t array_size( void ) const { return _buffer_array_size; }

    /* return the OpenGL buffer id */
    inline uint32_t buffer_id( void ) const { return _buffer_id; }

    /* return the data type tuple */
    inline auto data_type_tuple( void ) const { return _vertex_type; }

    /* return the vertex size of the buffer */
    inline size_t vertex_size( void ) const { return _vertex_size; }

    /* return the mapped buffer */
    inline uint8_t* mapped_buffer( void ) const { return _mapped_buffer; }
  private:
    /* The function simply iterate over all the elements in  */
    /* the vertex and compute their combined size.           */
    size_t compute_vertex_size( void );
  private:
    /* OpenGL buffer id, should probably in a abstract class */
    /* but since we only have OpenGL I'm not gonna bother    */
    /* and simply put it here.                               */
    uint32_t _buffer_id = 0;

    /* Vertex type is just used to create input layout and   */
    /* update internal data. The argument type here must be  */
    /* similar to `Position3` above, it must have static     */
    /* functions to get attribute size, num of elements and  */
    /* a attribute name.                                     */
    std::tuple<Args...> _vertex_type;

    /* Size of buffer, it is the number of `_vertex_type`    */
    /* elements in buffer and not the actual size in bytes.  */
    size_t _buffer_array_size = 0;

    /* Size of the vertex for the buffer, this multiplied by */
    /* the `_buffer_array_size` gives the total size of the  */
    /* vertex buffer in bytes. For example if the vertex     */
    /* buffer consist of one Position3 and one Color4 the    */
    /* vertex size will be 7 * sizeof( float ), because we   */
    /* have 3 float in Position3 and 4 in Color4.            */
    size_t _vertex_size = 0;

    /* Mapped buffer, this must only be used after a valid   */
    /* `map` call. Before drawing `unmap` must be called.    */
    uint8_t* _mapped_buffer = nullptr;

    /* To determine weather the buffer is mapped or not.     */
    bool _is_mapped = false;
  };

  template<typename... Args>
  template<typename T>
  inline uint32_t VertexBuffer<Args...>::get_offset( T& type ) {
    PROFILE_FUNCTION();

    uint32_t offset = 0;
    bool stop = false;

    std::apply( [&]( auto&&... args ) -> void {
      ( ( typeid( T ) == typeid( args ) ? stop = true : !stop ? offset += get_size_in_bytes( args.attribute_type() ) : offset = offset ), ... );
    }, _vertex_type );

    return offset;
  }

  template<typename... Args>
  inline size_t VertexBuffer<Args...>::compute_vertex_size( void ) {
    PROFILE_FUNCTION();

    /* use C++17's fold expression to compute the size */
    return ( get_size_in_bytes( Args::attribute_type() ) + ... );
  }

  template<typename... Args>
  inline VertexBuffer<Args...>::VertexBuffer( size_t array_size, const void* data ) :
    _buffer_array_size( array_size ) {
    PROFILE_FUNCTION();

    /* compute the vertex size */
    size_t vertex_size = compute_vertex_size();
    _vertex_size = vertex_size;

    /* compute the total size in byes for the buffer */
    size_t size_in_bytes = _buffer_array_size * vertex_size;

    /* create the buffer in GPU memory */
    _buffer_id = create_vertex_buffer( size_in_bytes, data );
  }
  
  template<typename... Args>
  inline VertexBuffer<Args...>::~VertexBuffer( void ) {
    PROFILE_FUNCTION();

    /* delete the vertex buffer and free GPU memory */
    delete_vertex_buffer( _buffer_id );
  }

  template<typename... Args>
  inline void VertexBuffer<Args...>::bind( void ) {
    PROFILE_FUNCTION();

    /* bind the vertex buffer to the graphics pipeline */
    bind_vertex_buffer( _buffer_id );
  }

  template<typename... Args>
  inline void VertexBuffer<Args...>::set( size_t index, std::tuple<Args...> data ) {
    PROFILE_FUNCTION();

    if ( index >= _buffer_array_size ) {
      LOG_ERROR( "index out of bounds, cannot update vertex buffer data: array_size = {}, requested_index = {}", _buffer_array_size, index );
      return;
    }

    std::vector<int8_t> byte_buffer( _vertex_size );

    std::apply( [&]( auto&&... args ) -> void {
      ( memcpy( byte_buffer.data() + get_offset( args ), args.data, get_size_in_bytes( args.attribute_type() ) ), ... );
    }, data );

    update_vertex_buffer_element( _buffer_id, index * _vertex_size, _vertex_size, byte_buffer.data() );
  }

  template<typename... Args>
  inline void VertexBuffer<Args...>::insert( size_t offset, std::tuple<Args...> data ) {
    PROFILE_FUNCTION();

    if ( offset > _buffer_array_size ) {
      LOG_ERROR( "index out of bounds, cannot insert vertex buffer data: array_size = {}, requested_index = {}", _buffer_array_size, offset );
      return;
    }

    std::vector<int8_t> byte_buffer( _vertex_size );

    std::apply( [&]( auto&&... args ) -> void {
      ( memcpy( byte_buffer.data() + get_offset( args ), args.data, get_size_in_bytes( args.attribute_type() ) ), ... );
    }, data );

    _buffer_id = insert_in_vertex_buffer( _buffer_id, offset * _vertex_size, _vertex_size, byte_buffer.data() );
    _buffer_array_size++;
  }

  template<typename ...Args>
  inline void VertexBuffer<Args...>::replace( size_t offset, size_t elements_to_remove, size_t num_elements, void* data ) {
    PROFILE_FUNCTION();

    MV_ASSERT( data );

    _buffer_id = replace_vertex_buffer_memory( _buffer_id, offset * _vertex_size, elements_to_remove * _vertex_size, num_elements * _vertex_size, data );
    _buffer_array_size += ( num_elements - elements_to_remove );
  }

  template<typename ...Args>
  inline void VertexBuffer<Args...>::map( void ) {
    PROFILE_FUNCTION();

    _mapped_buffer = (uint8_t*)map_vertex_buffer( _buffer_id, _buffer_array_size * _vertex_size );
    MV_ASSERT( _mapped_buffer );
    _is_mapped = true;
  }

  template<typename ...Args>
  inline void VertexBuffer<Args...>::unmap( void ) {
    PROFILE_FUNCTION();

    if ( _is_mapped ) {
      unmap_vertex_buffer( _buffer_id );
      _mapped_buffer = nullptr;
      _is_mapped = false;
    }
  }

  /* OpenGL function to handle a shader storage buffer. */
  /* They are automatically called from the respective  */
  /* classes so no need to call them by yourself.       */
  uint32_t create_ssbo( size_t size_in_bytes, const void* data );
  void delete_ssbo( uint32_t buffer_id );
  void retrieve_data( uint32_t buffer_id, size_t size, void* output_buffer );

  /* A buffer which can be both written and read from simultaniously */
  /* from a glsl shader. To bind it pass it to the shader.           */
  template<typename T, size_t S = 1>
  class ShaderStorageBuffer : public SharedObject {
  public:
    ShaderStorageBuffer( const T* data = nullptr );
    ~ShaderStorageBuffer( void );
  public:
    /* call this function to copy data from the GPU memory    */
    /* to CPU memory, without this call the data is undefined */
    void retrieve( void );

    /* return the OpenGL buffer id */
    inline uint32_t buffer_id( void ) const { return _buffer_id; }

    /* get data, only valid after `retrieve` call */
    inline T operator[]( size_t index ) const;
  private:
    /* array size of the buffer */
    size_t _buffer_array_size = S;

    /* size of each element in the array */
    size_t _element_size      = sizeof( T );

    /* memory to hold the data so that the user */
    /* does not have to provide it explicitly   */
    T _cpu_view[S]            = {};

    /* OpenGL buffer id */
    uint32_t _buffer_id       = 0;
  };

  template<typename T, size_t S>
  inline ShaderStorageBuffer<T, S>::ShaderStorageBuffer( const T* data ) {
    PROFILE_FUNCTION();

    size_t size_in_bytes = S * sizeof( T );

    /* create the buffer in GPU memory */
    _buffer_id = create_ssbo( size_in_bytes, data );
  }

  template<typename T, size_t S>
  inline ShaderStorageBuffer<T, S>::~ShaderStorageBuffer( void ) {
    PROFILE_FUNCTION();

    /* delete the vertex buffer and free GPU memory */
    delete_ssbo( _buffer_id );
  }

  template<typename T, size_t S>
  inline void ShaderStorageBuffer<T, S>::retrieve( void ) {
    PROFILE_FUNCTION();

    /* retrieve the data */
    retrieve_data( _buffer_id, _buffer_array_size * _element_size, &_cpu_view );
  }

  template<typename T, size_t S>
  inline T ShaderStorageBuffer<T, S>::operator[]( size_t index ) const {
    PROFILE_FUNCTION();

    if ( index >= _buffer_array_size )
      THROW( "shader storage buffer inex out of bounds" );

    return _cpu_view[index];
  }


  /* Index buffer is used to do random access on vertex data while  */
  /* drawing, the major benifit here is that we can reuse vertices. */
  /* This reduce memory usage and also make editing data simpler.   */
  class IndexBuffer : public SharedObject {
  public:
    IndexBuffer( const std::vector<uint32_t> indices );
    ~IndexBuffer( void );
  public:
    /* bind the index buffer to the pipeline */
    void bind( void ) const;

    /* return the number of indices in the buffer */
    inline uint32_t indices( void ) const { return _num_indices; }
  private:
    /* number of indices in the buffer */
    size_t _num_indices = 0;

    /* OpenGL buffer id */
    uint32_t _buffer_id = 0;
  };

}
