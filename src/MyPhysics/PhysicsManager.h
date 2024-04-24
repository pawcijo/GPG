#pragma once

#include "Obb.h"
#include <vector>
#include <glm/glm.hpp>
#include <unordered_set>

#include <cstdio>

#include <SimpleShape/Box.h>



class PhysicsManager
{
private:
    glm::vec3 gravity;

public:
    std::vector<Obb *> obbs;
    std::unordered_set<std::pair<Obb *, Obb *>, PairObbHash> collisionMap;
    PhysicsManager(float gravityX, float gravityY, float gravityZ);

    void addOBB(Obb *obb);
    void update(float deltaTime);

    void ClearCollisonMap() { collisionMap.clear(); }

    bool intersects(Obb *one, Obb *other);
    void resolveCollision(Obb *one, Obb *other);

    void applyPositionCorrection(Obb *obb);
};
