#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>

#include "glm/glm.hpp"

#define Q3_R32_MAX FLT_MAX

#define Q3_SLEEP_LINEAR float(0.01)

#define Q3_SLEEP_ANGULAR float((3.0 / 180.0) * glm::pi<float>())

#define Q3_SLEEP_TIME float(0.5)

class PhysicsHalfSpace
{
public:
	PhysicsHalfSpace();
	PhysicsHalfSpace(const glm::vec3 &normal, float distance);

	void Set(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c);
	void Set(const glm::vec3 &n, const glm::vec3 &p);
	const glm::vec3 Origin() const;
	float Distance(const glm::vec3 &p) const;
	const glm::vec3 Projected(const glm::vec3 &p) const;

	glm::vec3 normal;
	float distance;
};

struct PhysicsAABB
{
	glm::vec3 min;
	glm::vec3 max;

	bool Contains(const PhysicsAABB &other) const;
	bool Contains(const glm::vec3 &point) const;
	float SurfaceArea() const;
};

inline float PhysicsAABB::SurfaceArea( ) const
{
	float x = max.x - min.x;
	float y = max.y - min.y;
	float z = max.z - min.z;

	return float( 2.0 ) * (x * y + x * z + y * z);
}

struct PhysicsMassData
{
	glm::mat3 inertia;
	glm::vec3 center;
	float mass;
};

float Abs(float a);

const glm::vec3 Abs(const glm::vec3 &v);

void Integrate(glm::quat &quaternion, glm::vec3 velocities, float dt);

inline float InvertFloat(float a)
{
	return a != 0.0f ? 1.0f / a : 0.0f;
}

bool PhysicsAABBtoAABB(const PhysicsAABB &a, const PhysicsAABB &b);

float PhysicsSign(float a);

bool PhysicsTrackFaceAxis(int *axis, int n, float s, float *sMax, const glm::vec3 &normal, glm::vec3 *axisNormal);

bool PhysicsTrackEdgeAxis(int *axis, int n, float s, float *sMax, const glm::vec3 &normal, glm::vec3 *axisNormal);

glm::mat3 PhysicsDiagonal(float vector);

glm::mat3 PhysicsDiagonal(glm::vec3 vector);

glm::mat3 PhysicsDiagonal(float x, float y, float z);

const glm::mat3 PhysicsOuterProduct(const glm::vec3 &u, const glm::vec3 &v);

void  MatrixRawIdentity(glm::mat3& matrix, int raw);


inline void PhysicsComputeBasis( const glm::vec3& a, glm::vec3* __restrict b, glm::vec3* __restrict c )
{
	// Suppose vector a has all equal components and is a unit vector: a = (s, s, s)
	// Then 3*s*s = 1, s = sqrt(1/3) = 0.57735027. This means that at least one component of a
	// unit vector must be greater or equal to 0.57735027. Can use SIMD select operation.

	if ( Abs( a.x ) >= float( 0.57735027 ) )
		*b = glm::vec3( a.y, -a.x, float( 0.0 ) );
	else
		*b = glm::vec3( float( 0.0 ), a.z, -a.y );

	*b = glm::normalize( *b );
	*c = glm::cross( a, *b );
}

inline const PhysicsAABB PhysicsCombine( const PhysicsAABB& a, const PhysicsAABB& b )
{
	PhysicsAABB c;

	c.min = glm::min( a.min, b.min );
	c.max = glm::max( a.max, b.max );

	return c;
}

inline float Clamp01( float val )
{
	if ( val >= float( 1.0 ) )
		return 1.0;

	if ( val <= float( 0.0 ) )
		return 0.0;

	return val;
}