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



#include "PhysicsBroadPhase.h"

#include "PhysicsBox.h"
#include "PhysicsContactManager.h"

#include "Common.h"

#include <algorithm>

#include <string.h>
//--------------------------------------------------------------------------------------------------
// PhysicsBroadPhase
//--------------------------------------------------------------------------------------------------
PhysicsBroadPhase::PhysicsBroadPhase( PhysicsContactManager *manager )
{
	m_manager = manager;

	m_pairCount = 0;
	m_pairCapacity = 64;
	m_pairBuffer = (PhysicsContactPair*)PhysicsAlloc( m_pairCapacity * sizeof( PhysicsContactPair ) );

	m_moveCount = 0;
	m_moveCapacity = 64;
	m_moveBuffer = (int*)PhysicsAlloc( m_moveCapacity * sizeof( int ) );
}

//--------------------------------------------------------------------------------------------------
PhysicsBroadPhase::~PhysicsBroadPhase( )
{
	PhysicsFree( m_moveBuffer );
	PhysicsFree( m_pairBuffer );
}

//--------------------------------------------------------------------------------------------------
void PhysicsBroadPhase::InsertBox( PhysicsBox *box, const PhysicsAABB& aabb )
{
	int id = m_tree.Insert( aabb, box );
	box->broadPhaseIndex = id;
	BufferMove( id );
}

//--------------------------------------------------------------------------------------------------
void PhysicsBroadPhase::RemoveBox( const PhysicsBox *box )
{
	m_tree.Remove( box->broadPhaseIndex );
}

//--------------------------------------------------------------------------------------------------
inline bool ContactPairSort( const PhysicsContactPair& lhs, const PhysicsContactPair& rhs )
{
	if ( lhs.A < rhs.A )
		return true;

	if ( lhs.A == rhs.A )
		return lhs.B < rhs.B;

	return false;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBroadPhase::UpdatePairs( )
{
	m_pairCount = 0;

	// Query the tree with all moving boxs
	for ( int i = 0; i < m_moveCount; ++i)
	{
		m_currentIndex = m_moveBuffer[ i ];
		PhysicsAABB aabb = m_tree.GetFatAABB( m_currentIndex );

		// @TODO: Use a static and non-static tree and query one against the other.
		//        This will potentially prevent (gotta think about this more) time
		//        wasted with queries of static bodies against static bodies, and
		//        kinematic to kinematic.
		m_tree.Query( this, aabb );
	}

	// Reset the move buffer
	m_moveCount = 0;

	// Sort pairs to expose duplicates
	std::sort( m_pairBuffer, m_pairBuffer + m_pairCount, ContactPairSort );

	// Queue manifolds for solving
	{
		int i = 0;
		while ( i < m_pairCount )
		{
			// Add contact to manager
			PhysicsContactPair* pair = m_pairBuffer + i;
			PhysicsBox *A = (PhysicsBox*)m_tree.GetUserData( pair->A );
			PhysicsBox *B = (PhysicsBox*)m_tree.GetUserData( pair->B );
			m_manager->AddContact( A, B );

			++i;

			// Skip duplicate pairs by iterating i until we find a unique pair
			while ( i < m_pairCount )
			{
				PhysicsContactPair* potentialDup = m_pairBuffer + i;

				if ( pair->A != potentialDup->A || pair->B != potentialDup->B )
					break;

				++i;
			}
		}
	}

	m_tree.Validate( );
}

//--------------------------------------------------------------------------------------------------
void PhysicsBroadPhase::Update( int id, const PhysicsAABB& aabb )
{
	if ( m_tree.Update( id, aabb ) )
		BufferMove( id );
}

//--------------------------------------------------------------------------------------------------
bool PhysicsBroadPhase::TestOverlap( int A, int B ) const
{
	return PhysicsAABBtoAABB( m_tree.GetFatAABB( A ), m_tree.GetFatAABB( B ) );
}

//--------------------------------------------------------------------------------------------------
void PhysicsBroadPhase::BufferMove( int id )
{
	if ( m_moveCount == m_moveCapacity )
	{
		int* oldBuffer = m_moveBuffer;
		m_moveCapacity *= 2;
		m_moveBuffer = (int*)PhysicsAlloc( m_moveCapacity * sizeof( int ) );
		memcpy( m_moveBuffer, oldBuffer, m_moveCount * sizeof( int ) );
		PhysicsFree( oldBuffer );
	}

	m_moveBuffer[ m_moveCount++ ] = id;
}
