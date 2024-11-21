#include "sphere.h"

Sphere::Sphere(std::vector<float> center, float radius)
    : center(center), radius(radius) {}

bool Sphere::intersectSphere(const Ray& ray, float& t) const{
    std::vector<float> oc = {ray.origin[0] - center[0], ray.origin[1] - center[1], ray.origin[2] - center[2]}; 
    double a = ray.direction[0] * ray.direction[0] + ray.direction[1] * ray.direction[1] + ray.direction[2] * ray.direction[2];
    double b = 2.0f * (ray.direction[0] * oc[0] + 2.0 * ray.direction[1] * oc[1] + 2.0 * ray.direction[2] * oc[2]);
    double c = oc[0]*oc[0] + oc[1]*oc[1] + oc[2]*oc[2] - radius * radius;
    double discriminant = b*b - 4*a*c;
    if (discriminant < 0) {
        return false;
    } else {
        float temp1 = (-b - sqrt(discriminant)) / (2.0f * a);
        if (temp1 > 0) {
            t = temp1;
            return true;
        }
        float temp2 = (-b + sqrt(discriminant)) / (2.0f * a);
        if (temp2 > 0) {
            t = temp2;
            return true;
        }
    }
    return false;
}