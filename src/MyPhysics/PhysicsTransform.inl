//--------------------------------------------------------------------------------------------------
/**
@file	DropBoxes.h

@author	Randy Gaul
@date	11/25/2014

	Copyright (c) 2014 Randy Gaul http://www.randygaul.net

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
		1. The origin of this software must not be misrepresented; you must not
			 claim that you wrote the original software. If you use this software
			 in a product, an acknowledgment in the product documentation would be
			 appreciated but is not required.
		2. Altered source versions must be plainly marked as such, and must not
			 be misrepresented as being the original software.
		3. This notice may not be removed or altered from any source distribution.
*/
//--------------------------------------------------------------------------------------------------


#include "PhysicsTransform.h"

#include "Common.h"

inline const glm::vec3 TransformMul(const PhysicsTransform &tx, const glm::vec3 &v)
{
	return glm::vec3(tx.rotation * v + tx.position);
}

//--------------------------------------------------------------------------------------------------
inline const glm::vec3 TransformMul(const PhysicsTransform &tx, const glm::vec3 &scale, const glm::vec3 &v)
{
	return glm::vec3(tx.rotation * (scale * v) + tx.position);
}

//--------------------------------------------------------------------------------------------------
inline const glm::vec3 TransformMul(const PhysicsTransform *tx, const glm::vec3 &v)
{
	return glm::vec3(tx->rotation * v + tx->position);
}

//--------------------------------------------------------------------------------------------------
inline const glm::vec3 TransformMul(const glm::mat3 &r, const glm::vec3 &v)
{
	return r * v;
}

//--------------------------------------------------------------------------------------------------
inline const glm::mat3 TransformMul(const glm::mat3 &r, const glm::mat3 &q)
{
	return r * q;
}

//--------------------------------------------------------------------------------------------------
inline const PhysicsTransform TransformMul(const PhysicsTransform &t, const PhysicsTransform &u)
{
	PhysicsTransform v;
	v.rotation = TransformMul(t.rotation, u.rotation);
	v.position = TransformMul(t.rotation, u.position) + t.position;
	int dupa  = 1;
	return v;
}

//--------------------------------------------------------------------------------------------------
inline const PhysicsHalfSpace TransformMul(const PhysicsTransform &tx, const PhysicsHalfSpace &p)
{
	glm::vec3 origin = p.Origin();
	origin = TransformMul(tx, origin);
	glm::vec3 normal = TransformMul(tx.rotation, p.normal);

	return PhysicsHalfSpace(normal, glm::dot(origin, normal));
}

//--------------------------------------------------------------------------------------------------
inline const PhysicsHalfSpace TransformMul(const PhysicsTransform &tx, const glm::vec3 &scale, const PhysicsHalfSpace &p)
{
	glm::vec3 origin = p.Origin();
	origin = TransformMul(tx, scale, origin);
	glm::vec3 normal = TransformMul(tx.rotation, p.normal);

	return PhysicsHalfSpace(normal, glm::dot(origin, normal));
}

//--------------------------------------------------------------------------------------------------
inline const glm::vec3 TransformMulTranspose(const PhysicsTransform &tx, const glm::vec3 &v)
{
	return glm::transpose(tx.rotation) * (v - tx.position);
}

//--------------------------------------------------------------------------------------------------
inline const glm::vec3 TransformMulTranspose(const glm::mat3 &r, const glm::vec3 &v)
{
	glm::vec3 a = glm::transpose(r) * v;
	return a;
}

//--------------------------------------------------------------------------------------------------
inline const glm::mat3 TransformMulTranspose(const glm::mat3 &r, const glm::mat3 &q)
{
	return glm::transpose(r) * q;
}

//--------------------------------------------------------------------------------------------------
inline const PhysicsTransform TransformMulTranspose(const PhysicsTransform &t, const PhysicsTransform &u)
{
	PhysicsTransform v;
	v.rotation = TransformMulTranspose(t.rotation, u.rotation);
	v.position = TransformMulTranspose(t.rotation, u.position - t.position);
	return v;
}

//--------------------------------------------------------------------------------------------------
inline const PhysicsHalfSpace TransformMulTranspose(const PhysicsTransform &tx, const PhysicsHalfSpace &p)
{
	glm::vec3 origin = p.normal * p.distance;
	origin = TransformMulTranspose(tx, origin);
	glm::vec3 n = TransformMulTranspose(tx.rotation, p.normal);
	return PhysicsHalfSpace(n, glm::dot(origin, n));
}

//--------------------------------------------------------------------------------------------------
inline void TransformIdentity(PhysicsTransform &tx)
{
	tx.position = glm::vec3{0};
	tx.rotation = glm::mat3(1);
}

//--------------------------------------------------------------------------------------------------
inline const PhysicsTransform q3Inverse(const PhysicsTransform &tx)
{
	PhysicsTransform inverted;
	inverted.rotation = glm::transpose(tx.rotation);
	inverted.position = TransformMul(inverted.rotation, -tx.position);
	return inverted;
}