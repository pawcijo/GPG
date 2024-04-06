#pragma once

#include <vector>

#include "AABB.h"
#include "OBB.h"

enum RigidBodyType
{

    EBase = 0,
    EPoint,
    ESphere,
    EBox,
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
    virtual inline void Update(float deltaTime) { }
    virtual inline void ApplyForces() {}
    virtual inline void SolveConstraints(const std::vector<OBB>& constraints) { }


    inline bool HasVolume()
    {
        //TODO can be also sphere 
        return type == RigidBodyType::EBox && type == RigidBodyType::ESphere ;
    }
};
