#include "sphere.h"
#include <cmath>
#include <limits>

Sphere::Sphere(const std::vector<float> center, float radius)
    : center(center), radius(radius) {}

bool Sphere::intersectSphere(const Ray& ray, float& t) const{
    std::vector<float> oc = {ray.origin[0] - center[0], ray.origin[1] - center[1], ray.origin[2] - center[2]}; 
    float a = ray.direction[0] * ray.direction[0] + ray.direction[1] * ray.direction[1] + ray.direction[2] * ray.direction[2];
    float b = 2.0f * (ray.direction[0] * oc[0] + ray.direction[1] * oc[1] + ray.direction[2] * oc[2]);
    float c = oc[0]*oc[0] + oc[1]*oc[1] + oc[2]*oc[2] - radius * radius;
    float discriminant = b*b - 4*a*c;
    float temp2 = (-b + sqrt(discriminant)) / (2.0f * a);
    float temp1 = (-b - sqrt(discriminant)) / (2.0f * a);
    if (discriminant < 0) {
        return false;
    } else {
        if (temp1 > 0) {
            t = temp1;
            return true;
        } else if (temp2 > 0) {
            t = temp2;
            return true;
        }
    }
    return false;
}