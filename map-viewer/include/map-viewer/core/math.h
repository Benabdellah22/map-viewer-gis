#pragma once

/* This file is used as a wrapper so that we don't have */
/* to include <glm/glm.hpp> everywhere and also in case */
/* we decide to change the math library in the future,  */
/* it will be easier.                                   */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

#include <limits>

namespace mv {

  struct Box {
    /* adjust the bounding box to include to `vertex` */
    void include( glm::vec3 vertex ) {
      min = glm::min( vertex, min );
      max = glm::max( vertex, max );
    }

    void unify( const Box& other ) {
      min = glm::min( other.min, min );
      max = glm::max( other.max, max );
    }

    glm::vec3 center( void ) const {
      return ( max + min ) / 2.0f;
    }

    glm::vec3 size( void ) const {
      return glm::abs( max - min );
    }

    glm::vec3 min = glm::vec3(  std::numeric_limits<float>::max() );
    glm::vec3 max = glm::vec3( -std::numeric_limits<float>::max() );
  };

}


