#ifndef VECTOR_UTILS_H
#define VECTOR_UTILS_H

#include <vector>
#include <cmath>

// Function to normalize a vector
void normalize(std::vector<float> &vec);
std::vector<float> reflect(const std::vector<float> &incident, const std::vector<float> &normal);

#endif
