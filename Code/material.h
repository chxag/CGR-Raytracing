#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>

struct Material
{
    float ks_coeffcient;
    float kd_coeffcient;
    float specular_exponent;
    std::vector<float> diffuse_color;
    std::vector<float> specular_color;
    bool is_reflective;
    float reflectivity;
    bool is_refractive;
    float refractive_index;

    Material() : ks_coeffcient(0.0f), kd_coeffcient(0.0f), specular_exponent(0.0f), diffuse_color({0.0f, 0.0f, 0.0f}), specular_color({0.0f, 0.0f, 0.0f}), is_reflective(false), reflectivity(0.0f), is_refractive(false), refractive_index(0.0f) {}

    Material(float ks_coeffcient, float kd_coeffcient, float specular_exponent, std::vector<float> diffuse_color, std::vector<float> specular_color, bool is_reflective, float reflectivity, bool is_refractive, float refractive_index)
        : ks_coeffcient(ks_coeffcient), kd_coeffcient(kd_coeffcient), specular_exponent(specular_exponent), diffuse_color(diffuse_color), specular_color(specular_color), is_reflective(is_reflective), reflectivity(reflectivity), is_refractive(is_refractive), refractive_index(refractive_index) {}
};

#endif