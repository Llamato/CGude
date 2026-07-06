#include <iostream>
#include "DiffuseShader.h"

DiffuseShader::DiffuseShader(const Vec3& light, const Vec3& light_diffuse_color)
// Completed: initialize member variables here
{
  this->light = light;
  this->light_diffuse_color = light_diffuse_color;
}

Vec3 DiffuseShader::shade(Vertex surface) const {
  /* TODO:
   Implement this method and the rest of this class for the assignment
   This method should compute and return color of the given vertex using
   diffuse illumination.
   */

  // just a dummy return value, needs to be replaced with the right one
  //return Vec3{1.0f,1.0f,1.0f};

  Vec3 lightDirrection = Vec3::normalize(this->light - surface.position);
  Vec3 surfaceNormal = Vec3::normalize(surface.normal); // Just in case surface.normal is not normalized already. Somehow...
  float diffuseEffectManitude = std::max(Vec3::dot(surfaceNormal, lightDirrection), 0.0f);
  Vec3 directedDiffusedLightRay = diffuseEffectManitude * this->light_diffuse_color;
  return directedDiffusedLightRay * surface.material.color_diffuse;
}
