#pragma once

#include "Obb.h"
#include <vector>
#include <glm/glm.hpp>

class PhysicsManager
{
private:
    glm::vec3 gravity;

public:
    std::vector<Obb> obbs;
    PhysicsManager(float gravityX, float gravityY, float gravityZ);

    void addOBB(const Obb &obb);
    void update(float deltaTime);
};
