#include <iostream>
#include "AmbientShader.h"

Vec3 AmbientShader::shade(Vertex surface) const {
  /* Completed:
   Implement this method and the rest of this class for the assignment
   This method should return the ambient color of the given vertex
   */
  
  // just a dummy return value, needs to be replaced with the right one
  std::cout << surface.material.color_ambient << '\n';
  return surface.material.color_ambient;
}
