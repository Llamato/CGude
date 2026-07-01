#pragma once

#include "Vertex.h"
#include "Image.h"
#include "Shader.h"

#define TRIANGLE_VERTEX_COUNT 3

class Triangle {
public:
  Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Shader& s);

  void draw(Image& image);

private:
  // Completed: add the appropriate member variables here
  Vertex vertices[TRIANGLE_VERTEX_COUNT];
  const Shader& shader;

  Vec3 cartesianToBarycentric(Vec3& point);
};
