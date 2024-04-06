#pragma once

#include <glm/glm.hpp>

class OBB
{
public:
    glm::vec3 position;
    glm::vec3 size; // HALF SIZE!
    glm::mat3 orientation;

    inline OBB() : size(1, 1, 1),position(1,1,1),orientation(glm::mat3(1)) {}
    inline OBB(const glm::vec3 &p, const glm::vec3 &s) : position(p), size(s),orientation(glm::mat3(1))  {}
    inline OBB(const glm::vec3 &p, const glm::vec3 &s, const glm::mat3 &o) : position(p), size(s), orientation(o) {}
};
