#pragma once

namespace mv {

  /* Put this just below the declaration of a singleton class. */
  #define REGISTER_SINGLETON_CLASS( T )                                                \
              friend class Singleton<T>;                                               \
            public:                                                                    \
              const char* get_class_name( void ) const { return #T; }


  /* wrapper to create a singleton class */
  template<typename T>
  class Singleton {
  protected:
    Singleton( void ) {}
    virtual ~Singleton( void ) {}
  public:
    template<typename... Args>
    static void create_instance( Args... args );
    static void destory_instance( void );

    static T* get( void );
    static T& ref( void );
  private:
    static T* s_instance;
  };

  template<typename T>
  T* Singleton<T>::s_instance = nullptr;

  template<typename T>
  template<typename ...Args>
  inline void Singleton<T>::create_instance( Args... args ) {
    if ( !s_instance )
      s_instance = new T( args... );
  }

  template<typename T>
  inline void Singleton<T>::destory_instance( void ) {
    if ( s_instance )
      delete s_instance;
    s_instance = nullptr;
  }

  template<typename T>
  inline T* Singleton<T>::get( void ) {
    return s_instance;
  }

  template<typename T>
  inline T& Singleton<T>::ref( void ) {
    return *s_instance;
  }

}
