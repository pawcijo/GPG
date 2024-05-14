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


#include "PhysicsContactSolver.h"

#include "glm/glm.hpp"


#include "PhysicsContact.h"
#include "PhysicsIsland.h"
//#include "../common/q3Memory.h"
#include "PhysicsBody.h"

#include "Common.h"
#include "PhysicsSettings.h"


//--------------------------------------------------------------------------------------------------
// PhysicsContactSolver
//--------------------------------------------------------------------------------------------------
void PhysicsContactSolver::Initialize( PhysicsIsland *island )
{
	m_island = island;
	m_contactCount = island->m_contactCount;
	m_contacts = island->m_contactStates;
	m_velocities = m_island->m_velocities;
	m_enableFriction = island->m_enableFriction;
}

//--------------------------------------------------------------------------------------------------
void PhysicsContactSolver::ShutDown( void )
{
	for ( int i = 0; i < m_contactCount; ++i )
	{
		PhysicsContactConstraintState *c = m_contacts + i;
		PhysicsContactConstraint *cc = m_island->m_contacts[ i ];

		for ( int j = 0; j < c->contactCount; ++j )
		{
			PhysicsContact *oc = cc->manifold.contacts + j;
			PhysicsContactState *cs = c->contacts + j;
			oc->normalImpulse = cs->normalImpulse;
			oc->tangentImpulse[ 0 ] = cs->tangentImpulse[ 0 ];
			oc->tangentImpulse[ 1 ] = cs->tangentImpulse[ 1 ];
		}
	}
}

//--------------------------------------------------------------------------------------------------
void PhysicsContactSolver::PreSolve( float dt )
{
	for ( int i = 0; i < m_contactCount; ++i )
	{
		PhysicsContactConstraintState *cs = m_contacts + i;

		glm::vec3 vA = m_velocities[ cs->indexA ].v;
		glm::vec3 wA = m_velocities[ cs->indexA ].w;
		glm::vec3 vB = m_velocities[ cs->indexB ].v;
		glm::vec3 wB = m_velocities[ cs->indexB ].w;

		for ( int j = 0; j < cs->contactCount; ++j )
		{
			PhysicsContactState *c = cs->contacts + j;

			// Precalculate JM^-1JT for contact and friction constraints
			glm::vec3 raCn = glm::cross( c->ra, cs->normal );
			glm::vec3 rbCn = glm::cross( c->rb, cs->normal );
			float nm = cs->mA + cs->mB;
			float tm[ 2 ];
			tm[ 0 ] = nm;
			tm[ 1 ] = nm;

			nm += glm::dot( raCn, cs->iA * raCn ) + glm::dot( rbCn, cs->iB * rbCn );
			c->normalMass = InvertFloat( nm );

			for ( int i = 0; i < 2; ++i )
			{
				glm::vec3 raCt = glm::cross( cs->tangentVectors[ i ], c->ra );
				glm::vec3 rbCt = glm::cross( cs->tangentVectors[ i ], c->rb );
				tm[ i ] += glm::dot( raCt, cs->iA * raCt ) + glm::dot( rbCt, cs->iB * rbCt );
				c->tangentMass[ i ] = InvertFloat( tm[ i ] );
			}

			// Precalculate bias factor
			c->bias = -Q3_BAUMGARTE * (float( 1.0 ) / dt) * glm::min( float( 0.0 ), c->penetration + Q3_PENETRATION_SLOP );

			// Warm start contact
			glm::vec3 P = cs->normal * c->normalImpulse;

			if ( m_enableFriction )
			{
				P += cs->tangentVectors[ 0 ] * c->tangentImpulse[ 0 ];
				P += cs->tangentVectors[ 1 ] * c->tangentImpulse[ 1 ];
			}

			vA -= P * cs->mA;
			wA -= cs->iA * glm::cross( c->ra, P );

			vB += P * cs->mB;
			wB += cs->iB * glm::cross( c->rb, P );

			// Add in restitution bias
			float dv = glm::dot( vB + glm::cross( wB, c->rb ) - vA - glm::cross( wA, c->ra ), cs->normal );

			if ( dv < -float( 1.0 ) )
				c->bias += -(cs->restitution) * dv;
		}

		m_velocities[ cs->indexA ].v = vA;
		m_velocities[ cs->indexA ].w = wA;
		m_velocities[ cs->indexB ].v = vB;
		m_velocities[ cs->indexB ].w = wB;
	}
}

//--------------------------------------------------------------------------------------------------
void PhysicsContactSolver::Solve( )
{
	for ( int i = 0; i < m_contactCount; ++i )
	{
		PhysicsContactConstraintState *cs = m_contacts + i;

		glm::vec3 vA = m_velocities[ cs->indexA ].v;
		glm::vec3 wA = m_velocities[ cs->indexA ].w;
		glm::vec3 vB = m_velocities[ cs->indexB ].v;
		glm::vec3 wB = m_velocities[ cs->indexB ].w;

		for ( int j = 0; j < cs->contactCount; ++j )
		{
			PhysicsContactState *c = cs->contacts + j;

			// relative velocity at contact
			glm::vec3 dv = vB + glm::cross( wB, c->rb ) - vA - glm::cross( wA, c->ra );

			// Friction
			if ( m_enableFriction )
			{
				for ( int i = 0; i < 2; ++i )
				{
					float lambda = -glm::dot( dv, cs->tangentVectors[ i ] ) * c->tangentMass[ i ];

					// Calculate frictional impulse
					float maxLambda = cs->friction * c->normalImpulse;

					// Clamp frictional impulse
					float oldPT = c->tangentImpulse[ i ];
					c->tangentImpulse[ i ] = glm::clamp( -maxLambda, maxLambda, oldPT + lambda );
					lambda = c->tangentImpulse[ i ] - oldPT;

					// Apply friction impulse
					glm::vec3 impulse = cs->tangentVectors[ i ] * lambda;
					vA -= impulse * cs->mA;
					wA -= cs->iA * glm::cross( c->ra, impulse );

					vB += impulse * cs->mB;
					wB += cs->iB * glm::cross( c->rb, impulse );
				}
			}

			// Normal
			{
				dv = vB + glm::cross( wB, c->rb ) - vA - glm::cross( wA, c->ra );

				// Normal impulse
				float vn = glm::dot( dv, cs->normal );

				// Factor in positional bias to calculate impulse scalar j
				float lambda = c->normalMass * (-vn + c->bias);

				// Clamp impulse
				float tempPN = c->normalImpulse;
				c->normalImpulse = glm::max( tempPN + lambda, float( 0.0 ) );
				lambda = c->normalImpulse - tempPN;

				// Apply impulse
				glm::vec3 impulse = cs->normal * lambda;
				vA -= impulse * cs->mA;
				wA -= cs->iA * glm::cross( c->ra, impulse );

				vB += impulse * cs->mB;
				wB += cs->iB * glm::cross( c->rb, impulse );
			}
		}

		m_velocities[ cs->indexA ].v = vA;
		m_velocities[ cs->indexA ].w = wA;
		m_velocities[ cs->indexB ].v = vB;
		m_velocities[ cs->indexB ].w = wB;
	}
}
