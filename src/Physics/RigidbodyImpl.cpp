#include "RigidbodyImpl.h"

#include <cstdio>

bool AlmostEqualRelativeAndAbs(float A, float B, float maxDiff, float maxRelDiff)
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

#ifndef LINEAR_ONLY
glm::mat4 RigidbodyImpl::InvTensor()
{
	if (mass == 0.0f)
	{
		return glm::mat4(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0);
	}
	float ix = 0.0f;
	float iy = 0.0f;
	float iz = 0.0f;
	float iw = 0.0f;

	if (mass != 0 && type == ESphere)
	{
		float r2 = sphere.radius * sphere.radius;
		float fraction = (2.0f / 5.0f);

		ix = r2 * mass * fraction;
		iy = r2 * mass * fraction;
		iz = r2 * mass * fraction;
		iw = 1.0f;
	}
	else if (mass != 0 && type == EBox)
	{
		glm::vec3 size = box.size * 2.0f;
		float fraction = (1.0f / 12.0f);

		float x2 = size.x * size.x;
		float y2 = size.y * size.y;
		float z2 = size.z * size.z;

		ix = (y2 + z2) * mass * fraction;
		iy = (x2 + z2) * mass * fraction;
		iz = (x2 + y2) * mass * fraction;
		iw = 1.0f;
	}

	return glm::inverse(glm::mat4(
		ix, 0, 0, 0,
		0, iy, 0, 0,
		0, 0, iz, 0,
		0, 0, 0, iw));
}
#endif

void RigidbodyImpl::ApplyForces()
{
	forces = GRAVITY_CONST * mass;
}

#ifndef LINEAR_ONLY
void RigidbodyImpl::AddRotationalImpulse(const glm::vec3 &point, const glm::vec3 &impulse)
{
	glm::vec3 centerOfMass = position;
	glm::vec3 torque = glm::cross(point - centerOfMass, impulse);

	glm::vec3 angAccel = MultiplyVector(torque, InvTensor());
	angVel = angVel + angAccel;
}
#endif

void RigidbodyImpl::AddLinearImpulse(const glm::vec3 &impulse)
{
	velocity = velocity + impulse;
}

float RigidbodyImpl::InvMass()
{
	if (mass == 0.0f)
	{
		return 0.0f;
	}
	return 1.0f / mass;
}

void RigidbodyImpl::SynchCollisionVolumes()
{
	sphere.position = position;
	box.position = position;

#ifndef LINEAR_ONLY
	box.orientation = Rotation3x3(
		glm::degrees(orientation.x),
		glm::degrees(orientation.y),
		glm::degrees(orientation.z));
#endif
}

void RigidbodyImpl::Update(float dt)
{
	// Integrate velocity
	const float damping = 0.98f;

	// printf("InvMass \n");
	glm::vec3 acceleration = forces * InvMass();
	// printf("InvMass end \n");
	velocity = velocity + acceleration * dt;
	velocity = velocity * damping;

	if (fabsf(velocity.x) < 0.001f)
	{
		velocity.x = 0.0f;
	}
	if (fabsf(velocity.y) < 0.001f)
	{
		velocity.y = 0.0f;
	}
	if (fabsf(velocity.z) < 0.001f)
	{
		velocity.z = 0.0f;
	}

#ifndef LINEAR_ONLY
	if (type == EBox)
	{
		// printf("MultiplyVector \n");
		glm::vec3 angAccel = MultiplyVector(torques, InvTensor());
		// printf("MultiplyVector end \n");
		angVel = angVel + angAccel * dt;
		angVel = angVel * damping;

		if (fabsf(angVel.x) < 0.001f)
		{
			angVel.x = 0.0f;
		}
		if (fabsf(angVel.y) < 0.001f)
		{
			angVel.y = 0.0f;
		}
		if (fabsf(angVel.z) < 0.001f)
		{
			angVel.z = 0.0f;
		}
	}
#endif

	// Integrate position
	position = position + velocity * dt;

#ifndef LINEAR_ONLY
	if (type == EBox)
	{
		orientation = orientation + angVel * dt;
	}
#endif

	SynchCollisionVolumes();
}

CollisionResult FindCollisionFeatures(RigidbodyImpl &ra, RigidbodyImpl &rb)
{
	CollisionResult result;
	ResetCollisionResult(&result);

	// printf("FindCollisionFeatures Impl 188  \n");
	if (ra.type == ESphere)
	{
		if (rb.type == ESphere)
		{
			result = FindCollisionFeatures(ra.sphere, rb.sphere);
		}
		else if (rb.type == EBox)
		{
			result = FindCollisionFeatures(rb.box, ra.sphere);
			result.normal = result.normal * -1.0f;
		}
	}
	else if (ra.type == EBox)
	{
		if (rb.type == EBox)
		{
			// printf("FindCollisionFeatures Impl 205 BOX BOX  \n");
			result = FindCollisionFeatures(ra.box, rb.box);
		}
		else if (rb.type == ESphere)
		{

			result = FindCollisionFeatures(ra.box, rb.sphere);
		}
	}

	return result;
}

void ApplyImpulse(RigidbodyImpl &A, RigidbodyImpl &B, const CollisionResult &M, int c)
{
	// Linear impulse
	float invMass1 = A.InvMass();
	float invMass2 = B.InvMass();
	float invMassSum = invMass1 + invMass2;

	if (invMassSum == 0.0f)
	{
		return; // Both objects have infinate mass!
	}

#ifndef LINEAR_ONLY
	glm::vec3 r1 = M.contacts[c] - A.position;
	glm::vec3 r2 = M.contacts[c] - B.position;
	glm::mat4 i1 = A.InvTensor();
	glm::mat4 i2 = B.InvTensor();
#endif

	// Relative velocity
#ifndef LINEAR_ONLY
	glm::vec3 relativeVel = (B.velocity + glm::cross(B.angVel, r2)) - (A.velocity + glm::cross(A.angVel, r1));
#else
	vec3 relativeVel = B.velocity - A.velocity;
#endif
	// Relative collision normal
	glm::vec3 relativeNorm = M.normal;
	relativeNorm = glm::normalize(relativeNorm);

	// Moving away from each other? Do nothing!
	if (glm::dot(relativeVel, relativeNorm) > 0.0f)
	{
		return;
	}

	float e = fminf(A.cor, B.cor);

	float numerator = (-(1.0f + e) * glm::dot(relativeVel, relativeNorm));
	float d1 = invMassSum;
#ifndef LINEAR_ONLY
	glm::vec3 d2 = glm::cross(MultiplyVector(glm::cross(r1, relativeNorm), i1), r1);
	glm::vec3 d3 = glm::cross(MultiplyVector(glm::cross(r2, relativeNorm), i2), r2);
	float denominator = d1 + glm::dot(relativeNorm, d2 + d3);
#else
	float denominator = d1;
#endif

	float j = (denominator == 0.0f) ? 0.0f : numerator / denominator;
	if (M.contacts.size() > 0.0f && j != 0.0f)
	{
		j /= (float)M.contacts.size();
	}

	glm::vec3 impulse = relativeNorm * j;
	A.velocity = A.velocity - impulse * invMass1;
	B.velocity = B.velocity + impulse * invMass2;

#ifndef LINEAR_ONLY
	A.angVel = A.angVel - MultiplyVector(glm::cross(r1, impulse), i1);
	B.angVel = B.angVel + MultiplyVector(glm::cross(r2, impulse), i2);
#endif

	// Friction
	glm::vec3 t = relativeVel - (relativeNorm * glm::dot(relativeVel, relativeNorm));
	if (AlmostEqualRelativeAndAbs(MagnitudeSq(t), 0.0f, 0.005, FLT_EPSILON))
	{
		return;
	}
	t = glm::normalize(t);

	numerator = -glm::dot(relativeVel, t);
	d1 = invMassSum;
#ifndef LINEAR_ONLY
	d2 = glm::cross(MultiplyVector(glm::cross(r1, t), i1), r1);
	d3 = glm::cross(MultiplyVector(glm::cross(r2, t), i2), r2);
	denominator = d1 + glm::dot(t, d2 + d3);
#else
	denominator = d1;
#endif

	float jt = (denominator == 0.0f) ? 0.0f : numerator / denominator;
	if (M.contacts.size() > 0.0f && jt != 0.0f)
	{
		jt /= (float)M.contacts.size();
	}

	if (AlmostEqualRelativeAndAbs(jt, 0.0f, 0.05, FLT_EPSILON))
	{
		return;
	}

	glm::vec3 tangentImpuse;
#ifdef DYNAMIC_FRICTION
	float sf = sqrtf(A.staticFriction * B.staticFriction);
	float df = sqrtf(A.dynamicFriction * B.dynamicFriction);
	if (fabsf(jt) < j * sf)
	{
		tangentImpuse = t * jt;
	}
	else
	{
		tangentImpuse = t * -j * df;
	}
#else
	float friction = sqrtf(A.friction * B.friction);
	if (jt > j * friction)
	{
		jt = j * friction;
	}
	else if (jt < -j * friction)
	{
		jt = -j * friction;
	}
	tangentImpuse = t * jt;
#endif

	A.velocity = A.velocity - tangentImpuse * invMass1;
	B.velocity = B.velocity + tangentImpuse * invMass2;

#ifndef LINEAR_ONLY
	A.angVel = A.angVel - MultiplyVector(glm::cross(r1, tangentImpuse), i1);
	B.angVel = B.angVel + MultiplyVector(glm::cross(r2, tangentImpuse), i2);
#endif
}