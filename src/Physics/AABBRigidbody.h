#pragma once

#include "AABB.h"
#include "Rigidbody.h"

#include <glm/glm.hpp>

class AABBRigidBody : public Rigidbody
{
    glm::vec3 position;
    glm::vec3 velocity;

    // LINEAR
    //-----------------------------------
    glm::vec3 orientation;
    glm::vec3 angVel;
    glm::vec3 torques;

    //-----------------------------------

    glm::vec3 forces;

    float mass;
    float cor; // Coefficient of restitution

    // Dynamic friction
    float staticFriction;
    float dynamicFriction;
    // Static friction
    float friction;

    AABB box;

public:
    inline AABBRigidBody() : cor(0.5f), mass(1.0f),
                             staticFriction(0.5f),
                             dynamicFriction(0.3f),
                             friction(0.6f)
    {
        type = RigidBodyType::base;
    }

    inline AABBRigidBody(RigidBodyType aType) : cor(0.5f), mass(1.0f),
                                                staticFriction(0.5f),
                                                dynamicFriction(0.3f),
                                                friction(0.6f)
    {
        type = aType;
    }
    virtual ~AABBRigidBody() {}

    virtual void Update(float dt);

    float InvMass();

    virtual void ApplyForces();
    void SynchCollisionVolumes();

    virtual void AddLinearImpulse(const glm::vec3 &impulse);

    // FOR LINEAR
    glm::mat4 InvTensor();
    virtual void AddRotationalImpulse(const glm::vec3 &point, const glm::vec3 &impulse);
};


CollisionResult FindCollisionFeatures(AABBRigidBody& ra, AABBRigidBody& rb);
void ApplyImpulse(AABBRigidBody& A, AABBRigidBody& B, const CollisionResult& M, int c);