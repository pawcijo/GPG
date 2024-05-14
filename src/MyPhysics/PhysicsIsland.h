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

class PhysicsBody;
class PhysicsContactConstraint;
struct PhysicsContactConstraintState;

struct PhysicsVelocityState
{
	glm::vec3 w;
	glm::vec3 v;
};


class PhysicsIsland
{
    public:
	void Solve( );
	void Add( PhysicsBody *body );
	void Add( PhysicsContactConstraint *contact );
	void Initialize( );

	PhysicsBody **m_bodies;
	PhysicsVelocityState *m_velocities;
	int m_bodyCapacity;
	int m_bodyCount;

	PhysicsContactConstraint **m_contacts;
	PhysicsContactConstraintState *m_contactStates;
	int m_contactCount;
	int m_contactCapacity;

	float m_dt;
	glm::vec3 m_gravity;
	int m_iterations;

	bool m_allowSleep;
	bool m_enableFriction;
};