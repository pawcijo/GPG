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


#include "PhysicsScene.h"


#include <stdlib.h>

#include "PhysicsBody.h"
#include "PhysicsIsland.h"
#include "PhysicsContact.h"
#include "PhysicsIsland.h"
#include "PhysicsContactSolver.h"
#include "PhysicsBox.h"

//--------------------------------------------------------------------------------------------------
// PhysicsScene
//--------------------------------------------------------------------------------------------------
PhysicsScene::PhysicsScene( float dt, const glm::vec3& gravity, int iterations )
	: m_contactManager( &m_stack )
	, m_boxAllocator( sizeof( PhysicsBox ), 256 )
	, m_bodyCount( 0 )
	, m_bodyList( NULL )
	, m_gravity( gravity )
	, m_dt( dt )
	, m_iterations( iterations )
	, m_newBox( false )
	, m_allowSleep( true )
	, m_enableFriction( true )
{
}

//--------------------------------------------------------------------------------------------------
PhysicsScene::~PhysicsScene( )
{
	Shutdown( );
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::Step( float deltaTime)
{
	if ( m_newBox )
	{
		m_contactManager.m_broadphase.UpdatePairs( );
		m_newBox = false;
	}

	m_contactManager.TestCollisions( );

	for ( PhysicsBody* body = m_bodyList; body; body = body->m_next )
		body->m_flags &= ~PhysicsBody::eIsland;

	// Size the stack island, pick worst case size
	m_stack.Reserve(
		sizeof( PhysicsBody* ) * m_bodyCount
		+ sizeof( PhysicsVelocityState ) * m_bodyCount
		+ sizeof( PhysicsContactConstraint* ) * m_contactManager.m_contactCount
		+ sizeof( PhysicsContactConstraintState ) * m_contactManager.m_contactCount
		+ sizeof( PhysicsBody* ) * m_bodyCount
	);

	PhysicsIsland island;
	island.m_bodyCapacity = m_bodyCount;
	island.m_contactCapacity = m_contactManager.m_contactCount;
	island.m_bodies = (PhysicsBody**)m_stack.Allocate( sizeof( PhysicsBody* ) * m_bodyCount );
	island.m_velocities = (PhysicsVelocityState *)m_stack.Allocate( sizeof( PhysicsVelocityState ) * m_bodyCount );
	island.m_contacts = (PhysicsContactConstraint **)m_stack.Allocate( sizeof( PhysicsContactConstraint* ) * island.m_contactCapacity );
	island.m_contactStates = (PhysicsContactConstraintState *)m_stack.Allocate( sizeof( PhysicsContactConstraintState ) * island.m_contactCapacity );
	island.m_allowSleep = m_allowSleep;
	island.m_enableFriction = m_enableFriction;
	island.m_bodyCount = 0;
	island.m_contactCount = 0;
	island.m_dt = m_dt;
	island.m_gravity = m_gravity;
	island.m_iterations = m_iterations;

	// Build each active island and then solve each built island
	int stackSize = m_bodyCount;
	PhysicsBody** stack = (PhysicsBody**)m_stack.Allocate( sizeof( PhysicsBody* ) * stackSize );
	for ( PhysicsBody* seed = m_bodyList; seed; seed = seed->m_next )
	{
		// Seed cannot be apart of an island already
		if ( seed->m_flags & PhysicsBody::eIsland )
			continue;

		// Seed must be awake
		if ( !(seed->m_flags & PhysicsBody::eAwake) )
			continue;

		// Seed cannot be a static body in order to keep islands
		// as small as possible
		if ( seed->m_flags & PhysicsBody::eStatic )
			continue;

		int stackCount = 0;
		stack[ stackCount++ ] = seed;
		island.m_bodyCount = 0;
		island.m_contactCount = 0;

		// Mark seed as apart of island
		seed->m_flags |= PhysicsBody::eIsland;

		// Perform DFS on constraint graph
		while( stackCount > 0 )
		{
			// Decrement stack to implement iterative backtracking
			PhysicsBody *body = stack[ --stackCount ];
			island.Add( body );

			// Awaken all bodies connected to the island
			body->SetToAwake( );

			// Do not search across static bodies to keep island
			// formations as small as possible, however the static
			// body itself should be apart of the island in order
			// to properly represent a full contact
			if ( body->m_flags & PhysicsBody::eStatic )
				continue;

			// Search all contacts connected to this body
			PhysicsContactEdge* contacts = body->m_contactList;
			for ( PhysicsContactEdge* edge = contacts; edge; edge = edge->next )
			{
				PhysicsContactConstraint *contact = edge->constraint;

				// Skip contacts that have been added to an island already
				if ( contact->m_flags & PhysicsContactConstraint::eIsland )
					continue;

				// Can safely skip this contact if it didn't actually collide with anything
				if ( !(contact->m_flags & PhysicsContactConstraint::eColliding) )
					continue;

				// Skip sensors
				if ( contact->A->sensor || contact->B->sensor )
					continue;

				// Mark island flag and add to island
				contact->m_flags |= PhysicsContactConstraint::eIsland;
				island.Add( contact );

				// Attempt to add the other body in the contact to the island
				// to simulate contact awakening propogation
				PhysicsBody* other = edge->other;
				if ( other->m_flags & PhysicsBody::eIsland )
					continue;

				assert( stackCount < stackSize );

				stack[ stackCount++ ] = other;
				other->m_flags |= PhysicsBody::eIsland;
			}
		}

		assert( island.m_bodyCount != 0 );

		island.Initialize( );
		island.Solve( deltaTime);

		// Reset all static island flags
		// This allows static bodies to participate in other island formations
		for ( int i = 0; i < island.m_bodyCount; i++ )
		{
			PhysicsBody *body = island.m_bodies[ i ];

			if ( body->m_flags & PhysicsBody::eStatic )
				body->m_flags &= ~PhysicsBody::eIsland;
		}
	}

	m_stack.Free( stack );
	m_stack.Free( island.m_contactStates );
	m_stack.Free( island.m_contacts );
	m_stack.Free( island.m_velocities );
	m_stack.Free( island.m_bodies );

	// Update the broadphase AABBs
	for ( PhysicsBody* body = m_bodyList; body; body = body->m_next )
	{
		if ( body->m_flags & PhysicsBody::eStatic )
			continue;

		body->SynchronizeProxies( );
	}

	// Look for new contacts
	m_contactManager.FindNewContacts( );

	// Clear all forces
	for ( PhysicsBody* body = m_bodyList; body; body = body->m_next )
	{
		 body->m_force  = glm::vec3{0};
		 body->m_torque = glm::vec3{0}; 
	}
}

//--------------------------------------------------------------------------------------------------
PhysicsBody* PhysicsScene::CreateBody( const PhysicsBodyDef& def )
{
	PhysicsBody* body = (PhysicsBody*)m_heap.Allocate( sizeof( PhysicsBody ) );
	new (body) PhysicsBody( def, this );

	// Add body to scene bodyList
	body->m_prev = NULL;
	body->m_next = m_bodyList;

	if ( m_bodyList )
		m_bodyList->m_prev = body;

	m_bodyList = body;
	++m_bodyCount;

	return body;
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::RemoveBody( PhysicsBody* body )
{
	assert( m_bodyCount > 0 );

	m_contactManager.RemoveContactsFromBody( body );

	body->RemoveAllBoxes( );

	// Remove body from scene bodyList
	if ( body->m_next )
		body->m_next->m_prev = body->m_prev;

	if ( body->m_prev )
		body->m_prev->m_next = body->m_next;

	if ( body == m_bodyList )
		m_bodyList = body->m_next;

	--m_bodyCount;

	m_heap.Free( body );
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::RemoveAllBodies( )
{
	PhysicsBody* body = m_bodyList;

	while ( body )
	{
		PhysicsBody* next = body->m_next;

		body->RemoveAllBoxes( );

		m_heap.Free( body );

		body = next;
	}

	m_bodyList = NULL;
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::SetAllowSleep( bool allowSleep )
{
	m_allowSleep = allowSleep;

	if ( !allowSleep )
	{
		for ( PhysicsBody* body = m_bodyList; body; body = body->m_next )
			body->SetToAwake( );
	}
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::SetIterations( int iterations )
{
	m_iterations = glm::max( 1, iterations );
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::SetEnableFriction( bool enabled )
{
	m_enableFriction = enabled;
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::Render( PhysicsRender* render ) const
{
	PhysicsBody* body = m_bodyList;

	while ( body )
	{
		body->Render( render );
		body = body->m_next;
	}

	m_contactManager.RenderContacts( render );
	//m_contactManager.m_broadphase.m_tree.Render( render );
}

//--------------------------------------------------------------------------------------------------
const glm::vec3 PhysicsScene::GetGravity( ) const
{
	return m_gravity;
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::SetGravity( const glm::vec3& gravity )
{
	m_gravity = gravity;
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::Shutdown( )
{
	RemoveAllBodies( );

	m_boxAllocator.Clear( );
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::SetContactListener( PhysicsContactListener* listener )
{
	m_contactManager.m_contactListener = listener;
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::QueryAABB( PhysicsQueryCallback *cb, const PhysicsAABB& aabb ) const
{
	struct SceneQueryWrapper
	{
		bool TreeCallBack( int id )
		{
			PhysicsAABB aabb;
			PhysicsBox *box = (PhysicsBox *)broadPhase->m_tree.GetUserData( id );

			box->ComputeAABB( box->body->GetTransform( ), &aabb );

			if ( PhysicsAABBtoAABB( m_aabb, aabb ) )
			{
				return cb->ReportShape( box );
			}

			return true;
		}

		PhysicsQueryCallback *cb;
		const PhysicsBroadPhase *broadPhase;
		PhysicsAABB m_aabb;
	};

	SceneQueryWrapper wrapper;
	wrapper.m_aabb = aabb;
	wrapper.broadPhase = &m_contactManager.m_broadphase;
	wrapper.cb = cb;
	m_contactManager.m_broadphase.m_tree.Query( &wrapper, aabb );
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::QueryPoint( PhysicsQueryCallback *cb, const glm::vec3& point ) const
{
	struct SceneQueryWrapper
	{
		bool TreeCallBack( int id )
		{
			PhysicsBox *box = (PhysicsBox *)broadPhase->m_tree.GetUserData( id );

			if ( box->TestPoint( box->body->GetTransform( ), m_point ) )
			{
				cb->ReportShape( box );
			}

			return true;
		}

		PhysicsQueryCallback *cb;
		const PhysicsBroadPhase *broadPhase;
		glm::vec3 m_point;
	};

	SceneQueryWrapper wrapper;
	wrapper.m_point = point;
	wrapper.broadPhase = &m_contactManager.m_broadphase;
	wrapper.cb = cb;
	const float k_fattener = float( 0.5 );
	glm::vec3 v( k_fattener, k_fattener, k_fattener );
	PhysicsAABB aabb;
	aabb.min = point - v;
	aabb.max = point + v;
	m_contactManager.m_broadphase.m_tree.Query( &wrapper, aabb );
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::RayCast( PhysicsQueryCallback *cb, PhysicsRaycastData& rayCast ) const
{
	struct SceneQueryWrapper
	{
		bool TreeCallBack( int id )
		{
			PhysicsBox *box = (PhysicsBox *)broadPhase->m_tree.GetUserData( id );

			if ( box->Raycast( box->body->GetTransform( ), m_rayCast ) )
			{
				return cb->ReportShape( box );
			}

			return true;
		}

		PhysicsQueryCallback *cb;
		const PhysicsBroadPhase *broadPhase;
		PhysicsRaycastData *m_rayCast;
	};
	
	SceneQueryWrapper wrapper;
	wrapper.m_rayCast = &rayCast;
	wrapper.broadPhase = &m_contactManager.m_broadphase;
	wrapper.cb = cb;
	m_contactManager.m_broadphase.m_tree.Query( &wrapper, rayCast );
}

//--------------------------------------------------------------------------------------------------
void PhysicsScene::Dump( FILE* file ) const
{
	fprintf( file, "// Ensure 64/32-bit memory compatability with the dump contents\n" );
	fprintf( file, "assert( sizeof( int* ) == %lu );\n", sizeof( int* ) );
	fprintf( file, "scene.SetGravity( glm::vec3( %.15lf, %.15lf, %.15lf ) );\n", m_gravity.x, m_gravity.y, m_gravity.z );
	fprintf( file, "scene.SetAllowSleep( %s );\n", m_allowSleep ? "true" : "false" );
	fprintf( file, "scene.SetEnableFriction( %s );\n", m_enableFriction ? "true" : "false" );

	fprintf( file, "PhysicsBody** bodies = (PhysicsBody**)PhysicsAlloc( sizeof( PhysicsBody* ) * %d );\n", m_bodyCount );

	int i = 0;
	for ( PhysicsBody* body = m_bodyList; body; body = body->m_next, ++i )
	{
		body->Dump( file, i );
	}

	fprintf( file, "PhysicsFree( bodies );\n" );
}
