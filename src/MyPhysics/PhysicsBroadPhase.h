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

#include "PhysicsDynamicAABBTree.h"
#include "PhysicsMemory.h"

#include <string.h>

class PhysicsContactManager;
class PhysicsBox;
struct PhysicsTransform;
struct PhysicsAABB;

struct PhysicsContactPair
{
	int A;
	int B;
};

class PhysicsBroadPhase
{
public:
	PhysicsBroadPhase( PhysicsContactManager *manager );
	~PhysicsBroadPhase( );

	void InsertBox( PhysicsBox *shape, const PhysicsAABB& aabb );
	void RemoveBox( const PhysicsBox *shape );

	// Generates the contact list. All previous contacts are returned to the allocator
	// before generation occurs.
	void UpdatePairs( void );

	void Update( int id, const PhysicsAABB& aabb );

	bool TestOverlap( int A, int B ) const;

private:
	PhysicsContactManager *m_manager;

	PhysicsContactPair* m_pairBuffer;
	int m_pairCount;
	int m_pairCapacity;

	int* m_moveBuffer;
	int m_moveCount;
	int m_moveCapacity;

	PhysicsDynamicAABBTree m_tree;
	int m_currentIndex;

	void BufferMove( int id );
	bool TreeCallBack( int index );

	friend class PhysicsDynamicAABBTree;
	friend class PhysicsScene;
};

inline bool PhysicsBroadPhase::TreeCallBack( int index )
{
	// Cannot collide with self
	if ( index == m_currentIndex )
		return true;

	if ( m_pairCount == m_pairCapacity )
	{
		PhysicsContactPair* oldBuffer = m_pairBuffer;
		m_pairCapacity *= 2;
		m_pairBuffer = (PhysicsContactPair*)PhysicsAlloc( m_pairCapacity * sizeof( PhysicsContactPair ) );
		memcpy( m_pairBuffer, oldBuffer, m_pairCount * sizeof( PhysicsContactPair ) );
		PhysicsFree( oldBuffer );
	}

	int iA = glm::min( index, m_currentIndex );
	int iB = glm::max( index, m_currentIndex );

	m_pairBuffer[ m_pairCount ].A = iA;
	m_pairBuffer[ m_pairCount ].B = iB;
	++m_pairCount;

	return true;
}