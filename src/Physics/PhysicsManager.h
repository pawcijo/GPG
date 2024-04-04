#pragma once

#include <vector>
#include <AABB.h>

typedef struct CollisionResult
{
    bool colliding;
    glm::vec3 normal;
    float depth;
    std::vector<glm::vec3> contacts;
};

void ResetCollisionResult(CollisionResult* result);

class PhysicsManager
{

    std::vector<AABB> colliders1;
    std::vector<AABB> colliders2;

    std::vector<CollisionResult> results;

public:
    PhysicsManager();

    void Update(float deltaTime);
};
