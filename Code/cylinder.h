#ifndef CYLINDER_H
#define CYLINDER_H

#include "ray.h"
#include "raytracer.h"
#include <vector>

class Cylinder {
    public:

        Cylinder(const std::vector<float>& center, float radius, const std::vector<float>& axis, float height);
        bool intersectCylinder(const Ray& ray, float& t) const;
    
    private:
        std::vector<float> center;
        float radius;
        std::vector<float> axis;
        float height;
};

#endif