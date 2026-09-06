#include "PhongShader.h"
#include <cmath>
#include <algorithm>

PhongShader::PhongShader(const Vec3& viewer, const Vec3& light, const Vec3& light_ambient_color, const Vec3& light_diffuse_color, const Vec3& light_specular_color, float exponent)
{
  this->ka = light_ambient_color;
  this->kd = light_diffuse_color;
  this->ks = light_specular_color;
  this->vpos = viewer;
  this->lpos = light;
  this->ns = exponent;
}

PhongShader::PhongShader(const PhongShader& other)
{
    this->ka = other.ka;
    this->kd = other.kd;
    this->ks = other.ks;
    this->vpos = other.vpos;
    this->lpos = other.lpos;
    this->ns = other.ns;
}

Vec3 PhongShader::shade(Vertex surface) const
{
  const Vec3 l = Vec3::normalize(lpos - surface.position);
  const Vec3 n = Vec3::normalize(surface.normal);
  const Vec3 v = Vec3::normalize(vpos - surface.position);
  const Vec3 r = Vec3::normalize(2 * Vec3::dot(n, l) * n - l);
  const Vec3 ambient = surface.material.color_ambient * ka;
  const Vec3 diffuse = surface.material.color_diffuse * std::max(0.0f, Vec3::dot(l, n));
  const Vec3 specular = surface.material.color_specular * pow(std::max(0.0f, Vec3::dot(r, v)), ns);
  return ambient + diffuse + specular;
}
