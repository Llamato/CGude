#include <iostream>
#include "Triangle.h"

Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2,
                   const Shader& s)
// Completed: initialize member variables here
: vertices{v0, v1, v2}, shader(s)
{}

float dot(Vec2& vector1, Vec2& vector2) {
  return vector1.x * vector2.x + vector1.y * vector2.y;
}

Vec3 cartesianToBarycentric(Vec2& p) {
  Vec2 A = v1 - v0;
  Vec2 B = v2 - v0;
  Vec2 C = p - v0;
  float d00 = dot(A, A);
  float d01 = dot(A, B);
  float d11 = dot(B, B);
  float d20 = dot(C, A);
  float d21 = dot(C, B);
  float denominator = d00 * d20 - d01 * d01;
  float beta = d11 * d20 - d01 * d21;
  float gamma = d00 * d11 - d01 * d21;
  float alpha = 1.0f - beta - gamma;
  return Vec3{alpha, beta, gamma};
}

void Triangle::draw(Image& image) {
  // Completed:
  // Rasterize the triangle into the image. A possible, yet not very efficient,
  // solution would be to compute for every pixel in the image, whether it is
  // inside this triangle or not. If the pixel is inside the triangle you
  // should assign a color to the pixel using the shader of the triangle
  
  for(uint32_t currentRow = 0; currentRow < image.height; currentRow++) {
    for(uint32_t currentColumn = 0; currentColumn < image.width; currentColumn++) {
      Vec2 relativePixel = {1.0f/image.width, 1.0f/image.height};  
      Vec3 barycentric = cartesianToBarycentric(relativePixel);
      bool pixelIsInsideTriangle = barycentric.x > 0.0f && barycentric.y > 0.0f && barycentric.z > 0.0f &&
      barycentric.x < 1.0f && barycentric.y < 1.0f && barycentric.z < 1.0f;
      if (pixelIsInsideTriangle) {
        const Vec3 color = shader.shade(this->vertices[0]);
        std::cout << currentColumn << ':' << currentRow << ':' << '(' << color.r << ',' << color.g << ',' << color.b << ")\n";
        image.setNormalizedValue(currentColumn, currentRow, 0, color.r);
        image.setNormalizedValue(currentColumn, currentRow, 1, color.g);
        image.setNormalizedValue(currentColumn, currentRow, 2, color.b);
      }
    }
  }
}
