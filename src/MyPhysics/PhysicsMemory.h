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

#include <stdlib.h>

//--------------------------------------------------------------------------------------------------
// Memory Macros
//--------------------------------------------------------------------------------------------------
inline void* PhysicsAlloc( int bytes )
{
	return malloc( bytes );
}

inline void PhysicsFree( void* memory )
{
	free( memory );
}

#define Q3_PTR_ADD( P, BYTES ) \
	((decltype( P ))(((unsigned char *)P) + (BYTES)))

//--------------------------------------------------------------------------------------------------
// PhysicsStack
//--------------------------------------------------------------------------------------------------

class PhysicsStack
{
private:
	struct PhysicsStackEntry
	{
		unsigned char *data;
		int size;
	};

public:
	PhysicsStack( );
	~PhysicsStack( );

	void Reserve( unsigned int  size );
	void *Allocate( int size );
	void Free( void *data );

private:
	unsigned char* m_memory;
	PhysicsStackEntry* m_entries;

	unsigned int  m_index;

	int m_allocation;
	int m_entryCount;
	int m_entryCapacity;
	unsigned int  m_stackSize;
};

//--------------------------------------------------------------------------------------------------
// PhysicsHeap
//--------------------------------------------------------------------------------------------------
// 20 MB heap size, change as necessary
const int Physicsk_heapSize = 1024 * 1024 * 20;
const int Physicsk_heapInitialCapacity = 1024;

// Operates on first fit basis in attempt to improve cache coherency
class PhysicsHeap
{
private:
	struct PhysicsHeader
	{
		PhysicsHeader* next;
		PhysicsHeader* prev;
		int size;
	};

	struct PhysicsFreeBlock
	{
		PhysicsHeader* header;
		int size;
	};

public:
	PhysicsHeap( );
	~PhysicsHeap( );

	void *Allocate( int size );
	void Free( void *memory );

private:
	PhysicsHeader* m_memory;

	PhysicsFreeBlock* m_freeBlocks;
	int m_freeBlockCount;
	int m_freeBlockCapacity;
};

//--------------------------------------------------------------------------------------------------
// PhysicsPagedAllocator
//--------------------------------------------------------------------------------------------------
class PhysicsPagedAllocator
{
	struct PhysicsBlock
	{
		PhysicsBlock* next;
	};

	struct PhysicsPage
	{
		PhysicsPage* next;
		PhysicsBlock* data;
	};

public:
	PhysicsPagedAllocator( int elementSize, int elementsPerPage );
	~PhysicsPagedAllocator( );

	void* Allocate( );
	void Free( void* data );

	void Clear( );

private:
	int m_blockSize;
	int m_blocksPerPage;

	PhysicsPage *m_pages;
	int m_pageCount;

	PhysicsBlock *m_freeList;
};


