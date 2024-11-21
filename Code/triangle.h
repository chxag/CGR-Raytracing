#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "ray.h"
#include "raytracer.h"
#include <vector>

class Triangle {
    public:

        Triangle(const std::vector<float>& v0, const std::vector<float>& v1, const std::vector<float>& v2);
        bool intersectTriangle(const Ray& ray, float& t) const;
    
    private:
        std::vector<float> v0;
        std::vector<float> v1;
        std::vector<float> v2;
};
#endif