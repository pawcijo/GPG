#pragma once

#include <glm/glm.hpp>

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;

    inline Ray() : direction(0.0f, 0.0f, 1.0f) {}
    inline Ray(const glm::vec3 &o, const glm::vec3 &d) : origin(o), direction(d)
    {
        NormalizeDirection();
    }
    inline void NormalizeDirection()
    {
        glm::normalize(direction);
    }
};