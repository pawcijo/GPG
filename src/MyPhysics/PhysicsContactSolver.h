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


#pragma once

#include <glm/glm.hpp>

struct PhysicsIsland;
struct PhysicsVelocityState;

struct PhysicsContactState
{
	glm::vec3 ra;				// Vector from C.O.M to contact position
	glm::vec3 rb;				// Vector from C.O.M to contact position
	float penetration;			// Depth of penetration from collision
	float normalImpulse;		// Accumulated normal impulse
	float tangentImpulse[ 2 ];	// Accumulated friction impulse
	float bias;					// Restitution + baumgarte
	float normalMass;			// Normal constraint mass
	float tangentMass[ 2 ];		// Tangent constraint mass
};

struct PhysicsContactConstraintState
{
	PhysicsContactState contacts[ 8 ];
	int contactCount;
	glm::vec3 tangentVectors[ 2 ];	// Tangent vectors
	glm::vec3 normal;				// From A to B
	glm::vec3 centerA;
	glm::vec3 centerB;
	glm::mat3 iA;
	glm::mat3 iB;
	float mA;
	float mB;
	float restitution;
	float friction;
	int indexA;
	int indexB;
};

struct PhysicsContactSolver
{
	void Initialize( PhysicsIsland *island );
	void ShutDown( void );

	void PreSolve( float dt );
	void Solve( void );

	PhysicsIsland *m_island;
	PhysicsContactConstraintState *m_contacts;
	int m_contactCount;
	PhysicsVelocityState *m_velocities;

	bool m_enableFriction;
};