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
    void normalize(std::vector<float> &vec);
    std::vector<float> calculateBlinnPhong(const std::vector<float>& intersectionPoint, const std::vector<float> normal, const std::vector<float>viewDir, const Material& material, const std::vector<Light>& lights);
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