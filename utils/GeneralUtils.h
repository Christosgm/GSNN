#include <glm/glm.hpp>
#include <vector>
#include <iostream>


glm::mat3 build_rotation(glm::vec4 q)
{
    float r = q.x;
    float x = q.y;
    float y = q.z;
    float z = q.w;

    return glm::mat3(1.0 - 2.0 * (y*y + z*z), 2.0 * (x*y - r*z),       2.0 * (x*z + r*y),
                     2.0 * (x*y + r*z),       1.0 - 2.0 * (x*x + z*z), 2.0 * (y*z - r*x), 
                     2.0 * (x*z - r*y),       2.0 * (y*z + r*x),       1.0 - 2.0 * (x*x + y*y)
                    );
}

glm::mat3 build_scaling_rotation(glm::vec3 s, glm::vec4 q)
{
    glm::mat3 L = glm::mat3(0.0);
    glm::mat3 R = build_rotation(q);

    L[0][0] = s.x;
    L[1][1] = s.y;
    L[2][2] = s.z;

    L = L * R;

    return L;
}

std::vector<float> strip_symmetric(glm::mat3 L)
{
    std::vector<float> cov3d = std::vector<float>();
    cov3d.push_back(L[0][0]);
    cov3d.push_back(L[0][1]);
    cov3d.push_back(L[0][2]);
    cov3d.push_back(L[1][1]);
    cov3d.push_back(L[1][2]);
    cov3d.push_back(L[2][2]);
    return cov3d;
}

std::vector<float> build_covariance3D(glm::vec3 s, glm::vec4 q)
{
    glm::mat3 L = build_scaling_rotation(s, q);
    glm::mat3 actual_covariance = glm::transpose(L) * L;
    return strip_symmetric(actual_covariance);
}