#pragma once

#include <vector>

#include "AABB.h"

enum RigidBodyType
{

    base = 0,
    Point,
    Box,
    Sphere,
};

//TODO MOVE TO another file
static const  glm::vec3 GRAVITY_CONST(0.0f, -9.82f, 0.0f);

class Rigidbody
{

public:
    RigidBodyType type;

    inline Rigidbody()
    {
        type = base;
    };

    virtual inline ~Rigidbody() {}
    virtual inline void ApplyForces() {}

    // TODO REPLACE WITH OBB when written
    virtual inline void SolveConstraints(const std::vector<AABB> &constraints) {}

    inline bool HasVolume()
    {
        //TODO can be also sphere 
        return type = RigidBodyType::Box;
    }
};
