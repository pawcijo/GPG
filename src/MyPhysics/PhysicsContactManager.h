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


#include "PhysicsBroadPhase.h"
#include "PhysicsMemory.h"


struct PhysicsContactConstraint;
class PhysicsContactListener;
struct PhysicsBox;
class PhysicsBody;
class PhysicsRender;
class PhysicsStack;

class PhysicsContactManager
{
public:
	PhysicsContactManager( PhysicsStack* stack );

	// Add a new contact constraint for a pair of objects
	// unless the contact constraint already exists
	void AddContact( PhysicsBox *A, PhysicsBox *B );

	// Has broadphase find all contacts and call AddContact on the
	// ContactManager for each pair found
	void FindNewContacts( void );

	// Remove a specific contact
	void RemoveContact( PhysicsContactConstraint *contact );

	// Remove all contacts from a body
	void RemoveContactsFromBody( PhysicsBody *body );
	void RemoveFromBroadphase( PhysicsBody *body );

	// Remove contacts without broadphase overlap
	// Solves contact manifolds
	void TestCollisions( void );
	static void SolveCollision( void* param );

	void RenderContacts( PhysicsRender* debugDrawer ) const;

private:
	PhysicsContactConstraint* m_contactList;
	int m_contactCount;
	PhysicsStack* m_stack;
	PhysicsPagedAllocator m_allocator;
	PhysicsBroadPhase m_broadphase;
	PhysicsContactListener *m_contactListener;

	friend class PhysicsBroadPhase;
	friend class PhysicsScene;
	friend struct PhysicsBox;
	friend class PhysicsBody;
};

