#pragma once

#include <vector>

#include "AABB.h"

enum RigidBodyType
{

    EBase = 0,
    EPoint,
    EBox,
    ESphere,
};

//TODO MOVE TO another file
static const  glm::vec3 GRAVITY_CONST(0.0f, -9.82f, 0.0f);

class Rigidbody
{

public:
    RigidBodyType type;

    inline Rigidbody()
    {
        type = EBase;
    };

    virtual inline ~Rigidbody() {}
    virtual inline void ApplyForces() {}


    inline bool HasVolume()
    {
        //TODO can be also sphere 
        return type = RigidBodyType::EBox;
    }
};
