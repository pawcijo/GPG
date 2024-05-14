#include "Common.h"

#include "PhysicsTransform.h"

//--------------------------------------------------------------------------------------------------
// PhysicsHalfSpace
//--------------------------------------------------------------------------------------------------
PhysicsHalfSpace::PhysicsHalfSpace()
{
}

//--------------------------------------------------------------------------------------------------
PhysicsHalfSpace::PhysicsHalfSpace(const glm::vec3 &n, float d)
	: normal(n), distance(d)
{
}

//--------------------------------------------------------------------------------------------------
void PhysicsHalfSpace::Set(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
{
	normal = glm::normalize(glm::cross(b - a, c - a));
	distance = glm::dot(normal, a);
}

//--------------------------------------------------------------------------------------------------
void PhysicsHalfSpace::Set(const glm::vec3 &n, const glm::vec3 &p)
{
	normal = glm::normalize(n);
	distance = glm::dot(normal, p);
}

//--------------------------------------------------------------------------------------------------
const glm::vec3 PhysicsHalfSpace::Origin() const
{
	return normal * distance;
}

//--------------------------------------------------------------------------------------------------
float PhysicsHalfSpace::Distance(const glm::vec3 &p) const
{
	return glm::dot(normal, p) - distance;
}

//--------------------------------------------------------------------------------------------------
const glm::vec3 PhysicsHalfSpace::Projected(const glm::vec3 &p) const
{
	return p - normal * Distance(p);
}

//--------------------------------------------------------------------------------------------------
bool PhysicsAABB::Contains(const PhysicsAABB &other) const
{
	return min.x <= other.min.x &&
		   min.y <= other.min.y &&
		   min.z <= other.min.z &&
		   max.x >= other.max.x &&
		   max.y >= other.max.y &&
		   max.z >= other.max.z;
}

//--------------------------------------------------------------------------------------------------
bool PhysicsAABB::Contains(const glm::vec3 &point) const
{
	return min.x <= point.x &&
		   min.y <= point.y &&
		   min.z <= point.z &&
		   max.x >= point.x &&
		   max.y >= point.y &&
		   max.z >= point.z;
}

float Abs(float a)
{
	if (a < float(0.0))
		return -a;

	return a;
}


const glm::vec3 Abs(const glm::vec3 &v)
{
	return glm::vec3(Abs(v.x), Abs(v.y), Abs(v.z));
}

void Integrate(glm::quat &quaternion, glm::vec3 angularVelocity, float dt)
{

	glm::quat dq = glm::quat(0, angularVelocity.x * dt, angularVelocity.y * dt, angularVelocity.z * dt);

	quaternion.x += dq.x * float(0.5f);
	quaternion.y += dq.y * float(0.5f);
	quaternion.z += dq.z * float(0.5f);
	quaternion.w += dq.w * float(0.5f);


	// Integrate the change into the current rotation
	quaternion = glm::normalize(quaternion);
}

bool PhysicsAABBtoAABB(const PhysicsAABB &a, const PhysicsAABB &b)
{
	if (a.max.x < b.min.x || a.min.x > b.max.x)
		return false;

	if (a.max.y < b.min.y || a.min.y > b.max.y)
		return false;

	if (a.max.z < b.min.z || a.min.z > b.max.z)
		return false;

	return true;
}

float PhysicsSign(float a)
{
	if (a >= float(0.0))
	{
		return float(1.0);
	}

	else
	{
		return float(-1.0);
	}
}

bool PhysicsTrackFaceAxis(int *axis, int n, float s, float *sMax, const glm::vec3 &normal, glm::vec3 *axisNormal)
{
	if (s > float(0.0))
		return true;

	if (s > *sMax)
	{
		*sMax = s;
		*axis = n;
		*axisNormal = normal;
	}

	return false;
}

bool PhysicsTrackEdgeAxis(int *axis, int n, float s, float *sMax, const glm::vec3 &normal, glm::vec3 *axisNormal)
{
	if (s > float(0.0))
		return true;

	float l = float(1.0) / glm::length(normal);
	s *= l;

	if (s > *sMax)
	{
		*sMax = s;
		*axis = n;
		*axisNormal = normal * l;
	}

	return false;
}

glm::mat3 PhysicsDiagonal(float value)
{
	return glm::mat3(glm::vec3(value, 0.0f, 0.0f),
					 glm::vec3(0.0f, value, 0.0f),
					 glm::vec3(0.0f, 0.0f, value));
}

glm::mat3 PhysicsDiagonal(float x, float y, float z)
{
	return glm::mat3(glm::vec3(x, 0.0f, 0.0f),
					 glm::vec3(0.0f, y, 0.0f),
					 glm::vec3(0.0f, 0.0f, z));
}

glm::mat3 PhysicsDiagonal(glm::vec3 vector)
{
	return glm::mat3(glm::vec3(vector.x, 0.0f, 0.0f),
					 glm::vec3(0.0f, vector.y, 0.0f),
					 glm::vec3(0.0f, 0.0f, vector.z));
}

const glm::mat3 PhysicsOuterProduct(const glm::vec3 &u, const glm::vec3 &v)
{
	glm::vec3 a = v * u.x;
	glm::vec3 b = v * u.y;
	glm::vec3 c = v * u.z;

	return glm::mat3(
		a.x, a.y, a.z,
		b.x, b.y, b.z,
		c.x, c.y, c.z);
}

void MatrixRawIdentity(glm::mat3& matrix, int raw)
{

	glm::vec3 raw0 = glm::vec3(matrix[0][0], matrix[1][0], matrix[2][0]);
	glm::vec3 raw1 = glm::vec3(matrix[0][1], matrix[1][1], matrix[2][1]);
	glm::vec3 raw2 = glm::vec3(matrix[0][2], matrix[1][2], matrix[2][2]);

	if (0 == raw)
	{
		raw0 = glm::vec3{1};
	}
	else if (1 == raw)
	{
		raw1 = glm::vec3{1};
	}
	else if (2 == raw)
	{
		raw2 = glm::vec3{1};
	}

	matrix = glm::mat3(raw0, raw1, raw2);
}