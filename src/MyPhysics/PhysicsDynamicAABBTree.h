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

#include "Common.h"
#include "PhysicsRaycastData.h"


class PhysicsRender;

class PhysicsDynamicAABBTree
{
public:
	PhysicsDynamicAABBTree( );
	~PhysicsDynamicAABBTree( );

	// Provide tight-AABB
	int Insert( const PhysicsAABB& aabb, void *userData );
	void Remove( int id );
	bool Update( int id, const PhysicsAABB& aabb );

	void *GetUserData( int id ) const;
	const PhysicsAABB& GetFatAABB( int id ) const;
	
    void Render( PhysicsRender *render ) const;

	template <typename T>
	void Query( T *cb, const PhysicsAABB& aabb ) const;
	template <typename T>
	void Query( T *cb, PhysicsRaycastData& rayCast ) const;

	// For testing
	void Validate( ) const;

private:
	struct Node
	{
		bool IsLeaf( void ) const
		{
			// The right leaf does not use the same memory as the userdata,
			// and will always be Null (no children)
			return right == Null;
		}

		// Fat AABB for leafs, bounding AABB for branches
		PhysicsAABB aabb;

		union
		{
			int parent;
			int next; // free list
		};

		// Child indices
		struct
		{
			int left;
			int right;
		};

		// Since only leaf nodes hold userdata, we can use the
		// same memory used for left/right indices to store
		// the userdata void pointer
		void *userData;

		// leaf = 0, free nodes = -1
		int height;

		static const int Null = -1;
	};

	inline int AllocateNode( );
	inline void DeallocateNode( int index );
	int Balance( int index );
	void InsertLeaf( int index );
	void RemoveLeaf( int index );
	void ValidateStructure( int index ) const;
	
    void RenderNode( PhysicsRender *render, int index ) const;

	// Correct AABB hierarchy heights and AABBs starting at supplied
	// index traversing up the heirarchy
	void SyncHeirarchy( int index );

	// Insert nodes at a given index until m_capacity into the free list
	void AddToFreeList( int index );

	int m_root;
	Node *m_nodes;
	int m_count;	// Number of active nodes
	int m_capacity;	// Max capacity of nodes
	int m_freeList;
};



#include "PhysicsDynamicAABBTree.inl"