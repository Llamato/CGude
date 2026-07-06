#include <iostream>
#include "AmbientShader.h"

Vec3 AmbientShader::shade(Vertex surface) const {
  /* Completed:
   Implement this method and the rest of this class for the assignment
   This method should return the ambient color of the given vertex
   */
  
  // just a dummy return value, needs to be replaced with the right one
  //return Vec3{0, 0, 0};
  
  return surface.material.color_ambient;
}
