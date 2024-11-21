#include "cylinder.h"
#include <cmath>

Cylinder::Cylinder(const std::vector<float>& center, float radius, const std::vector<float>& axis, float height) {
    this->center = center;
    this->radius = radius;
    this->height = height;
    float axis_length = sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
    this->axis = {axis[0] / axis_length, axis[1] / axis_length, axis[2] / axis_length};
}

bool Cylinder::intersectCylinder(const Ray& ray, float& t) const{
   std::vector<float> oc ={ray.origin[0] - center[0], ray.origin[1] - center[1], ray.origin[2] - center[2]};
   const float epsilon = 1e-6;

   float d_perp = ray.direction[0] * axis[0] + ray.direction[1] * axis[1] + ray.direction[2] * axis[2];
   float oc_perp = oc[0] * axis[0] + oc[1] * axis[1] + oc[2] * axis[2];

    float a = 1.0f - d_perp * d_perp;
    float b = 2.0f * (oc[0] * ray.direction[0] + oc[1] * ray.direction[1] + oc[2] * ray.direction[2] - oc_perp * d_perp);
    float c = oc[0] * oc[0] + oc[1] * oc[1] + oc[2] * oc[2] - oc_perp * oc_perp - radius * radius;
    double discriminant = b*b - 4*a*c;


    if (discriminant < 0) {
        return false;
    };

    auto valid_hit = [&](float t) -> bool{
        if (t < epsilon) {
            return false;
        }
        std::vector<float> hit = {ray.origin[0] + t * ray.direction[0], ray.origin[1] + t * ray.direction[1], ray.origin[2] + t * ray.direction[2]};
        float proj_height = (hit[0] - center[0]) * axis[0] + (hit[1] - center[1]) * axis[1] + (hit[2] - center[2]) * axis[2];
        return proj_height >= 0 && proj_height <= height;
    };
    
    bool valid_hit_if = valid_hit((-b - sqrt(discriminant)) / (2.0f * a));
    bool valid_hit_else = valid_hit((-b + sqrt(discriminant)) / (2.0f * a));
    if (valid_hit_if && (!valid_hit_else || (-b - sqrt(discriminant)) / (2.0f * a) < (-b + sqrt(discriminant)) / (2.0f * a))) {
        t = (-b - sqrt(discriminant)) / (2.0f * a);
        return true;
    } else if (valid_hit_else) {
        t = (-b + sqrt(discriminant)) / (2.0f * a);
        return true;
    } else {
        return false;
    }
}