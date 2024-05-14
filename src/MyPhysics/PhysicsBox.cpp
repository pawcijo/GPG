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



#include "PhysicsBox.h"

#include "PhysicsRender.h"
#include "PhysicsTransform.h"


bool PhysicsBox::TestPoint( const PhysicsTransform& tx, const glm::vec3& p ) const
{
	PhysicsTransform world = TransformMul( tx, local );
	glm::vec3 p0 = TransformMulTranspose( world, p );

	for ( int i = 0; i < 3; ++i )
	{
		float d = p0[ i ];
		float ei = e[ i ];

		if ( d > ei || d < -ei )
		{
			return false;
		}
	}

	return true;
}

//--------------------------------------------------------------------------------------------------
bool PhysicsBox::Raycast( const PhysicsTransform& tx, PhysicsRaycastData* raycast ) const
{
	PhysicsTransform world = TransformMul( tx, local );
	glm::vec3 d = TransformMulTranspose( world.rotation, raycast->dir );
	glm::vec3 p = TransformMulTranspose( world, raycast->start );
	const float epsilon = float( 1.0e-8 );
	float tmin = 0;
	float tmax = raycast->t;

	// t = (e[ i ] - p.[ i ]) / d[ i ]
	float t0;
	float t1;
	glm::vec3 n0;

	for ( int i = 0; i < 3; ++i )
	{
		// Check for ray parallel to and outside of AABB
		if ( Abs( d[ i ] ) < epsilon )
		{
			// Detect separating axes
			if ( p[ i ] < -e[ i ] || p[ i ] > e[ i ] )
			{
				return false;
			}
		}

		else
		{
			float d0 = float( 1.0 ) / d[ i ];
			float s = PhysicsSign( d[ i ] );
			float ei = e[ i ] * s;
			glm::vec3 n( 0, 0, 0 );
			n[ i ] = -s;

			t0 = -(ei + p[ i ]) * d0;
			t1 = (ei - p[ i ]) * d0;

			if ( t0 > tmin )
			{
				n0 = n;
				tmin = t0;
			}

			tmax = glm::min( tmax, t1 );

			if ( tmin > tmax )
			{
				return false;
			}
		}
	}

	raycast->normal = TransformMul( world.rotation, n0 ); 
	raycast->toi = tmin;

	return true;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBox::ComputeAABB( const PhysicsTransform& tx, PhysicsAABB* aabb ) const
{
	PhysicsTransform world = TransformMul( tx, local );

	glm::vec3 v[ 8 ] = {
		glm::vec3( -e.x, -e.y, -e.z ),
		glm::vec3( -e.x, -e.y,  e.z ),
		glm::vec3( -e.x,  e.y, -e.z ),
		glm::vec3( -e.x,  e.y,  e.z ),
		glm::vec3(  e.x, -e.y, -e.z ),
		glm::vec3(  e.x, -e.y,  e.z ),
		glm::vec3(  e.x,  e.y, -e.z ),
		glm::vec3(  e.x,  e.y,  e.z )
	};

	for ( int i = 0; i < 8; ++i )
		v[ i ] = TransformMul( world, v[ i ] );

	glm::vec3 min( Q3_R32_MAX, Q3_R32_MAX, Q3_R32_MAX );
	glm::vec3 max( -Q3_R32_MAX, -Q3_R32_MAX, -Q3_R32_MAX );

	for ( int i = 0; i < 8; ++i )
	{
		min = glm::min( min, v[ i ] );
		max = glm::max( max, v[ i ] );
	}

	aabb->min = min;
	aabb->max = max;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBox::ComputeMass( PhysicsMassData* md ) const
{
	// Calculate inertia tensor
	float ex2 = float( 4.0 ) * e.x * e.x;
	float ey2 = float( 4.0 ) * e.y * e.y;
	float ez2 = float( 4.0 ) * e.z * e.z;
	float mass = float( 8.0 ) * e.x * e.y * e.z * density;
	float x = float( 1.0 / 12.0 ) * mass * (ey2 + ez2);
	float y = float( 1.0 / 12.0 ) * mass * (ex2 + ez2);
	float z = float( 1.0 / 12.0 ) * mass * (ex2 + ey2);
	glm::mat3 I = PhysicsDiagonal( x, y, z );

	// Transform tensor to local space
	I = local.rotation * I * glm::transpose( local.rotation );
	glm::mat3 identity;
	identity = glm::mat3{1};
	I += (identity * glm::dot( local.position, local.position ) - PhysicsOuterProduct( local.position, local.position )) * mass;

	md->center = local.position;
	md->inertia = I;
	md->mass = mass;
}

//--------------------------------------------------------------------------------------------------
const int kBoxIndices[ 36 ] = {
	1 - 1, 7 - 1, 5 - 1,
	1 - 1, 3 - 1, 7 - 1,
	1 - 1, 4 - 1, 3 - 1,
	1 - 1, 2 - 1, 4 - 1,
	3 - 1, 8 - 1, 7 - 1,
	3 - 1, 4 - 1, 8 - 1,
	5 - 1, 7 - 1, 8 - 1,
	5 - 1, 8 - 1, 6 - 1,
	1 - 1, 5 - 1, 6 - 1,
	1 - 1, 6 - 1, 2 - 1,
	2 - 1, 6 - 1, 8 - 1,
	2 - 1, 8 - 1, 4 - 1
};

//--------------------------------------------------------------------------------------------------
void PhysicsBox::Render( const PhysicsTransform& tx, bool awake, PhysicsRender* render ) const
{
	PhysicsTransform world = TransformMul( tx, local );

	glm::vec3 vertices[ 8 ] = {
		glm::vec3( -e.x, -e.y, -e.z ),
		glm::vec3( -e.x, -e.y,  e.z ),
		glm::vec3( -e.x,  e.y, -e.z ),
		glm::vec3( -e.x,  e.y,  e.z ),
		glm::vec3(  e.x, -e.y, -e.z ),
		glm::vec3(  e.x, -e.y,  e.z ),
		glm::vec3(  e.x,  e.y, -e.z ),
		glm::vec3(  e.x,  e.y,  e.z )
	};

	for ( int i = 0; i < 36; i += 3 )
	{
		glm::vec3 a = TransformMul( world, vertices[ kBoxIndices[ i ] ] );
		glm::vec3 b = TransformMul( world, vertices[ kBoxIndices[ i + 1 ] ] );
		glm::vec3 c = TransformMul( world, vertices[ kBoxIndices[ i + 2 ] ] );

		glm::vec3 n = glm::normalize( glm::cross( b - a, c - a ) );

		//render->SetPenColor( 0.2f, 0.4f, 0.7f, 0.5f );
		//render->SetPenPosition( a.x, a.y, a.z );
		//render->Line( b.x, b.y, b.z );
		//render->Line( c.x, c.y, c.z );
		//render->Line( a.x, a.y, a.z );

		render->SetTriNormal( n.x, n.y, n.z );
		render->Triangle( a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z );
	}
}
