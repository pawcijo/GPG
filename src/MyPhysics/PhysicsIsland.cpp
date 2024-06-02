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


#include "PhysicsIsland.h"


#include "Common.h"

#include "PhysicsSettings.h"
#include "PhysicsBody.h"
#include "PhysicsMemory.h"
#include "PhysicsContactSolver.h"
#include "PhysicsBroadPhase.h"
#include "PhysicsContact.h"



//--------------------------------------------------------------------------------------------------
// PhysicsIsland
//--------------------------------------------------------------------------------------------------
void PhysicsIsland::Solve( float deltaTime)
{
	// Apply gravity
	// Integrate velocities and create state buffers, calculate world inertia
	for ( int i = 0 ; i < m_bodyCount; ++i )
	{
		PhysicsBody *body = m_bodies[ i ];
		PhysicsVelocityState *v = m_velocities + i;

		if ( body->m_flags & PhysicsBody::eDynamic )
		{
			body->ApplyLinearForce( m_gravity * body->m_gravityScale );

			// Calculate world space intertia tensor
			glm::mat3 r = body->m_tx.rotation;
			body->m_invInertiaWorld = r * body->m_invInertiaModel * glm::transpose( r );

			// Integrate velocity
			body->m_linearVelocity += (body->m_force * body->m_invMass) * deltaTime;
			body->m_angularVelocity += (body->m_invInertiaWorld * body->m_torque) * deltaTime;

			// From Box2D!
			// Apply damping.
			// ODE: dv/dt + c * v = 0
			// Solution: v(t) = v0 * exp(-c * t)
			// Time step: v(t + dt) = v0 * exp(-c * (t + dt)) = v0 * exp(-c * t) * exp(-c * dt) = v * exp(-c * dt)
			// v2 = exp(-c * dt) * v1
			// Pade approximation:
			// v2 = v1 * 1 / (1 + c * dt)
			body->m_linearVelocity *= float( 1.0 ) / (float( 1.0 ) + deltaTime * body->m_linearDamping);
			body->m_angularVelocity *= float( 1.0 ) / (float( 1.0 ) + deltaTime * body->m_angularDamping);
		}

		v->v = body->m_linearVelocity;
		v->w = body->m_angularVelocity;
	}

	// Create contact solver, pass in state buffers, create buffers for contacts
	// Initialize velocity constraint for normal + friction and warm start
	PhysicsContactSolver contactSolver;
	contactSolver.Initialize( this );
	contactSolver.PreSolve( deltaTime );

	// Solve contacts
	for ( int i = 0; i < m_iterations; ++i )
		contactSolver.Solve( );

	contactSolver.ShutDown( );

	// Copy back state buffers
	// Integrate positions
	for ( int i = 0 ; i < m_bodyCount; ++i )
	{
		PhysicsBody *body = m_bodies[ i ];
		PhysicsVelocityState *v = m_velocities + i;

		if ( body->m_flags & PhysicsBody::eStatic )
			continue;

		body->m_linearVelocity = v->v;
		body->m_angularVelocity = v->w;

		// Integrate position
		body->m_worldCenter += body->m_linearVelocity * deltaTime;
		Integrate(body->m_q,body->m_angularVelocity, deltaTime);
		body->m_q = glm::normalize( body->m_q );
		body->m_tx.rotation = glm::mat3{body->m_q};
	}

	if ( m_allowSleep )
	{
		// Find minimum sleep time of the entire island
		float minSleepTime = Q3_R32_MAX;
		for ( int i = 0; i < m_bodyCount; ++i )
		{
			PhysicsBody* body = m_bodies[ i ];

			if ( body->m_flags & PhysicsBody::eStatic )
				continue;

			const float sqrLinVel = glm::dot( body->m_linearVelocity, body->m_linearVelocity );
			const float cbAngVel = glm::dot( body->m_angularVelocity, body->m_angularVelocity );
			const float linTol = Q3_SLEEP_LINEAR;
			const float angTol = Q3_SLEEP_ANGULAR;

			if ( sqrLinVel > linTol || cbAngVel > angTol )
			{
				minSleepTime = float( 0.0 );
				body->m_sleepTime = float( 0.0 );
			}

			else
			{
				body->m_sleepTime += deltaTime;
				minSleepTime = glm::min( minSleepTime, body->m_sleepTime );
			}
		}

		// Put entire island to sleep so long as the minimum found sleep time
		// is below the threshold. If the minimum sleep time reaches below the
		// sleeping threshold, the entire island will be reformed next step
		// and sleep test will be tried again.
		if ( minSleepTime > Q3_SLEEP_TIME )
		{
			for ( int i = 0; i < m_bodyCount; ++i )
				m_bodies[ i ]->SetToSleep( );
		}
	}
}

//--------------------------------------------------------------------------------------------------
void PhysicsIsland::Add( PhysicsBody *body )
{
	assert( m_bodyCount < m_bodyCapacity );

	body->m_islandIndex = m_bodyCount;

	m_bodies[ m_bodyCount++ ] = body;
}

//--------------------------------------------------------------------------------------------------
void PhysicsIsland::Add( PhysicsContactConstraint *contact )
{
	assert( m_contactCount < m_contactCapacity );

	m_contacts[ m_contactCount++ ] = contact;
}

//--------------------------------------------------------------------------------------------------
void PhysicsIsland::Initialize( )
{
	for ( int i = 0; i < m_contactCount; ++i )
	{
		PhysicsContactConstraint *cc = m_contacts[ i ];

		PhysicsContactConstraintState *c = m_contactStates + i;

		c->centerA = cc->bodyA->m_worldCenter;
		c->centerB = cc->bodyB->m_worldCenter;
		c->iA = cc->bodyA->m_invInertiaWorld;
		c->iB = cc->bodyB->m_invInertiaWorld;
		c->mA = cc->bodyA->m_invMass;
		c->mB = cc->bodyB->m_invMass;
		c->restitution = cc->restitution;
		c->friction = cc->friction;
		c->indexA = cc->bodyA->m_islandIndex;
		c->indexB = cc->bodyB->m_islandIndex;
		c->normal = cc->manifold.normal;
		c->tangentVectors[ 0 ] = cc->manifold.tangentVectors[ 0 ];
		c->tangentVectors[ 1 ] = cc->manifold.tangentVectors[ 1 ];
		c->contactCount = cc->manifold.contactCount;

		for ( int j = 0; j < c->contactCount; ++j )
		{
			PhysicsContactState *s = c->contacts + j;
			PhysicsContact *cp = cc->manifold.contacts + j;
			s->ra = cp->position - c->centerA;
			s->rb = cp->position - c->centerB;
			s->penetration = cp->penetration;
			s->normalImpulse = cp->normalImpulse;
			s->tangentImpulse[ 0 ] = cp->tangentImpulse[ 0 ];
			s->tangentImpulse[ 1 ] = cp->tangentImpulse[ 1 ];
		}
	}
}
