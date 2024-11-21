#ifndef SPHERE_H
#define SPHERE_H

#include "ray.h"
#include "raytracer.h"
#include <vector>

class Sphere{
    public:
        Sphere(std::vector<float> center, float radius);
        bool intersectSphere(const Ray& ray, float& t) const;
    private:
        std::vector<float> center;
        float radius;

};



#endif