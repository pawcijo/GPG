#pragma once

#include <glm/glm.hpp>

struct Line
{
    glm::vec3 start;
    glm::vec3 end;

    inline Line() {}
    inline Line(const glm::vec3 &s, const glm::vec3 &e) : start(s), end(e) {}
};