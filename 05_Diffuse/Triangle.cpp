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

Vec3 Triangle::cartesianToBarycentric(Vec2& p) {
  //Reassignment for easier math
  Vec2 v0 = this->vertices[0].position.xy;
  Vec2 v1 = this->vertices[1].position.xy;
  Vec2 v2 = this->vertices[2].position.xy;

  //Translate verticies such that A becomes the origin
  Vec2 A = v1 - v0;
  Vec2 B = v2 - v0;
  Vec2 C = p - v0;

  //Solve 
  // C.x = beta * A.x + gamma * B.x
  // C.y = beta * A.y + gamma * B.y
  //By using dot product projection for numerical stability
  float d00 = dot(A, A);
  float d01 = dot(A, B);
  float d11 = dot(B, B);
  float d20 = dot(C, A);
  float d21 = dot(C, B);

  //Now we can instead solve
  // dot(C, A) = beta * dot(A, A) + gamma * dot(B, A)
  // dot(C, B) = beta * dot(A, B) + gamma * dot(B, B)
  //for beta and gamma using Cramer's Rule, Witch states that for
  //a*x + b*y = e
  //c*x + d*y = f
  //The solutions are
  //x = (e*d - b*f) / (a*d - b*c)
  //y = (a*f - e*c) / (a*d - b*c)
  float denominator = d00 * d11 - d01 * d01;
  float beta = (d11 * d20 - d01 * d21) / denominator;
  float gamma = (d00 * d21 - d01 * d20) / denominator;

  //Lastly we use the triangle identity: alpha + beta + gamma = 1
  //to solve for alpha.
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
      Vec2 absolutePixel = {currentColumn, currentRow};
      Vec3 barycentric = cartesianToBarycentric(absolutePixel);
      bool pixelIsInsideTriangle = barycentric.x > 0.0f && barycentric.y > 0.0f && barycentric.z > 0.0f;
      if(pixelIsInsideTriangle) {
        const Vec3 color = shader.shade(Vertex{Vec3{absolutePixel.x, absolutePixel.y, 1.0f}, Material{barycentric}});
        image.setNormalizedValue(currentColumn, currentRow, 0, color.r);
        image.setNormalizedValue(currentColumn, currentRow, 1, color.g);
        image.setNormalizedValue(currentColumn, currentRow, 2, color.b);
      }
    }
  }
}
