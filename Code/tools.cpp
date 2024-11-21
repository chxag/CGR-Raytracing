#include <iostream>
#include <cmath>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include "tools.h"
#include <stdexcept>
#include <limits>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;

float pi = 3.14159265358979323846;

void Tools::readConfig(const std::string& filename)
{
    std::ifstream file(filename);
    json j;
    file >> j;

    // config.nbounces = j["nbounces"];
    rendermode = j["rendermode"];
    camera_type = j["camera"]["type"];
    width = j["camera"]["width"].get<int>();
    height = j["camera"]["height"].get<int>();
    position = j["camera"]["position"].get<std::vector<float>>();
    lookAt = j["camera"]["lookAt"].get<std::vector<float>>();
    upVector = j["camera"]["upVector"].get<std::vector<float>>();
    fov = j["camera"]["fov"].get<float>();
    exposure = j["camera"]["exposure"].get<float>();
    backgroundcolor = j["scene"]["backgroundcolor"].get<std::vector<float>>();
    
    for (const auto& shape : j["scene"]["shapes"]){
        if(shape["type"].get<std::string>() == "sphere") {
            std::vector<float> center = {shape["center"][0].get<float>(), shape["center"][1].get<float>(), shape["center"][2].get<float>()};
            float radius = shape["radius"].get<float>();
            spheres.emplace_back(center, radius);
        }
        if(shape["type"].get<std::string>() == "cylinder") {
            std::vector<float> center = {shape["center"][0].get<float>(), shape["center"][1].get<float>(), shape["center"][2].get<float>()};
            float radius = shape["radius"].get<float>();
            std::vector<float> axis = {shape["axis"][0].get<float>(), shape["axis"][1].get<float>(), shape["axis"][2].get<float>()};
            float height = shape["height"].get<float>();
            cylinders.emplace_back(center, radius, axis, height);
        }
        if(shape["type"].get<std::string>() == "triangle") {
            std::vector<float> v0 = {shape["v0"][0].get<float>(), shape["v0"][1].get<float>(), shape["v0"][2].get<float>()};
            std::vector<float> v1 = {shape["v1"][0].get<float>(), shape["v1"][1].get<float>(), shape["v1"][2].get<float>()};
            std::vector<float> v2 = {shape["v2"][0].get<float>(), shape["v2"][1].get<float>(), shape["v2"][2].get<float>()};
            triangles.emplace_back(v0, v1, v2);
        }
    }
};

void Tools::render(PPMWriter& ppmwriter) {

    std::vector<float> forward = {lookAt[0] - position[0], lookAt[1] - position[1], lookAt[2] - position[2]};
    std::vector<float> right = {upVector[1] * forward[2] - upVector[2] * forward[1],
                                upVector[2] * forward[0] - upVector[0] * forward[2],
                                upVector[0] * forward[1] - upVector[1] * forward[0]};
    std::vector<float> up = {forward[1] * right[2] - forward[2] * right[1],
                             forward[2] * right[0] - forward[0] * right[2],
                             forward[0] * right[1] - forward[1] * right[0]};
    
    for(float &f : forward)
    {
        f = f / sqrt(forward[0] * forward[0] + forward[1] * forward[1] + forward[2] * forward[2]);
    }

    for(float &r : right)
    {
        r = r / sqrt(right[0] * right[0] + right[1] * right[1] + right[2] * right[2]);
    }

    float aspectRatio = static_cast<float>(width) / height;
    float scale = tan(fov * 0.5 * pi / 180.0f);

    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x)
        {
            float u = (2 * (x + 0.5f) / width - 1) * aspectRatio * scale;
            float v = (1 - 2 * (y +0.5f) / height) * scale;

            std::vector<float> direction = {right[0] * u + up[0] * v + forward[0],
                                            right[1] * u + up[1] * v + forward[1],
                                            right[2] * u + up[2] * v + forward[2]};
            float direction_normalized = sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
            for(float &d : direction)
            {
                d = d / direction_normalized;
            }

            Ray ray(position, direction);

            float closestT = std::numeric_limits<float>::max();
            bool intersected = false; 
            std::vector<float> intersected_color = backgroundcolor;

            for (const auto& sphere: spheres){
                float t;
                if (sphere.intersectSphere(ray, t)){
                    if (t < closestT){
                        closestT = t;
                        intersected = true;
                        intersected_color = {1.0f, 0.0f, 0.0f};
                    }
                }
            }

            for (const auto& cylinder: cylinders){
                float t;
                if (cylinder.intersectCylinder(ray, t)){
                    if (t < closestT){
                        closestT = t;
                        intersected = true;
                        intersected_color = {1.0f, 0.0f, 0.0f};
                    }
                }
            }

            for (const auto& triangle: triangles){
                float t;
                if (triangle.intersectTriangle(ray, t)){
                    if (t < closestT){
                        closestT = t;
                        intersected = true;
                        intersected_color = {1.0f, 0.0f, 0.0f};
                    }
                }
            };

            if (intersected){
                ppmwriter.getPixelData(x, y, {static_cast<unsigned char>(intersected_color[0] * 255), static_cast<unsigned char>(intersected_color[1] * 255), static_cast<unsigned char>(intersected_color[2] * 255)});
            } else {
                ppmwriter.getPixelData(x, y, {static_cast<unsigned char>(backgroundcolor[0] * 255), static_cast<unsigned char>(backgroundcolor[1] * 255), static_cast<unsigned char>(backgroundcolor[2] * 255)});
            }
        }
    }
}