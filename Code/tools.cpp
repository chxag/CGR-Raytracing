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
#include "material.h"
#include "binary_shader.h"
#include "blinn_phong_shader.h"
#include "vector_utils.h"
#include "tone_mapping.h"
#include "shadow.h"

using json = nlohmann::json;

float pi = 3.14159265358979323846;

void Tools::readConfig(const std::string &filename)
{
    std::ifstream file(filename);
    json j;
    file >> j;

    nbounces = j["nbounces"];
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

    for (const auto &light : j["scene"]["lightsources"])
    {
        std::string light_type = light["type"].get<std::string>();
        std::vector<float> light_position = light["position"].get<std::vector<float>>();
        std::vector<float> intensity = light["intensity"].get<std::vector<float>>();

        lightsources.emplace_back(light_type, light_position, intensity);
    }

    for (const auto &shape : j["scene"]["shapes"])
    {
        if (shape["type"].get<std::string>() == "sphere")
        {
            std::vector<float> center = {shape["center"][0].get<float>(), shape["center"][1].get<float>(), shape["center"][2].get<float>()};
            float radius = shape["radius"].get<float>();
            float ks_coeffcient = shape["material"]["ks"].get<float>();
            float kd_coeffcient = shape["material"]["kd"].get<float>();
            float specular_exponent = shape["material"]["specularexponent"].get<float>();
            std::vector<float> diffuse_color = shape["material"]["diffusecolor"].get<std::vector<float>>();
            std::vector<float> specular_color = shape["material"]["specularcolor"].get<std::vector<float>>();
            bool is_reflective = shape["material"]["isreflective"].get<bool>();
            float reflectivity = shape["material"]["reflectivity"].get<float>();
            bool is_refractive = shape["material"]["isrefractive"].get<bool>();
            float refractive_index = shape["material"]["refractiveindex"].get<float>();

            Material material(ks_coeffcient, kd_coeffcient, specular_exponent, diffuse_color, specular_color, is_reflective, reflectivity, is_refractive, refractive_index);

            spheres.emplace_back(center, radius, material);
        }
        if (shape["type"].get<std::string>() == "cylinder")
        {
            std::vector<float> center = {shape["center"][0].get<float>(), shape["center"][1].get<float>(), shape["center"][2].get<float>()};
            float radius = shape["radius"].get<float>();
            std::vector<float> axis = {shape["axis"][0].get<float>(), shape["axis"][1].get<float>(), shape["axis"][2].get<float>()};
            float height = shape["height"].get<float>();
            float ks_coeffcient = shape["material"]["ks"].get<float>();
            float kd_coeffcient = shape["material"]["kd"].get<float>();
            float specular_exponent = shape["material"]["specularexponent"].get<float>();
            std::vector<float> diffuse_color = shape["material"]["diffusecolor"].get<std::vector<float>>();
            std::vector<float> specular_color = shape["material"]["specularcolor"].get<std::vector<float>>();
            bool is_reflective = shape["material"]["isreflective"].get<bool>();
            float reflectivity = shape["material"]["reflectivity"].get<float>();
            bool is_refractive = shape["material"]["isrefractive"].get<bool>();
            float refractive_index = shape["material"]["refractiveindex"].get<float>();

            Material material(ks_coeffcient, kd_coeffcient, specular_exponent, diffuse_color, specular_color, is_reflective, reflectivity, is_refractive, refractive_index);

            cylinders.emplace_back(center, radius, axis, height, material);
        }
        if (shape["type"].get<std::string>() == "triangle")
        {
            std::vector<float> v0 = {shape["v0"][0].get<float>(), shape["v0"][1].get<float>(), shape["v0"][2].get<float>()};
            std::vector<float> v1 = {shape["v1"][0].get<float>(), shape["v1"][1].get<float>(), shape["v1"][2].get<float>()};
            std::vector<float> v2 = {shape["v2"][0].get<float>(), shape["v2"][1].get<float>(), shape["v2"][2].get<float>()};
            float ks_coeffcient = shape["material"]["ks"].get<float>();
            float kd_coeffcient = shape["material"]["kd"].get<float>();
            float specular_exponent = shape["material"]["specularexponent"].get<float>();
            std::vector<float> specular_color = shape["material"]["specularcolor"].get<std::vector<float>>();
            std::vector<float> diffuse_color = shape["material"]["diffusecolor"].get<std::vector<float>>();
            bool is_reflective = shape["material"]["isreflective"].get<bool>();
            float reflectivity = shape["material"]["reflectivity"].get<float>();
            bool is_refractive = shape["material"]["isrefractive"].get<bool>();
            float refractive_index = shape["material"]["refractiveindex"].get<float>();

            Material material(ks_coeffcient, kd_coeffcient, specular_exponent, diffuse_color, specular_color, is_reflective, reflectivity, is_refractive, refractive_index);

            triangles.emplace_back(v0, v1, v2, material);
        }
    }
};

std::vector<float> Tools::traceRay(const Ray &ray, int depth, const std::string &rendermode)
{

    if (depth > nbounces)
    {
        return backgroundcolor;
    }

    std::vector intersection_color = backgroundcolor;

    if (rendermode == "phong")
    {
        ShaderResult result = BlinnPhongShader::intersectionTests(ray, spheres, cylinders, triangles, backgroundcolor);
        intersection_color = result.color;
        bool intersected = result.intersected;
        std::vector<float> intersectionPoint = result.intersection_point;
        Material intersectedMaterial = result.intersected_material;
        std::vector<float> normal = result.normal;

        if (intersected)
        {
            std::vector<float> viewDir = {
                position[0] - intersectionPoint[0],
                position[1] - intersectionPoint[1],
                position[2] - intersectionPoint[2]};
            normalize(viewDir);
            intersection_color = BlinnPhongShader::calculateColor(intersectionPoint, normal, viewDir, intersectedMaterial, lightsources, spheres, cylinders, triangles);

            if (intersectedMaterial.is_reflective)
            {
                std::vector<float> reflectionDir = reflect(ray.direction, normal);
                normalize(reflectionDir);
                std::vector<float> temp = {intersectionPoint[0] + 0.001f * reflectionDir[0], + intersectionPoint[1] + 0.001f * reflectionDir[1],  intersectionPoint[2] + 0.001f * reflectionDir[2]};
                Ray reflectionRay(temp, reflectionDir);

                std::vector<float> reflectionColor = traceRay(reflectionRay, depth + 1, rendermode);

                for (auto& light  : lightsources){

                    bool inShadow = Shadow::isInShadow(intersectionPoint, light, spheres, cylinders, triangles);
                    if (inShadow)
                    {
                        continue;
                    }
                }

                intersection_color[0] = (1 - intersectedMaterial.reflectivity) * intersection_color[0] + intersectedMaterial.reflectivity * reflectionColor[0];
                intersection_color[1] = (1 - intersectedMaterial.reflectivity) * intersection_color[1] + intersectedMaterial.reflectivity * reflectionColor[1];
                intersection_color[2] = (1 - intersectedMaterial.reflectivity) * intersection_color[2] + intersectedMaterial.reflectivity * reflectionColor[2];
                
            }
        }
    }

    if (rendermode == "binary")
    {
        ShaderResult result = BinaryShader::calculateColor(ray, spheres, cylinders, triangles, backgroundcolor);
        intersection_color = result.color;
    }

    return intersection_color;
}

void Tools::render(PPMWriter &ppmwriter, std::string rendermode)
{

    std::vector<float> forward = {lookAt[0] - position[0], lookAt[1] - position[1], lookAt[2] - position[2]};
    normalize(forward);
    std::vector<float> right = {upVector[1] * forward[2] - upVector[2] * forward[1],
                                upVector[2] * forward[0] - upVector[0] * forward[2],
                                upVector[0] * forward[1] - upVector[1] * forward[0]};
    normalize(right);
    std::vector<float> up = {forward[1] * right[2] - forward[2] * right[1],
                             forward[2] * right[0] - forward[0] * right[2],
                             forward[0] * right[1] - forward[1] * right[0]};

    normalize(up);
    float aspectRatio = static_cast<float>(width) / height;
    float scale = tan(fov * 0.5 * pi / 180.0f);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float u = (2 * (x + 0.5f) / width - 1) * aspectRatio * scale;
            float v = (1 - 2 * (y + 0.5f) / height) * scale;

            std::vector<float> direction = {right[0] * u + up[0] * v + forward[0],
                                            right[1] * u + up[1] * v + forward[1],
                                            right[2] * u + up[2] * v + forward[2]};
            normalize(direction);
            Ray ray(position, direction);

            std::vector<float> intersection_color = traceRay(ray, 0, rendermode);

            max_value = std::max({max_value, intersection_color[0], intersection_color[1], intersection_color[2]});

            ppmwriter.getPixelData(x, y, {
                static_cast<unsigned char>(intersection_color[0] * 255),
                static_cast<unsigned char>(intersection_color[1] * 255),
                static_cast<unsigned char>(intersection_color[2] * 255)});
        }

        // for (int y = 0; y < height; ++y)
        // {
        //     for (int x = 0; x < width; ++x)
        //     {
        //         float u = (2 * (x + 0.5f) / width - 1) * aspectRatio * scale;
        //         float v = (1 - 2 * (y + 0.5f) / height) * scale;

        //         std::vector<float> direction = {right[0] * u + up[0] * v + forward[0],
        //                                         right[1] * u + up[1] * v + forward[1],
        //                                         right[2] * u + up[2] * v + forward[2]};
        //         normalize(direction);
        //         Ray ray(position, direction);

        //         std::vector<float> intersection_color = traceRay(ray, 0, rendermode);

        //         std::vector<float> tone_mapped_color = linearToneMapping(intersection_color, max_value);

        //         ppmwriter.getPixelData(x,y, {
        //             static_cast<unsigned char>(tone_mapped_color[0] * 255),
        //             static_cast<unsigned char>(tone_mapped_color[1] * 255),
        //             static_cast<unsigned char>(tone_mapped_color[2] * 255)
        //         });
        //     }
        // }
    }
}