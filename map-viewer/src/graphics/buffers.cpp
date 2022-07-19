#include <graphics/buffers.h>

#include <glad/glad.h>

#include <utils/logger.h>
#include <utils/exception.h>

namespace mv {

  size_t get_size_in_bytes( VertexDataType type ) {
    switch ( type ) {
    case VertexDataType::Float:  return sizeof( float );
    case VertexDataType::Float2: return sizeof( float ) * 2;
    case VertexDataType::Float3: return sizeof( float ) * 3;
    case VertexDataType::Float4: return sizeof( float ) * 4;
    case VertexDataType::Int:    return sizeof( int32_t );
    case VertexDataType::Int2:   return sizeof( int32_t ) * 2;
    case VertexDataType::Int3:   return sizeof( int32_t ) * 3;
    case VertexDataType::Int4:   return sizeof( int32_t ) * 4;
    case VertexDataType::UInt:   return sizeof( uint32_t );
    case VertexDataType::UInt2:  return sizeof( uint32_t ) * 2;
    case VertexDataType::UInt3:  return sizeof( uint32_t ) * 3;
    case VertexDataType::UInt4:  return sizeof( uint32_t ) * 4;
    default:
      break;
    }

    THROW( "invalid vertex data type" );
    return 0;
  }

  uint32_t get_num_elements( VertexDataType type ) {
    switch ( type ) {
    case VertexDataType::UInt:
    case VertexDataType::Int:
    case VertexDataType::Float:  return 1;
    case VertexDataType::Int2:
    case VertexDataType::UInt2:
    case VertexDataType::Float2: return 2;
    case VertexDataType::Int3:
    case VertexDataType::UInt3:
    case VertexDataType::Float3: return 3;
    case VertexDataType::Int4:
    case VertexDataType::UInt4:
    case VertexDataType::Float4: return 4;
    default:
      break;
    }

    THROW( "invalid vertex data type" );
    return 0;
  }

  uint32_t create_vertex_buffer( size_t size_in_bytes, const void* data ) {
    PROFILE_FUNCTION();

    GLuint id = 0;

    /* allocate the OpenGL buffer object */
    glCreateBuffers( 1, &id );
    if ( id == 0 )
      THROW( "failed to create opengl buffer object" );

    /* Allocate the buffer using size and initialize with the data */
    /* if the data is null then the buffer will simply be empty.   */
    glNamedBufferData( id, size_in_bytes, data, GL_DYNAMIC_DRAW );

    LOG_INFO( "created opengl vertex buffer: id = {}", id );
    return (uint32_t)id;
  }

  void delete_vertex_buffer( uint32_t buffer_id ) {
    PROFILE_FUNCTION();

    LOG_INFO( "deleted opengl vertex buffer: id = {}", buffer_id );
    glDeleteBuffers( 1, &buffer_id );
  }

  void bind_vertex_buffer( uint32_t buffer_id ) {
    PROFILE_FUNCTION();

    glBindBuffer( GL_ARRAY_BUFFER, buffer_id );
  }

  void* map_vertex_buffer( uint32_t buffer_id, size_t size ) {
    PROFILE_FUNCTION();

    MV_ASSERT( buffer_id != 0 && size != 0 );
    return glMapNamedBufferRange( buffer_id, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
  }

  void unmap_vertex_buffer( uint32_t buffer_id ) {
    PROFILE_FUNCTION();

    MV_ASSERT( buffer_id != 0 );
    glUnmapNamedBuffer( buffer_id );
  }

  void update_vertex_buffer_element( uint32_t buffer_id, size_t offset, size_t size, const void* data ) {
    PROFILE_FUNCTION();

    MV_ASSERT( buffer_id != 0 );
    glNamedBufferSubData( buffer_id, (GLintptr)offset, size, data );
  }

  uint32_t insert_in_vertex_buffer( uint32_t buffer_id, size_t offset, size_t size, const void* data ) {
    PROFILE_FUNCTION();

    MV_ASSERT( buffer_id != 0 );

    /* get the old buffer size */
    GLint64 old_buffer_size = 0;
    glGetNamedBufferParameteri64v( buffer_id, GL_BUFFER_SIZE, &old_buffer_size );

    /* assert that the offset is valid */
    MV_ASSERT( offset <= old_buffer_size );

    /* create a new buffer to hold the combined data */
    uint32_t new_buffer = create_vertex_buffer( old_buffer_size + size, nullptr );

    /* copy the old data behind offset */
    if ( offset != 0 )
      glCopyNamedBufferSubData( buffer_id, new_buffer, 0, 0, offset );

    /* append the new data */
    glNamedBufferSubData( new_buffer, offset, size, data );

    /* copy the old data after offset */
    if ( offset != old_buffer_size )
      glCopyNamedBufferSubData( buffer_id, new_buffer, offset, offset + size, old_buffer_size - offset );

    /* delete the old buffer */
    delete_vertex_buffer( buffer_id );

    return new_buffer;
  }

  uint32_t replace_vertex_buffer_memory( uint32_t buffer_id, size_t offset, size_t old_size, size_t new_size, const void* data ) {
    PROFILE_FUNCTION();

    MV_ASSERT( buffer_id != 0 );

    /* get the old buffer size */
    GLint64 old_buffer_size = 0;
    glGetNamedBufferParameteri64v( buffer_id, GL_BUFFER_SIZE, &old_buffer_size );

    /* assert that the offset and old size is valid */
    MV_ASSERT( offset + old_size <= old_buffer_size );

    /* If the memory to replace can fit in the buffer, update the */
    /* old buffer's memory and simply return the same buffer.     */
    if ( new_size == old_size ) {
      glNamedBufferSubData( buffer_id, offset, new_size, data );
      return buffer_id;
    }

    /* compute the new size */
    size_t new_buffer_size = old_buffer_size - old_size + new_size;

    /* create a new buffer with appropriate size */
    uint32_t new_buffer = create_vertex_buffer( new_buffer_size, nullptr );

    /* copy the old data behind offset */
    if ( offset != 0 )
      glCopyNamedBufferSubData( buffer_id, new_buffer, 0, 0, offset );

    /* copy the new data after offset */
    glNamedBufferSubData( new_buffer, offset, new_size, data );

    /* copy the old data after old_size */
    if ( offset != old_buffer_size )
      glCopyNamedBufferSubData( buffer_id, new_buffer, offset + old_size, offset + new_size, old_buffer_size - offset - old_size );

    /* delete the old buffer */
    delete_vertex_buffer( buffer_id );

    return new_buffer;
  }

  uint32_t create_ssbo( size_t size_in_bytes, const void* data ) {
    PROFILE_FUNCTION();

    GLuint id = 0;

    /* allocate the OpenGL buffer object */
    glCreateBuffers( 1, &id );
    if ( id == 0 )
      THROW( "failed to create opengl buffer object" );

    /* Allocate the buffer using size and initialize with the data */
    /* if the data is null then the buffer will simply be empty.   */
    glNamedBufferStorage( id, size_in_bytes, data, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT );

    LOG_INFO( "created opengl shader storage buffer: id = {}", id );
    return (uint32_t)id;
  }

  void delete_ssbo( uint32_t buffer_id ) {
    PROFILE_FUNCTION();

    LOG_INFO( "deleted opengl shader storage buffer: id = {}", buffer_id );
    glDeleteBuffers( 1, &buffer_id );
  }

  void retrieve_data( uint32_t buffer_id, size_t size, void* output_buffer ) {
    PROFILE_FUNCTION();

    MV_ASSERT( output_buffer );

    /* map the buffer to the memory */
    void* mapped_buffer = glMapNamedBufferRange( buffer_id, 0, size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT );

    /* copy the data to the output buffer */
    memcpy( output_buffer, mapped_buffer, size );

    /* zero out the GPU memory */
    memset( mapped_buffer, 0, size );

    /* unmap the buffer */
    glUnmapNamedBuffer( buffer_id );
  }

  IndexBuffer::IndexBuffer( const std::vector<uint32_t> indices ) : _num_indices( indices.size() ) {
    PROFILE_FUNCTION();

    GLuint id = 0;

    /* allocate the OpenGL buffer object */
    glCreateBuffers( 1, &id );
    if ( id == 0 )
      THROW( "failed to create opengl buffer object" );

    /* upload data to the GPU memory */
    glNamedBufferData( id, indices.size() * sizeof( indices[0] ), indices.data(), GL_STATIC_DRAW );

    LOG_INFO( "created opengl index_buffer buffer: id = {}", id );

    /* assign member */
    _buffer_id = id;
  }

  IndexBuffer::~IndexBuffer( void ) {
    PROFILE_FUNCTION();

    LOG_INFO( "deleted opengl index_buffer buffer: id = {}", _buffer_id );
    glDeleteBuffers( 1, &_buffer_id );
  }

  void IndexBuffer::bind( void ) const {
    PROFILE_FUNCTION();

    /* bind the index buffer to the pipeline */
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _buffer_id );
  }

}
