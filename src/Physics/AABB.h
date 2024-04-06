#pragma once

#include <glm/glm.hpp>
#include <vector>

class AABB
{

public:
    glm::vec3 position;
    glm::vec3 size; // half_size

    inline AABB() : size(1, 1, 1), position(0, 0, 0) {}
    inline AABB(const glm::vec3 &aPoint, const glm::vec3 &aSize) : position(aPoint),
                                                                   size(aSize) {}
};
