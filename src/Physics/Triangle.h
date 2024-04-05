#pragma once
#include <glm/glm.hpp>

struct Triangle
{

    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;

    inline Triangle() {}
    inline Triangle(const glm::vec3 &_p1, const glm::vec3 &_p2, const glm::vec3 &_p3) : a(_p1), b(_p2), c(_p3) {}
};