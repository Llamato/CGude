#include "Sphere.h"

Sphere::Sphere(const Vec3& center, float radius, const Material& material)
	: center(center)
	, radius(radius)
	, material(material)
{
	// Completed: implement this constructor and the rest of this class necessary for the assignment
}

Material Sphere::getMaterial() const
{
	// Completed: implement this method and the rest of this class necessary for the assignment
	return material;
}

std::optional<Intersection> Sphere::intersect(const Ray& ray) const
{
	// Completed: implement this method and the rest of this class necessary for the assignment
	float epsilon = 0.01f;
	Vec3 rayOrigin = ray.getOrigin();
	Vec3 rayDirection = ray.getDirection();
	Vec3 pixelPos = rayOrigin + rayDirection;
	Vec3 sphereCenter = center;
	float radius = this->radius.x;
	float x_1 = rayOrigin.x;
	float y_1 = rayOrigin.y;
	float z_1 = rayOrigin.z;
	float x_2 = pixelPos.x;
	float y_2 = pixelPos.y;
	float z_2 = pixelPos.z;
	float x_3 = sphereCenter.x;
	float y_3 = sphereCenter.y;
	float z_3 = sphereCenter.z;
	float r = radius;
	float a = pow(x_2 - x_1, 2) + pow(y_2 - y_1, 2) + pow(z_2 - z_1, 2);
	float b = 2 * ((x_2 - x_1) * (x_1 - x_3) + (y_2 - y_1) * (y_1 - y_3) + (z_2 - z_1) * (z_1 - z_3));
	float c = pow(x_3, 2) + pow(y_3, 2) + pow(z_3, 2) + pow(x_1, 2) + pow(y_1, 2) + pow(z_1, 2) - 2 * (x_3 * x_1 + y_3 * y_1 + z_3 * z_1) - pow(r, 2);
	float isIntersecting = pow(b, 2) - 4 * a * c;
	if(isIntersecting >= 0){
		float intersectionPoint1 = (-b - sqrt(isIntersecting)) / (2 * a);
		float intersectionPoint2 = (-b + sqrt(isIntersecting)) / (2 * a);
		float pointOfClosestIntersection;
		if(intersectionPoint1 < epsilon && intersectionPoint2 < epsilon) return std::nullopt;
		if(intersectionPoint1 < epsilon) pointOfClosestIntersection = intersectionPoint2;
		else if(intersectionPoint2 < epsilon) pointOfClosestIntersection = intersectionPoint1;
		else if(intersectionPoint1 < intersectionPoint2) pointOfClosestIntersection = intersectionPoint1;
		else pointOfClosestIntersection = intersectionPoint2;
		Vec3 intersectionVector = rayOrigin + rayDirection * pointOfClosestIntersection;
		return Intersection{this->material, Vec3::normalize(intersectionVector - this->center), pointOfClosestIntersection};
    }
    return std::nullopt;
}
