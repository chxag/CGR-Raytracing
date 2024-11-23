#include "vector_utils.h"

void normalize(std::vector<float> &vec)
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
