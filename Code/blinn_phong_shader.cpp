#include "blinn_phong_shader.h"
#include <cmath>
#include <algorithm>
#include "vector_utils.h"
#include "shadow.h"

std::vector<float> BlinnPhongShader::calculateColor(const std::vector<float> &intersectionPoint, const std::vector<float> &normal, const std::vector<float> &viewDir, const Material &material, const std::vector<Light> &lights, const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles)
{
    std::vector<float> color = {0.0f, 0.0f, 0.0f};

    // Ambient light contribution
    float ambient_intensity = 0.4f;
    std::vector<float> ambient_light = {
        ambient_intensity * material.diffuse_color[0],
        ambient_intensity * material.diffuse_color[1],
        ambient_intensity * material.diffuse_color[2]
    };

    color[0] += ambient_light[0];
    color[1] += ambient_light[1];
    color[2] += ambient_light[2];

    for (const auto &light : lights)
    {
        bool inShadow = Shadow::isInShadow(intersectionPoint, light, spheres, cylinders, triangles);
        if (inShadow)
        {
            continue;
        }
        // Light direction
        std::vector<float> lightDir = {
            light.light_position[0] - intersectionPoint[0],
            light.light_position[1] - intersectionPoint[1],
            light.light_position[2] - intersectionPoint[2]
        };
        normalize(lightDir);

        // Diffuse contribution
        float NdotL = normal[0] * lightDir[0] +
                      normal[1] * lightDir[1] +
                      normal[2] * lightDir[2];
        float diff = material.kd_coeffcient * std::max(NdotL, 0.0f);
        std::vector<float> diffuse = {
            diff * material.diffuse_color[0] * light.intensity[0],
            diff * material.diffuse_color[1] * light.intensity[1],
            diff * material.diffuse_color[2] * light.intensity[2]
        };

        // Specular contribution
        std::vector<float> halfwayDir = {
            viewDir[0] + lightDir[0],
            viewDir[1] + lightDir[1],
            viewDir[2] + lightDir[2]
        };
        normalize(halfwayDir);

        float NdotH = normal[0] * halfwayDir[0] +
                      normal[1] * halfwayDir[1] +
                      normal[2] * halfwayDir[2];
        float specularFactor = pow(std::max(NdotH, 0.0f), material.specular_exponent);
        std::vector<float> specular = {
            specularFactor * material.specular_color[0] * light.intensity[0] * material.ks_coeffcient,
            specularFactor * material.specular_color[1] * light.intensity[1] * material.ks_coeffcient,
            specularFactor * material.specular_color[2] * light.intensity[2] * material.ks_coeffcient
        };

        // Sum up diffuse and specular contributions
        color[0] += diffuse[0] + specular[0];
        color[1] += diffuse[1] + specular[1];
        color[2] += diffuse[2] + specular[2];
    }

    // Clamp color values to [0, 1]
    color[0] = std::min(color[0], 1.0f);
    color[1] = std::min(color[1], 1.0f);
    color[2] = std::min(color[2], 1.0f);


    return color;
};

ShaderResult BlinnPhongShader::intersectionTests(const Ray &ray, const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles, std::vector<float> &backgroundcolor){
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
                        
                        // Calculate point relative to cylinder center
                        std::vector<float> pc = {
                            intersectionPoint[0] - cylinder.center[0],
                            intersectionPoint[1] - cylinder.center[1],
                            intersectionPoint[2] - cylinder.center[2]
                        };
                        
                        // Project point onto axis
                        float dot = pc[0] * cylinder.axis[0] + pc[1] * cylinder.axis[1] + pc[2] * cylinder.axis[2];
                        
                        // Calculate normal as point minus its projection on axis
                        normal = {
                            pc[0] - dot * cylinder.axis[0],
                            pc[1] - dot * cylinder.axis[1],
                            pc[2] - dot * cylinder.axis[2]
                        };
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
                
                return {intersected_color, intersected, intersectionPoint, intersectedMaterial, normal};
}