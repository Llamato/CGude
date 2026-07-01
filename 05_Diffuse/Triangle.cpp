#include <iostream>
#include "Triangle.h"

Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2,
                   const Shader& s)
// Completed: initialize member variables here
: vertices{v0, v1, v2}, shader(s)
{}

Vec3 cartesianToBarycentric(Vec3& point) {
  const float idet = 1/(v0.x * (v1.y - v2.y) + v1.x * (v2.y - v0.y) + v2.x * (v0.y - v1.y));
  const Mat3 T = Mat3{
    v1.x * v2.y - v2.x * v1.y, v1.y - v2.y, v2.x - v1.x,
    v2.x * v0.y - v0.x * v2.y, v2.y - v0.y, v0.x - v2.x,
    x0.x * v2.y - v1.x * v0.y, v0.y - v1.y, v1.x - v0.x
  };
  const Vec3 V = Vec3{1, point.x, point.y};
  const Vec3 barycentric = idet * T * V;
  return barycentric;
}

void Triangle::draw(Image& image) {
  // Completed:
  // Rasterize the triangle into the image. A possible, yet not very efficient,
  // solution would be to compute for every pixel in the image, whether it is
  // inside this triangle or not. If the pixel is inside the triangle you
  // should assign a color to the pixel using the shader of the triangle
  
  for(uint32_t currentRow = 0; currentRow < image.height; currentRow++) {
    for(uint32_t currentColumn = 0; currentColumn < image.width; currentColumn++) {
      const Vec2 relativePixel = {1/image.width, 1/image.height};  
      const Vec3 barycentric = cartesianToBarycentric(relativePixel);
      bool pixelIsInsideTriangle = barycentric.x > 0.0f && barycentric.y > 0.0f && barycentric.z > 0.0f;
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
