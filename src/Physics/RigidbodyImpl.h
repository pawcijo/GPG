#pragma once

#include "OBB.h"
#include "Sphere.h"
#include "Rigidbody.h"

#include "Common.h"

#include <glm/glm.hpp>



bool AlmostEqualRelativeAndAbs(float A, float B, float maxDiff, float maxRelDiff = FLT_EPSILON);

class RigidbodyImpl : public Rigidbody
{
public:
    glm::vec3 position{0, 0, 0};
    glm::vec3 velocity{0, 0, 0};

    glm::vec3 torques{0, 0, 0};

    glm::vec3 forces{0, 0, 0};

    float mass;
    float cor; // Coefficient of restitution

    float friction;

#ifndef LINEAR_ONLY
    glm::vec3 orientation;
    glm::vec3 angVel;
#endif

#ifdef DYNAMIC_FRICTION
    float staticFriction;
    float dynamicFriction;
#endif

    OBB box;
    Sphere sphere;

public:
    inline RigidbodyImpl() : cor(0.5f), mass(1.0f),
#ifdef DYNAMIC_FRICTION
                             staticFriction(0.5f),
                             dynamicFriction(0.3f),
#else
                             friction(0.6f)
#endif
    {
        type = RigidBodyType::EBase;
    }

    inline RigidbodyImpl(RigidBodyType aType) : cor(0.5f), mass(1.0f),
#ifdef DYNAMIC_FRICTION
                                                staticFriction(0.5f),
                                                dynamicFriction(0.3f),
#else
                                                friction(0.6f)
#endif
    {
        type = aType;
    }
    virtual ~RigidbodyImpl() {}

    float InvMass();

    virtual void ApplyForces();
    virtual void Update(float dt);
    virtual void AddLinearImpulse(const glm::vec3 &impulse);
    void SynchCollisionVolumes();

#ifndef LINEAR_ONLY
    glm::mat4 InvTensor();
    virtual void AddRotationalImpulse(const glm::vec3 &point, const glm::vec3 &impulse);
#endif
};

CollisionResult FindCollisionFeatures(RigidbodyImpl &ra, RigidbodyImpl &rb);
void ApplyImpulse(RigidbodyImpl &A, RigidbodyImpl &B, const CollisionResult &M, int c);