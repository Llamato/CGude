#pragma once
#include "Shader.h"

class DiffuseShader : public Shader {
public:
  DiffuseShader(const Vec3& light, const Vec3& light_diffuse_color);
  virtual ~DiffuseShader() {}

  // Inherited via Shader
  virtual Vec3 shade(Vertex surface) const override;

private:
  // Completed: add the appropriate member variables here
  Vec3 light;
  Vec3 light_diffuse_color;

};
