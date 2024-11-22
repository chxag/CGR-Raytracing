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

using json = nlohmann::json;

float pi = 3.14159265358979323846;

void Tools::readConfig(const std::string &filename)
{
    std::ifstream file(filename);
    json j;
    file >> j;

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
            bool is_refractive = shape["material"]["isrefractive"].get<bool>();
            float refractive_index = shape["material"]["refractiveindex"].get<float>();

            Material material(ks_coeffcient, kd_coeffcient, specular_exponent, diffuse_color, specular_color, is_reflective, is_refractive, refractive_index);

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
            bool is_refractive = shape["material"]["isrefractive"].get<bool>();
            float refractive_index = shape["material"]["refractiveindex"].get<float>();

            Material material(ks_coeffcient, kd_coeffcient, specular_exponent, diffuse_color, specular_color, is_reflective, is_refractive, refractive_index);

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
            bool is_refractive = shape["material"]["isrefractive"].get<bool>();
            float refractive_index = shape["material"]["refractiveindex"].get<float>();

            Material material(ks_coeffcient, kd_coeffcient, specular_exponent, diffuse_color, specular_color, is_reflective, is_refractive, refractive_index);

            triangles.emplace_back(v0, v1, v2, material);
        }
    }
};

void Tools::normalize(std::vector<float> &vec)
{
    float length = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    if (length > 1e-6)
    {
        for (float &v : vec)
        {
            v = v / length;
        }
    }
}

std::vector<float> Tools::calculateBlinnPhong(const std::vector<float> &intersectionPoint, const std::vector<float> normal, const std::vector<float> viewDir, const Material &material, const std::vector<Light> &lights)
{
    std::vector<float> color = {0.0f, 0.0f, 0.0f};

    float ambient_intensity = 0.5f;

    std::vector<float> ambient_light = {ambient_intensity * material.diffuse_color[0],
                                        ambient_intensity * material.diffuse_color[1],
                                        ambient_intensity * material.diffuse_color[2]};

    color[0] += ambient_light[0];
    color[1] += ambient_light[1];
    color[2] += ambient_light[2];

    for (const auto &light : lights)
    {

        std::vector<float> lightDir = {light.light_position[0] - intersectionPoint[0], light.light_position[1] - intersectionPoint[1], light.light_position[2] - intersectionPoint[2]};
        normalize(lightDir);

        float NdotL = normal[0] * lightDir[0] +
                      normal[1] * lightDir[1] +
                      normal[2] * lightDir[2];

        float diff = material.kd_coeffcient * std::max(NdotL, 0.0f);

        std::vector<float> diffuse = {diff * material.diffuse_color[0] * light.intensity[0] * material.kd_coeffcient,
                                      diff * material.diffuse_color[1] * light.intensity[1] * material.kd_coeffcient,
                                      diff * material.diffuse_color[2] * light.intensity[2] * material.kd_coeffcient};

        std::vector<float> halfwayDir = {viewDir[0] + lightDir[0], viewDir[1] + lightDir[1], viewDir[2] + lightDir[2]};
        normalize(halfwayDir);

        float NdotH = normal[0] * halfwayDir[0] +
                      normal[1] * halfwayDir[1] +
                      normal[2] * halfwayDir[2];
        float specularFactor = pow(std::max(NdotH, 0.0f), material.specular_exponent);

        std::vector<float> specular = {specularFactor * material.specular_color[0] * light.intensity[0] * material.ks_coeffcient,
                                       specularFactor * material.specular_color[1] * light.intensity[1] * material.ks_coeffcient,
                                       specularFactor * material.specular_color[2] * light.intensity[2] * material.ks_coeffcient};

        color[0] += diffuse[0] + specular[0];
        color[1] += diffuse[1] + specular[1];
        color[2] += diffuse[2] + specular[2];
    }

    color[0] = std::min(color[0], 1.0f);
    color[1] = std::min(color[1], 1.0f);
    color[2] = std::min(color[2], 1.0f);

    return color;
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

            if (rendermode == "phong")
            {
                float closestT = std::numeric_limits<float>::max();
                bool intersected = false;
                Material intersectedMaterial;
                std::vector<float> intersected_color = backgroundcolor;

                std::vector<float> intersectionPoint;
                std::vector<float> normal;

                for (const auto &sphere : spheres)
                {
                    float t;
                    if (sphere.intersectSphere(ray, t) && t < closestT)
                    {
                        closestT = t;
                        intersected = true;
                        intersectedMaterial = sphere.material;
                        intersectionPoint = {ray.origin[0] + t * ray.direction[0],
                                             ray.origin[1] + t * ray.direction[1],
                                             ray.origin[2] + t * ray.direction[2]};
                        normal = {intersectionPoint[0] - sphere.center[0],
                                  intersectionPoint[1] - sphere.center[1],
                                  intersectionPoint[2] - sphere.center[2]};
                        normalize(normal);
                    }
                }

                for (const auto &cylinder : cylinders)
                {
                    float t;
                    if (cylinder.intersectCylinder(ray, t) && t < closestT)
                    {
                        closestT = t;
                        intersected = true;
                        intersectedMaterial = cylinder.material;
                        intersectionPoint = {ray.origin[0] + t * ray.direction[0],
                                             ray.origin[1] + t * ray.direction[1],
                                             ray.origin[2] + t * ray.direction[2]};
                        normal = {intersectionPoint[0] - cylinder.center[0],
                                  intersectionPoint[1] - cylinder.center[1],
                                  intersectionPoint[2] - cylinder.center[2]};
                        normalize(normal);
                    }
                }

                for (const auto &triangle : triangles)
                {
                    float t;
                    if (triangle.intersectTriangle(ray, t) && t < closestT)
                    {
                        closestT = t;
                        intersected = true;
                        intersectedMaterial = triangle.material;
                        intersectionPoint = {ray.origin[0] + t * ray.direction[0],
                                             ray.origin[1] + t * ray.direction[1],
                                             ray.origin[2] + t * ray.direction[2]};
                        std::vector<float> edge1 = {triangle.v1[0] - triangle.v0[0],
                                                    triangle.v1[1] - triangle.v0[1],
                                                    triangle.v1[2] - triangle.v0[2]};
                        std::vector<float> edge2 = {triangle.v2[0] - triangle.v0[0],
                                                    triangle.v2[1] - triangle.v0[1],
                                                    triangle.v2[2] - triangle.v0[2]};
                        normal = {edge1[1] * edge2[2] - edge1[2] * edge2[1],
                                  edge1[2] * edge2[0] - edge1[0] * edge2[2],
                                  edge1[0] * edge2[1] - edge1[1] * edge2[0]};
                        normalize(normal);
                    }
                }

                if (intersected)
                {
                    std::vector<float> viewDir = {
                        position[0] - intersectionPoint[0],
                        position[1] - intersectionPoint[1],
                        position[2] - intersectionPoint[2]};
                    normalize(viewDir);

                    intersected_color = calculateBlinnPhong(intersectionPoint, normal, viewDir, intersectedMaterial, lightsources);
                    ppmwriter.getPixelData(x, y, {static_cast<unsigned char>(intersected_color[0] * 255), static_cast<unsigned char>(intersected_color[1] * 255), static_cast<unsigned char>(intersected_color[2] * 255)});
                }
                else
                {
                    ppmwriter.getPixelData(x, y, {static_cast<unsigned char>(backgroundcolor[0] * 255), static_cast<unsigned char>(backgroundcolor[1] * 255), static_cast<unsigned char>(backgroundcolor[2] * 255)});
                }
            }

            if (rendermode == "binary")
            {
                float closestT = std::numeric_limits<float>::max();
                bool intersected = false;
                std::vector<float> intersected_color = backgroundcolor;

                for (const auto &sphere : spheres)
                {
                    float t;
                    if (sphere.intersectSphere(ray, t) && t < closestT)
                    {
                        closestT = t;
                        intersected = true;
                        intersected_color = {1.0f, 0.0f, 0.0f};
                    }
                }

                for (const auto &cylinder : cylinders)
                {
                    float t;
                    if (cylinder.intersectCylinder(ray, t) && t < closestT)
                    {
                        closestT = t;
                        intersected = true;
                        intersected_color = {1.0f, 0.0f, 0.0f};
                    }
                }

                for (const auto &triangle : triangles)
                {
                    float t;
                    if (triangle.intersectTriangle(ray, t) && t < closestT)
                    {
                        closestT = t;
                        intersected = true;
                        intersected_color = {1.0f, 0.0f, 0.0f};
                    }
                };

                if (intersected)
                {
                    ppmwriter.getPixelData(x, y, {static_cast<unsigned char>(intersected_color[0] * 255), static_cast<unsigned char>(intersected_color[1] * 255), static_cast<unsigned char>(intersected_color[2] * 255)});
                }
                else
                {
                    ppmwriter.getPixelData(x, y, {static_cast<unsigned char>(backgroundcolor[0] * 255), static_cast<unsigned char>(backgroundcolor[1] * 255), static_cast<unsigned char>(backgroundcolor[2] * 255)});
                }
            }
        }
    }
}