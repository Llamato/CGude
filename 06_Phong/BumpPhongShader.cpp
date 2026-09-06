#include "BumpPhongShader.h"
#include <cmath>

/**
* @param phong a phong shader to use for the actual shading
* @param cellSize size of the bumps in the x-y-plane measured in pixels
* @param bumpHeight amplitude of the bumps in positive z-direction (out of the image plane)
*/

float sinf2(float x) {
	float y = std::sin(x);
	return y * y;
}

float f(float x, float y) {
	return sinf2(x * M_PI) * sinf2(y * M_PI);
}

float fx(float x, float y) {
	return 2 * M_PI * sinf2(M_PI * y) * cos(M_PI * x) * sin(M_PI * x);
}

float fy(float x, float y) {
	return 2 * M_PI * sinf2(M_PI * x) * cos(M_PI * y) * sin(M_PI * y);
}

BumpPhongShader::BumpPhongShader(const PhongShader& phong, float cellSize, float bumpHeight): PhongShader(phong)
{
	this->cs = cellSize;
	this->bh = bumpHeight;
}

// Inherited via Shader
Vec3 BumpPhongShader::shade(Vertex surface) const
{
    float u = surface.position.x / this->cs;
    float v = surface.position.y / this->cs;
    Vec3 gradient = Vec3{this->bh * fx(u, v), this->bh * fy(u, v), 1.0f};
    surface.normal = Vec3::normalize(gradient);
    return PhongShader::shade(surface);
}
