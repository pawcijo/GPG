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
#include "PhysicsBox.h"

class PhysicsBody;
class PhysicsContactEdge;
class PhysicsContactConstraint;


// Restitution mixing. The idea is to use the maximum bounciness, so bouncy
// objects will never not bounce during collisions.
inline float PhysicsMixRestitution( const PhysicsBox* A, const PhysicsBox* B )
{
	return glm::max( A->restitution, B->restitution );
}

// Friction mixing. The idea is to allow a very low friction value to
// drive down the mixing result. Example: anything slides on ice.
inline float PhysicsMixFriction( const PhysicsBox* A, const PhysicsBox* B )
{
	return std::sqrt( A->friction * B->friction );
}


union PhysicsFeaturePair
{
	struct
	{
		unsigned char inR;
		unsigned char outR;
		unsigned char inI;
		unsigned char outI;
	};

	int key;
};

struct PhysicsContact
{
	glm::vec3 position;			// World coordinate of contact
	float penetration;			// Depth of penetration from collision
	float normalImpulse;		// Accumulated normal impulse
	float tangentImpulse[ 2 ];	// Accumulated friction impulse
	float bias;					// Restitution + baumgarte
	float normalMass;			// Normal constraint mass
	float tangentMass[ 2 ];		// Tangent constraint mass
	PhysicsFeaturePair fp;			    // Features on A and B for this contact
	unsigned char warmStarted;	// Used for debug rendering
};


struct PhysicsContactEdge
{
	PhysicsBody *other;
	PhysicsContactConstraint *constraint;
	PhysicsContactEdge* next;
	PhysicsContactEdge* prev;
};

class PhysicsManifold
{
	public:
	void SetPair( PhysicsBox *a, PhysicsBox *b );

	PhysicsBox *A;
	PhysicsBox *B;

	glm::vec3 normal;				// From A to B
	glm::vec3 tangentVectors[ 2 ];	// Tangent vectors
	PhysicsContact contacts[ 8 ];
	int contactCount;

	PhysicsManifold* next;
	PhysicsManifold* prev;

	bool sensor;
};

class PhysicsContactConstraint
{
    public:
	void SolveCollision( void );

	PhysicsBox *A, *B;
	PhysicsBody *bodyA, *bodyB;

	PhysicsContactEdge edgeA;
	PhysicsContactEdge edgeB;
	PhysicsContactConstraint* next;
	PhysicsContactConstraint* prev;

	float friction;
	float restitution;

	PhysicsManifold manifold;

	enum
	{
		eColliding    = 0x00000001, // Set when contact collides during a step
		eWasColliding = 0x00000002, // Set when two objects stop colliding
		eIsland       = 0x00000004, // For internal marking during island forming
	};

	int m_flags;

	friend class ContactManager;
	friend class PhysicsScene;
	friend struct PhysicsIsland;
	friend struct PhysicsContactSolver;
};
