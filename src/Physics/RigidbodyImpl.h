#pragma once

#include "OBB.h"
#include "Sphere.h"
#include "Rigidbody.h"

#include "Common.h"

#include <glm/glm.hpp>

bool AlmostEqualRelativeAndAbs(float A, float B, float maxDiff, float maxRelDiff = FLT_EPSILON)
{
	// Check if the numbers are really close -- needed when comparing numbers near zero.
	float diff = fabs(A - B);
	if (diff <= maxDiff)
	{
		return true;
	}

	A = fabs(A);
	B = fabs(B);
	float largest = (B > A) ? B : A;

	if (diff <= largest * maxRelDiff)
	{
		return true;
	}
	return false;
}

#define CMP_ABS(x, y) \
	AlmostEqualRelativeAndAbs(x, y, 0.005f)

class RigidbodyImpl : public Rigidbody
{
public:
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

    OBB box;
	Sphere sphere;

public:
    inline RigidbodyImpl() : cor(0.5f), mass(1.0f),
                             staticFriction(0.5f),
                             dynamicFriction(0.3f),
                             friction(0.6f)
    {
        type = RigidBodyType::EBase;
    }

    inline RigidbodyImpl(RigidBodyType aType) : cor(0.5f), mass(1.0f),
                                                staticFriction(0.5f),
                                                dynamicFriction(0.3f),
                                                friction(0.6f)
    {
        type = aType;
    }
    virtual ~RigidbodyImpl() {}

    virtual void Update(float dt);

    float InvMass();

    virtual void ApplyForces();
    void SynchCollisionVolumes();

    virtual void AddLinearImpulse(const glm::vec3 &impulse);

    // FOR LINEAR
    glm::mat4 InvTensor();
    virtual void AddRotationalImpulse(const glm::vec3 &point, const glm::vec3 &impulse);
};


void ApplyImpulse(RigidbodyImpl& A, RigidbodyImpl& B, const CollisionResult& M, int c);