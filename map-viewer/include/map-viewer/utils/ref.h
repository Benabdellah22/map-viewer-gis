#pragma once

#include <utils/assert.h>
#include <types.h>

namespace mv {

  template<typename T>
  class Ref;

  /* inherit from this class to turn reference counting on in any class */
  class SharedObject {
    template<typename>
    friend class Ref;
  protected:
    SharedObject( void ) : _reference_count( 0 ) {}
    virtual ~SharedObject( void ) {}
  private:
    void add_ref( void ) { _reference_count++; }
    void rem_ref( void ) { if ( _reference_count > 0 ) _reference_count--; }

    size_t ref_count( void ) const { return _reference_count; }
  private:
    size_t _reference_count;
  };

  /* This class uses the above interface to reference count objects  */
  /* and destroy them once the reference count reaches 0. This is    */
  /* kind of like smart pointers but it handled manually to increase */
  /* performance.                                                    */
  template<typename T>
  class Ref {
  public:
    Ref( void ) : _instance( nullptr ) {}
    Ref( T* instance ) : _instance( nullptr ) { if ( instance ) { _instance = instance; _instance->add_ref(); } }
    Ref( const Ref& other ) : _instance( nullptr ) { if ( other ) { _instance = other._instance; _instance->add_ref(); } }
    virtual ~Ref( void ) { destruct(); }
  public:
    template<typename B>
    operator Ref<B>( void ) const {
      Ref<B> temp;
      B* base = dynamic_cast<B*>( _instance );

      if ( base )
        temp = base;
      else
        MV_ASSERT( "class does not inherit from base" );

      return temp;
    }

    void operator =( const Ref& other ) {
      if ( other ) {
        destruct();

        _instance = other._instance;
        _instance->add_ref();
      }
    }

    void operator =( T* const other ) {
      destruct();

      _instance = other;
      if ( _instance )
        _instance->add_ref();
    }

    T* operator ->( void ) const {
      MV_ASSERT( _instance );
      return _instance;
    }

    T& operator *( void ) const {
      MV_ASSERT( _instance );
      return *_instance;
    }

    operator bool( void ) const { return _instance != nullptr; }
    bool operator ==( const Ref& other ) const { return _instance == other._instance; }
    bool operator !=( const Ref& other ) const { return _instance != other._instance; }
    bool operator ==( const T* const other ) const { return _instance == other; }
    bool operator !=( const T* const other ) const { return _instance != other; }
  private:
    void destruct( void ) {
      if ( _instance ) {
        _instance->rem_ref();

        if ( _instance->ref_count() == 0 ) {
          SharedObject* base_class = dynamic_cast<T*>( _instance );
          delete base_class;
        }

        _instance = nullptr;
      }
    }
  private:
    T* _instance;
  };

}
