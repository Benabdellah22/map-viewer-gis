#pragma once

#include <core/math.h>
#include <utils/ref.h>

namespace mv {

  /* Camera interface, to create a camera simply create a  */
  /* parent class that inherit from the `Camera` class and */
  /* then implement all the pure virtual functions.        */
  class Camera : public SharedObject {
  protected:
    Camera( void ) {}
    virtual ~Camera( void ) {}
  public:
    /* return the projection matrix */
    virtual glm::mat4 projection( void ) const = 0;

    /* return the camera view matrix */
    virtual glm::mat4 view( void ) const = 0;

    /* A generic update function that is called every */
    /* frame, you can do anything you want in this.   */
    virtual void update( void ) = 0;

    /* called in case the window is resized */
    virtual void resize( glm::vec2 new_size ) = 0;
  };

}
