#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <vector>
#include "sphere.h"
#include "cylinder.h"
#include "triangle.h"
#include "ppmWriter.h"
#include "light.h"

class Tools

{
public:
    void readConfig(const std::string &filename);
    void render(PPMWriter& ppmwriter, std::string rendermode);
    std::vector<float> traceRay(const Ray& ray, int depth, const std::string& rendermode, std::vector<Sphere>& spheres, const std::vector<Cylinder>& cylinders, const std::vector<Triangle>& triangles, const std::vector<Light>& lightsources, const std::vector<float>& backgroundcolor);
private:

    std::string rendermode;

    std::string camera_type;
    int width;
    int height;
    std::vector<float>  position;
    std::vector<float>  lookAt;
    std::vector<float>  upVector;
    float fov;
    float exposure;

    std::vector<float>  backgroundcolor;
    std::vector<Sphere> spheres;
    std::vector<Cylinder> cylinders;
    std::vector<Triangle> triangles;
    std::vector<Light> lightsources;


};

#endif