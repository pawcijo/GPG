#pragma once

#include <glm/glm.hpp>

struct Vertex
{
    const static unsigned int float_number  = 5; // 3 (position) + 2 (texCords)  
    glm::vec3 position;
    glm::vec2 texCord;

    Vertex(glm::vec3 aPosition, glm::vec2 aTexCords)
    {
        position = aPosition;
        texCord = aTexCords;
    }

    Vertex(float p1, float p2, float p3, float p4, float p5)
    {
        position = glm::vec3(p1, p2, p3);
        texCord = glm::vec2(p4, p5);
    }
};