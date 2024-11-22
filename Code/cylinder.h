#ifndef CYLINDER_H
#define CYLINDER_H

#include "ray.h"
#include <vector>

class Cylinder {
    public:

        Cylinder(const std::vector<float>& center, float radius, const std::vector<float>& axis, float height);
        bool intersectCylinder(const Ray& ray, float& t) const;
        float find_root(const Ray& ray) const;
        bool check_cap_intersection(const Ray& ray, float& t, const std::vector<float>& cap_center, const std::vector<float>& cap_normal) const;
    
    private:
        std::vector<float> center;
        float radius;
        std::vector<float> axis;
        float height;

};

#endif