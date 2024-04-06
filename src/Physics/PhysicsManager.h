#pragma once

#include <vector>
#include "Common.h"
#include "Rigidbody.h"

void ResetCollisionResult(CollisionResult *result);

class PhysicsManager
{

public:
    std::vector<Rigidbody *> bodies;
    std::vector<OBB> constraints;

    std::vector<Rigidbody *> colliders1;
    std::vector<Rigidbody *> colliders2;
    std::vector<CollisionResult> results;

public:
    bool DoLinearProjection;

    float LinearProjectionPercent; // [0.2 to 0.8], Smaller = less jitter / more penetration
    float PenetrationSlack;        // [0.01 to 0.1],  Samller = more accurate
    int ImpulseIteration;

    PhysicsManager();

    void AddRigidbody(Rigidbody *body);
    void AddConstraint(const OBB &constraint);

    void Update(float deltaTime);
    void ClearRigidbodys();
    void ClearConstraints();
};
