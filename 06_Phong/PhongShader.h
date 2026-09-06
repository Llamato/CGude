#pragma once
#include "Shader.h"

class PhongShader : public Shader {
public:
  PhongShader(const Vec3& viewer, const Vec3& light, const Vec3& light_ambient_color, const Vec3& light_diffuse_color, const Vec3& light_specular_color, float exponent);

  PhongShader(const PhongShader& other);

  virtual ~PhongShader() {}

  virtual Vec3 shade(Vertex surface) const override;

  private:
  Vec3 ka;
  Vec3 kd;
  Vec3 ks;
  Vec3 vpos;
  Vec3 lpos;
  float ns;
};
