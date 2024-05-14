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


#include "PhysicsCollide.h"

#include "PhysicsBody.h"
#include "PhysicsContact.h"

//--------------------------------------------------------------------------------------------------
// qBoxtoBox
//--------------------------------------------------------------------------------------------------
inline bool PhysicsTrackFaceAxis( int* axis, int n, float s, float* sMax, const glm::vec3& normal, glm::vec3* axisNormal )
{
	if ( s > float( 0.0 ) )
		return true;

	if ( s > *sMax )
	{
		*sMax = s;
		*axis = n;
		*axisNormal = normal;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------
inline bool PhysicsTrackEdgeAxis( int* axis, int n, float s, float* sMax, const glm::vec3& normal, glm::vec3* axisNormal )
{
	if ( s > float( 0.0 ) )
		return true;

	float l = float( 1.0 ) / glm::length( normal );
	s *= l;

	if ( s > *sMax )
	{
		*sMax = s;
		*axis = n;
		*axisNormal = normal * l;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------
struct PhysicsClipVertex
{
	PhysicsClipVertex( )
	{
		f.key = ~0;
	}

	glm::vec3 v;
	PhysicsFeaturePair f;
};

//--------------------------------------------------------------------------------------------------
void PhysicsComputeReferenceEdgesAndBasis( const glm::vec3& eR, const PhysicsTransform& rtx, glm::vec3 n, int axis, unsigned char* out, glm::mat3* basis, glm::vec3* e )
{
	n = TransformMulTranspose( rtx.rotation, n );

	if ( axis >= 3 )
		axis -= 3;

	switch ( axis )
	{
	case 0:
		if ( n.x > float( 0.0 ) )
		{
			out[ 0 ] = 1;
			out[ 1 ] = 8;
			out[ 2 ] = 7;
			out[ 3 ] = 9;

			*e = glm::vec3( eR.y, eR.z, eR.x );
			*basis = glm::mat3( rtx.rotation[1], rtx.rotation[2], rtx.rotation[0] );
		}

		else
		{
			out[ 0 ] = 11;
			out[ 1 ] = 3;
			out[ 2 ] = 10;
			out[ 3 ] = 5;

			*e = glm::vec3( eR.z, eR.y, eR.x );
			*basis = glm::mat3( rtx.rotation[2], rtx.rotation[1], -rtx.rotation[0] );
		}
		break;

	case 1:
		if ( n.y > float( 0.0 ) )
		{
			out[ 0 ] = 0;
			out[ 1 ] = 1;
			out[ 2 ] = 2;
			out[ 3 ] = 3;

			*e = glm::vec3( eR.z, eR.x, eR.y );
			*basis = glm::mat3( rtx.rotation[2], rtx.rotation[0], rtx.rotation[1] );
		}

		else
		{
			out[ 0 ] = 4;
			out[ 1 ] = 5;
			out[ 2 ] = 6;
			out[ 3 ] = 7;

			*e = glm::vec3( eR.z, eR.x, eR.y );
			*basis = glm::mat3( rtx.rotation[2], -rtx.rotation[0], -rtx.rotation[1] );
		}
		break;

	case 2:
		if ( n.z > float( 0.0 ) )
		{
			out[ 0 ] = 11;
			out[ 1 ] = 4;
			out[ 2 ] = 8;
			out[ 3 ] = 0;

			*e = glm::vec3( eR.y, eR.x, eR.z );
			*basis = glm::mat3( -rtx.rotation[1], rtx.rotation[0], rtx.rotation[2] );
		}

		else
		{
			out[ 0 ] = 6;
			out[ 1 ] = 10;
			out[ 2 ] = 2;
			out[ 3 ] = 9;

			*e = glm::vec3( eR.y, eR.x, eR.z );
			*basis = glm::mat3( -rtx.rotation[1], -rtx.rotation[0], -rtx.rotation[2] );
		}
		break;
	}
}

//--------------------------------------------------------------------------------------------------
void PhysicsComputeIncidentFace( const PhysicsTransform& itx, const glm::vec3& e, glm::vec3 n, PhysicsClipVertex* out )
{
	n = -TransformMulTranspose( itx.rotation, n );
	glm::vec3 absN = Abs( n );

	if ( absN.x > absN.y && absN.x > absN.z )
	{
		if ( n.x > float( 0.0 ) )
		{
			out[ 0 ].v = glm::vec3(  e.x,  e.y, -e.z );
			out[ 1 ].v = glm::vec3(  e.x,  e.y,  e.z );
			out[ 2 ].v = glm::vec3(  e.x, -e.y,  e.z );
			out[ 3 ].v = glm::vec3(  e.x, -e.y, -e.z );

			out[ 0 ].f.inI = 9;
			out[ 0 ].f.outI = 1;
			out[ 1 ].f.inI = 1;
			out[ 1 ].f.outI = 8;
			out[ 2 ].f.inI = 8;
			out[ 2 ].f.outI = 7;
			out[ 3 ].f.inI = 7;
			out[ 3 ].f.outI = 9;
		}

		else
		{
			out[ 0 ].v = glm::vec3( -e.x, -e.y,  e.z );
			out[ 1 ].v = glm::vec3( -e.x,  e.y,  e.z );
			out[ 2 ].v = glm::vec3( -e.x,  e.y, -e.z );
			out[ 3 ].v = glm::vec3( -e.x, -e.y, -e.z );

			out[ 0 ].f.inI = 5;
			out[ 0 ].f.outI = 11;
			out[ 1 ].f.inI = 11;
			out[ 1 ].f.outI = 3;
			out[ 2 ].f.inI = 3;
			out[ 2 ].f.outI = 10;
			out[ 3 ].f.inI = 10;
			out[ 3 ].f.outI = 5;
		}
	}

	else if ( absN.y > absN.x && absN.y > absN.z )
	{
		if ( n.y > float( 0.0 ) )
		{
			out[ 0 ].v = glm::vec3 ( -e.x,  e.y,  e.z );
			out[ 1 ].v = glm::vec3(  e.x,  e.y,  e.z );
			out[ 2 ].v = glm::vec3(  e.x,  e.y, -e.z );
			out[ 3 ].v = glm::vec3( -e.x,  e.y, -e.z );

			out[ 0 ].f.inI = 3;
			out[ 0 ].f.outI = 0;
			out[ 1 ].f.inI = 0;
			out[ 1 ].f.outI = 1;
			out[ 2 ].f.inI = 1;
			out[ 2 ].f.outI = 2;
			out[ 3 ].f.inI = 2;
			out[ 3 ].f.outI = 3;
		}

		else
		{
			out[ 0 ].v = glm::vec3(  e.x, -e.y,  e.z );
			out[ 1 ].v = glm::vec3( -e.x, -e.y,  e.z );
			out[ 2 ].v = glm::vec3( -e.x, -e.y, -e.z );
			out[ 3 ].v = glm::vec3(  e.x, -e.y, -e.z );

			out[ 0 ].f.inI = 7;
			out[ 0 ].f.outI = 4;
			out[ 1 ].f.inI = 4;
			out[ 1 ].f.outI = 5;
			out[ 2 ].f.inI = 5;
			out[ 2 ].f.outI = 6;
			out[ 3 ].f.inI = 5;
			out[ 3 ].f.outI = 6;
		}
	}

	else
	{
		if ( n.z > float( 0.0 ) )
		{
			out[ 0 ].v = glm::vec3( -e.x,  e.y,  e.z );
			out[ 1 ].v = glm::vec3( -e.x, -e.y,  e.z );
			out[ 2 ].v = glm::vec3(  e.x, -e.y,  e.z );
			out[ 3 ].v = glm::vec3(  e.x,  e.y,  e.z );

			out[ 0 ].f.inI = 0;
			out[ 0 ].f.outI = 11;
			out[ 1 ].f.inI = 11;
			out[ 1 ].f.outI = 4;
			out[ 2 ].f.inI = 4;
			out[ 2 ].f.outI = 8;
			out[ 3 ].f.inI = 8;
			out[ 3 ].f.outI = 0;
		}

		else
		{
			out[ 0 ].v = glm::vec3(  e.x, -e.y, -e.z );
			out[ 1 ].v = glm::vec3( -e.x, -e.y, -e.z );
			out[ 2 ].v = glm::vec3( -e.x,  e.y, -e.z );
			out[ 3 ].v = glm::vec3(  e.x,  e.y, -e.z );

			out[ 0 ].f.inI = 9;
			out[ 0 ].f.outI = 6;
			out[ 1 ].f.inI = 6;
			out[ 1 ].f.outI = 10;
			out[ 2 ].f.inI = 10;
			out[ 2 ].f.outI = 2;
			out[ 3 ].f.inI = 2;
			out[ 3 ].f.outI = 9;
		}
	}

	for ( int i = 0; i < 4; ++i )
		out[ i ].v = TransformMul( itx, out[ i ].v );
}

//--------------------------------------------------------------------------------------------------
#define InFront( a ) \
	((a) < float( 0.0 ))

#define Behind( a ) \
	((a) >= float( 0.0 ))

#define On( a ) \
	((a) < float( 0.005 ) && (a) > -float( 0.005 ))

int PhysicsOrthographic( float sign, float e, int axis, int clipEdge, PhysicsClipVertex* in, int inCount, PhysicsClipVertex* out )
{
	int outCount = 0;
	PhysicsClipVertex a = in[ inCount - 1 ];

	for ( int i = 0; i < inCount; ++i )
	{
		PhysicsClipVertex b = in[ i ];

		float da = sign * a.v[ axis ] - e;
		float db = sign * b.v[ axis ] - e;

		PhysicsClipVertex cv;

		// B
		if ( ((InFront( da ) && InFront( db )) || On( da ) || On( db )) )
		{
			assert( outCount < 8 );
			out[ outCount++ ] = b;
		}

		// I
		else if ( InFront( da ) && Behind( db ) )
		{
			cv.f = b.f;
			cv.v = a.v + (b.v - a.v) * (da / (da - db));
			cv.f.outR = clipEdge;
			cv.f.outI = 0;
			assert( outCount < 8 );
			out[ outCount++ ] = cv;
		}

		// I, B
		else if ( Behind( da ) && InFront( db ) )
		{
			cv.f = a.f;
			cv.v = a.v + (b.v - a.v) * (da / (da - db));
			cv.f.inR = clipEdge;
			cv.f.inI = 0;
			assert( outCount < 8 );
			out[ outCount++ ] = cv;

			assert( outCount < 8 );
			out[ outCount++ ] = b;
		}

		a = b;
	}

	return outCount;
}

//--------------------------------------------------------------------------------------------------
// Resources (also see PhysicsBoxtoBox's resources):
// http://www.randygaul.net/2013/10/27/sutherland-hodgman-clipping/
int PhysicsClip( const glm::vec3& rPos, const glm::vec3& e, unsigned char* clipEdges, const glm::mat3& basis, PhysicsClipVertex* incident, PhysicsClipVertex* outVerts, float* outDepths )
{
	int inCount = 4;
	int outCount;
	PhysicsClipVertex in[ 8 ];
	PhysicsClipVertex out[ 8 ];

	for ( int i = 0; i < 4; ++i )
		in[ i ].v = TransformMulTranspose( basis, incident[ i ].v - rPos );

	outCount = PhysicsOrthographic( float( 1.0 ), e.x, 0, clipEdges[ 0 ], in, inCount, out );

	if ( !outCount )
		return 0;

	inCount = PhysicsOrthographic( float( 1.0 ), e.y, 1, clipEdges[ 1 ], out, outCount, in );

	if ( !inCount )
		return 0;

	outCount = PhysicsOrthographic( float( -1.0 ), e.x, 0, clipEdges[ 2 ], in, inCount, out );

	if ( !outCount )
		return 0;

	inCount = PhysicsOrthographic( float( -1.0 ), e.y, 1, clipEdges[ 3 ], out, outCount, in );

	// Keep incident vertices behind the reference face
	outCount = 0;
	for ( int i = 0; i < inCount; ++i )
	{
		float d = in[ i ].v.z - e.z;

		if ( d <= float( 0.0 ) )
		{
			outVerts[ outCount ].v = TransformMul( basis, in[ i ].v ) + rPos;
			outVerts[ outCount ].f = in[ i ].f;
			outDepths[ outCount++ ] = d;
		}
	}

	assert( outCount <= 8 );

	return outCount;
}

//--------------------------------------------------------------------------------------------------
inline void PhysicsEdgesContact( glm::vec3 *CA, glm::vec3 *CB, const glm::vec3& PA, const glm::vec3& QA, const glm::vec3& PB, const glm::vec3& QB )
{
	glm::vec3 DA = QA - PA;
	glm::vec3 DB = QB - PB;
	glm::vec3 r = PA - PB;
	float a = glm::dot( DA, DA );
	float e = glm::dot( DB, DB );
	float f = glm::dot( DB, r );
	float c = glm::dot( DA, r );

	float b = glm::dot( DA, DB );
	float denom = a * e - b * b;

	float TA = (b * f - c * e) / denom;
	float TB = (b * TA + f) / e;

	*CA = PA + DA * TA;
	*CB = PB + DB * TB;
}

//--------------------------------------------------------------------------------------------------
void PhysicsSupportEdge( const PhysicsTransform& tx, const glm::vec3& e, glm::vec3 n, glm::vec3* aOut, glm::vec3* bOut )
{
	n = TransformMulTranspose( tx.rotation, n );
	glm::vec3 absN = Abs( n );
	glm::vec3 a, b;

	// x > y
	if ( absN.x > absN.y )
	{
		// x > y > z
		if ( absN.y > absN.z )
		{
			a = glm::vec3( e.x, e.y, e.z );
			b = glm::vec3( e.x, e.y, -e.z );
		}

		// x > z > y || z > x > y
		else
		{
			a = glm::vec3( e.x, e.y, e.z );
			b = glm::vec3( e.x, -e.y, e.z );
		}
	}

	// y > x
	else
	{
		// y > x > z
		if ( absN.x > absN.z )
		{
			a = glm::vec3( e.x, e.y, e.z );
			b = glm::vec3( e.x, e.y, -e.z );
		}

		// z > y > x || y > z > x
		else
		{
			a = glm::vec3( e.x, e.y, e.z );
			b = glm::vec3( -e.x, e.y, e.z );
		}
	}

	float signx = PhysicsSign( n.x );
	float signy = PhysicsSign( n.y );
	float signz = PhysicsSign( n.z );

	a.x *= signx;
	a.y *= signy;
	a.z *= signz;
	b.x *= signx;
	b.y *= signy;
	b.z *= signz;

	*aOut = TransformMul( tx, a );
	*bOut = TransformMul( tx, b );
}

//--------------------------------------------------------------------------------------------------
// Resources:
// http://www.randygaul.net/2014/05/22/deriving-obb-to-obb-intersection-sat/
// https://box2d.googlecode.com/files/GDC2007_ErinCatto.zip
// https://box2d.googlecode.com/files/Box2D_Lite.zip
void PhysicsBoxtoBox( PhysicsManifold* m, PhysicsBox* a, PhysicsBox* b )
{
	PhysicsTransform atx = a->body->GetTransform( );
	PhysicsTransform btx = b->body->GetTransform( );
	PhysicsTransform aL = a->local;
	PhysicsTransform bL = b->local;
	atx = TransformMul( atx, aL );
	btx = TransformMul( btx, bL );
	glm::vec3 eA = a->e;
	glm::vec3 eB = b->e;

	// B's frame in A's space
	glm::mat3 C = glm::transpose( atx.rotation ) * btx.rotation;

	glm::mat3 absC;
	bool parallel = false;
	const float kCosTol = float( 1.0e-6 );
	for ( int i = 0; i < 3; ++i )
	{
		for ( int j = 0; j < 3; ++j )
		{
			float val = Abs( C[ i ][ j ] );
			absC[ i ][ j ] = val;

			if ( val + kCosTol >= float( 1.0 ) )
				parallel = true;
		}
	}

	// Vector from center A to center B in A's space
	glm::vec3 t = TransformMulTranspose( atx.rotation, btx.position - atx.position );

	// Query states
	float s;
	float aMax = -Q3_R32_MAX;
	float bMax = -Q3_R32_MAX;
	float eMax = -Q3_R32_MAX;
	int aAxis = ~0;
	int bAxis = ~0;
	int eAxis = ~0;
	glm::vec3 nA;
	glm::vec3 nB;
	glm::vec3 nE;

	// Face axis checks

	// a's x axis
	s = Abs( t.x ) - (eA.x + glm::dot( glm::vec3(absC[0][0], absC[1][0], absC[2][0]), eB ));
	if ( PhysicsTrackFaceAxis( &aAxis, 0, s, &aMax, atx.rotation[0], &nA ) )
		return;

	// a's y axis
	s = Abs( t.y ) - (eA.y + glm::dot( glm::vec3(absC[0][1], absC[1][1], absC[2][1]), eB ));
	if ( PhysicsTrackFaceAxis( &aAxis, 1, s, &aMax, atx.rotation[1], &nA ) )
		return;

	// a's z axis
	s = Abs( t.z ) - (eA.z + glm::dot( glm::vec3(absC[0][2], absC[1][2], absC[2][2]), eB ));
	if ( PhysicsTrackFaceAxis( &aAxis, 2, s, &aMax, atx.rotation[2], &nA ) )
		return;

	// b's x axis
	s = Abs( glm::dot( t, C[0] ) ) - (eB.x + glm::dot( absC[0], eA ));
	if ( PhysicsTrackFaceAxis( &bAxis, 3, s, &bMax, btx.rotation[0], &nB ) )
		return;

	// b's y axis
	s = Abs( glm::dot( t, C[1] ) ) - (eB.y + glm::dot( absC[1], eA ));
	if ( PhysicsTrackFaceAxis( &bAxis, 4, s, &bMax, btx.rotation[1], &nB ) )
		return;

	// b's z axis
	s = Abs( glm::dot( t, C[2] ) ) - (eB.z + glm::dot( absC[2], eA ));
	if ( PhysicsTrackFaceAxis( &bAxis, 5, s, &bMax, btx.rotation[2], &nB ) )
		return;

	if ( !parallel )
	{
		// Edge axis checks
		float rA;
		float rB;

		// Cross( a.x, b.x )
		rA = eA.y * absC[ 0 ][ 2 ] + eA.z * absC[ 0 ][ 1 ];
		rB = eB.y * absC[ 2 ][ 0 ] + eB.z * absC[ 1 ][ 0 ];
		s = Abs( t.z * C[ 0 ][ 1 ] - t.y * C[ 0 ][ 2 ] ) - (rA + rB);
		if ( PhysicsTrackEdgeAxis( &eAxis, 6, s, &eMax, glm::vec3( float( 0.0 ), -C[ 0 ][ 2 ], C[ 0 ][ 1 ] ), &nE ) )
			return;

		// Cross( a.x, b.y )
		rA = eA.y * absC[ 1 ][ 2 ] + eA.z * absC[ 1 ][ 1 ];
		rB = eB.x * absC[ 2 ][ 0 ] + eB.z * absC[ 0 ][ 0 ];
		s = Abs( t.z * C[ 1 ][ 1 ] - t.y * C[ 1 ][ 2 ] ) - (rA + rB);
		if ( PhysicsTrackEdgeAxis( &eAxis, 7, s, &eMax, glm::vec3( float( 0.0 ), -C[ 1 ][ 2 ], C[ 1 ][ 1 ] ), &nE ) )
			return;

		// Cross( a.x, b.z )
		rA = eA.y * absC[ 2 ][ 2 ] + eA.z * absC[ 2 ][ 1 ];
		rB = eB.x * absC[ 1 ][ 0 ] + eB.y * absC[ 0 ][ 0 ];
		s = Abs( t.z * C[ 2 ][ 1 ] - t.y * C[ 2 ][ 2 ] ) - (rA + rB);
		if ( PhysicsTrackEdgeAxis( &eAxis, 8, s, &eMax, glm::vec3( float( 0.0 ), -C[ 2 ][ 2 ], C[ 2 ][ 1 ] ), &nE ) )
			return;

		// Cross( a.y, b.x )
		rA = eA.x * absC[ 0 ][ 2 ] + eA.z * absC[ 0 ][ 0 ];
		rB = eB.y * absC[ 2 ][ 1 ] + eB.z * absC[ 1 ][ 1 ];
		s = Abs( t.x * C[ 0 ][ 2 ] - t.z * C[ 0 ][ 0 ] ) - (rA + rB);
		if ( PhysicsTrackEdgeAxis( &eAxis, 9, s, &eMax, glm::vec3( C[ 0 ][ 2 ], float( 0.0 ), -C[ 0 ][ 0 ] ), &nE ) )
			return;

		// Cross( a.y, b.y )
		rA = eA.x * absC[ 1 ][ 2 ] + eA.z * absC[ 1 ][ 0 ];
		rB = eB.x * absC[ 2 ][ 1 ] + eB.z * absC[ 0 ][ 1 ];
		s = Abs( t.x * C[ 1 ][ 2 ] - t.z * C[ 1 ][ 0 ] ) - (rA + rB);
		if ( PhysicsTrackEdgeAxis( &eAxis, 10, s, &eMax, glm::vec3( C[ 1 ][ 2 ], float( 0.0 ), -C[ 1 ][ 0 ] ), &nE ) )
			return;

		// Cross( a.y, b.z )
		rA = eA.x * absC[ 2 ][ 2 ] + eA.z * absC[ 2 ][ 0 ];
		rB = eB.x * absC[ 1 ][ 1 ] + eB.y * absC[ 0 ][ 1 ];
		s = Abs( t.x * C[ 2 ][ 2 ] - t.z * C[ 2 ][ 0 ] ) - (rA + rB);
		if ( PhysicsTrackEdgeAxis( &eAxis, 11, s, &eMax, glm::vec3( C[ 2 ][ 2 ], float( 0.0 ), -C[ 2 ][ 0 ] ), &nE ) )
			return;

		// Cross( a.z, b.x )
		rA = eA.x * absC[ 0 ][ 1 ] + eA.y * absC[ 0 ][ 0 ];
		rB = eB.y * absC[ 2 ][ 2 ] + eB.z * absC[ 1 ][ 2 ];
		s = Abs( t.y * C[ 0 ][ 0 ] - t.x * C[ 0 ][ 1 ] ) - (rA + rB);
		if ( PhysicsTrackEdgeAxis( &eAxis, 12, s, &eMax, glm::vec3( -C[ 0 ][ 1 ], C[ 0 ][ 0 ], float( 0.0 ) ), &nE ) )
			return;

		// Cross( a.z, b.y )
		rA = eA.x * absC[ 1 ][ 1 ] + eA.y * absC[ 1 ][ 0 ];
		rB = eB.x * absC[ 2 ][ 2 ] + eB.z * absC[ 0 ][ 2 ];
		s = Abs( t.y * C[ 1 ][ 0 ] - t.x * C[ 1 ][ 1 ] ) - (rA + rB);
		if ( PhysicsTrackEdgeAxis( &eAxis, 13, s, &eMax, glm::vec3( -C[ 1 ][ 1 ], C[ 1 ][ 0 ], float( 0.0 ) ), &nE ) )
			return;

		// Cross( a.z, b.z )
		rA = eA.x * absC[ 2 ][ 1 ] + eA.y * absC[ 2 ][ 0 ];
		rB = eB.x * absC[ 1 ][ 2 ] + eB.y * absC[ 0 ][ 2 ];
		s = Abs( t.y * C[ 2 ][ 0 ] - t.x * C[ 2 ][ 1 ] ) - (rA + rB);
		if ( PhysicsTrackEdgeAxis( &eAxis, 14, s, &eMax, glm::vec3( -C[ 2 ][ 1 ], C[ 2 ][ 0 ], float( 0.0 ) ), &nE ) )
			return;
	}

	// Artificial axis bias to improve frame coherence
	const float kRelTol = float( 0.95 );
	const float kAbsTol = float( 0.01 );
	int axis;
	float sMax;
	glm::vec3 n;
	float faceMax = glm::max( aMax, bMax );
	if ( kRelTol * eMax > faceMax + kAbsTol )
	{
		axis = eAxis;
		sMax = eMax;
		n = nE;
	}

	else
	{
		if ( kRelTol * bMax > aMax + kAbsTol )
		{
			axis = bAxis;
			sMax = bMax;
			n = nB;
		}

		else
		{
			axis = aAxis;
			sMax = aMax;
			n = nA;
		}
	}

	if ( glm::dot( n, btx.position - atx.position ) < float( 0.0 ) )
		n = -n;

	if ( axis == ~0 )
		return;

	if ( axis < 6 )
	{
		PhysicsTransform rtx;
		PhysicsTransform itx;
		glm::vec3 eR;
		glm::vec3 eI;
		bool flip;

		if ( axis < 3 )
		{
			rtx = atx;
			itx = btx;
			eR = eA;
			eI = eB;
			flip = false;
		}

		else
		{
			rtx = btx;
			itx = atx;
			eR = eB;
			eI = eA;
			flip = true;
			n = -n;
		}

		// Compute reference and incident edge information necessary for clipping
		PhysicsClipVertex incident[ 4 ];
		PhysicsComputeIncidentFace( itx, eI, n, incident );
		unsigned char clipEdges[ 4 ];
		glm::mat3 basis;
		glm::vec3 e;
		PhysicsComputeReferenceEdgesAndBasis( eR, rtx, n, axis, clipEdges, &basis, &e );

		// Clip the incident face against the reference face side planes
		PhysicsClipVertex out[ 8 ];
		float depths[ 8 ];
		int outNum;
		outNum = PhysicsClip( rtx.position, e, clipEdges, basis, incident, out, depths );

		if ( outNum )
		{
			m->contactCount = outNum;
			m->normal = flip ? -n : n;

			for ( int i = 0; i < outNum; ++i )
			{
				PhysicsContact* c = m->contacts + i;

				PhysicsFeaturePair pair = out[ i ].f;

				if ( flip )
				{
					std::swap( pair.inI, pair.inR );
					std::swap( pair.outI, pair.outR );
				}

				c->fp = out[ i ].f;
				c->position = out[ i ].v;
				c->penetration = depths[ i ];
			}
		}
	}

	else
	{
		n = atx.rotation * n;

		if ( glm::dot( n, btx.position - atx.position ) < float( 0.0 ) )
			n = -n;

		glm::vec3 PA, QA;
		glm::vec3 PB, QB;
		PhysicsSupportEdge( atx, eA, n, &PA, &QA );
		PhysicsSupportEdge( btx, eB, -n, &PB, &QB );

		glm::vec3 CA, CB;
		PhysicsEdgesContact( &CA, &CB, PA, QA, PB, QB );

		m->normal = n;
		m->contactCount = 1;

		PhysicsContact* c = m->contacts;
		PhysicsFeaturePair pair;
		pair.key = axis;
		c->fp = pair;
		c->penetration = sMax;
		c->position = (CA + CB) * float( 0.5 );
	}
}
