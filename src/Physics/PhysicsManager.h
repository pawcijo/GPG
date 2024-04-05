#pragma once

#include <vector>
#include <AABB.h>


void ResetCollisionResult(CollisionResult *result);

class PhysicsManager
{
    float LinearProjectionPercent; // [0.2 to 0.8], Smaller = less jitter / more penetration
    float PenetrationSlack;        // [0.01 to 0.1],  Samller = more accurate
    int ImpulseIteration;

    std::vector<AABB> colliders1;
    std::vector<AABB> colliders2;

    std::vector<CollisionResult> results;

public:
    PhysicsManager();

    void Update(float deltaTime);
};
