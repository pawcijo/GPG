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


#include "PhysicsMemory.h"


#include <cassert>
#include <cstdio>

#include <string.h>


//--------------------------------------------------------------------------------------------------
// PhysicsStack
//--------------------------------------------------------------------------------------------------
PhysicsStack::PhysicsStack( )
	: m_memory( 0 )
	, m_entries( (PhysicsStackEntry*)PhysicsAlloc( sizeof( PhysicsStackEntry ) * 64 ) )
	, m_index( 0 )
	, m_allocation( 0 )
	, m_entryCount( 0 )
	, m_entryCapacity( 64 )
	, m_stackSize( 0 )
{
}

//--------------------------------------------------------------------------------------------------
PhysicsStack::~PhysicsStack( )
{
	if (m_memory) PhysicsFree( m_memory );
	assert( m_index == 0 );
	assert( m_entryCount == 0 );
}

//--------------------------------------------------------------------------------------------------
void PhysicsStack::Reserve( unsigned int size )
{
	assert( !m_index );

	if ( size == 0 )
		return;

	if ( size >= m_stackSize )
	{
		if ( m_memory ) PhysicsFree( m_memory );
		m_memory = (unsigned char*)PhysicsAlloc( size );
		m_stackSize = size;
	}
}

//--------------------------------------------------------------------------------------------------
void *PhysicsStack::Allocate( int size )
{
	assert( m_index + size <= m_stackSize );

	if ( m_entryCount == m_entryCapacity )
	{
		PhysicsStackEntry* oldEntries = m_entries;
		m_entryCapacity *= 2;
		m_entries = (PhysicsStackEntry*)PhysicsAlloc( m_entryCapacity * sizeof( PhysicsStackEntry ) );
		memcpy( m_entries, oldEntries, m_entryCount * sizeof( PhysicsStackEntry ) );
		PhysicsFree( oldEntries );
	}

	PhysicsStackEntry* entry = m_entries + m_entryCount;
	entry->size = size;

	entry->data = m_memory + m_index;
	m_index += size;

	m_allocation += size;
	++m_entryCount;

	return entry->data;
}

//--------------------------------------------------------------------------------------------------
void PhysicsStack::Free( void *data )
{
	// Cannot call free when there are no entries.
	assert( m_entryCount > 0 );

	PhysicsStackEntry *entry = m_entries + m_entryCount - 1;

	// Validate that the data * is a proper location to free.
	// Must be in reverse order of allocation.
	assert( data == entry->data );

	m_index -= entry->size;

	m_allocation -= entry->size;
	--m_entryCount;
}

//--------------------------------------------------------------------------------------------------
// PhysicsHeap
//--------------------------------------------------------------------------------------------------
PhysicsHeap::PhysicsHeap( )
{
	m_memory = (PhysicsHeader*)PhysicsAlloc( Physicsk_heapSize );
	m_memory->next = NULL;
	m_memory->prev = NULL;
	m_memory->size = Physicsk_heapSize;

	m_freeBlocks = (PhysicsFreeBlock*)PhysicsAlloc( sizeof( PhysicsFreeBlock ) * Physicsk_heapInitialCapacity );
	m_freeBlockCount = 1;
	m_freeBlockCapacity = Physicsk_heapInitialCapacity;

	m_freeBlocks->header = m_memory;
	m_freeBlocks->size = Physicsk_heapSize;
}

//--------------------------------------------------------------------------------------------------
PhysicsHeap::~PhysicsHeap( )
{
	PhysicsFree( m_memory );
	PhysicsFree( m_freeBlocks );
}

//--------------------------------------------------------------------------------------------------
void *PhysicsHeap::Allocate( int size )
{
	int sizeNeeded = size + sizeof( PhysicsHeader );
	PhysicsFreeBlock* firstFit = NULL;

	for ( int i = 0; i < m_freeBlockCount; ++i )
	{
		PhysicsFreeBlock* block = m_freeBlocks + i;

		if ( block->size >= sizeNeeded )
		{
			firstFit = block;
			break;
		}
	}

	if ( !firstFit )
		return NULL;

	PhysicsHeader* node = firstFit->header;
	PhysicsHeader* newNode = Q3_PTR_ADD( node, sizeNeeded );
	node->size = sizeNeeded;

	firstFit->size -= sizeNeeded;
	firstFit->header = newNode;

	newNode->next = node->next;
	if ( node->next )
		node->next->prev = newNode;
	node->next = newNode;
	newNode->prev = node;

	return Q3_PTR_ADD( node, sizeof( PhysicsHeader ) );
}

//--------------------------------------------------------------------------------------------------
void PhysicsHeap::Free( void *memory )
{
	assert( memory );
	PhysicsHeader* node = (PhysicsHeader*)Q3_PTR_ADD( memory, -int( sizeof( PhysicsHeader ) ) );

	PhysicsHeader* next = node->next;
	PhysicsHeader* prev = node->prev;
	PhysicsFreeBlock* nextBlock = NULL;
	int prevBlockIndex = ~0;
	PhysicsFreeBlock* prevBlock = NULL;
	int freeBlockCount = m_freeBlockCount;

	for ( int i = 0; i < freeBlockCount; ++i )
	{
		PhysicsFreeBlock* block = m_freeBlocks + i;
		PhysicsHeader* header = block->header;

		if ( header == next )
			nextBlock = block;

		else if ( header == prev )
		{
			prevBlock = block;
			prevBlockIndex = i;
		}
	}

	bool merged = false;

	if ( prevBlock )
	{
		merged = true;

		prev->next = next;
		if ( next )
			next->prev = prev;

		prevBlock->size += node->size;
		prev->size = prevBlock->size;

		if ( nextBlock )
		{
			nextBlock->header = prev;
			nextBlock->size += prev->size;
			prev->size = nextBlock->size;

			PhysicsHeader* nextnext = next->next;
			prev->next = nextnext;

			if ( nextnext )
				nextnext->prev = prev;

			// Remove the nextBlock from the freeBlocks array
			assert( m_freeBlockCount );
			assert( prevBlockIndex != ~0 );
			--m_freeBlockCount;
			m_freeBlocks[ prevBlockIndex ] = m_freeBlocks[ m_freeBlockCount ];
		}
	}

	else if ( nextBlock )
	{
		merged = true;

		nextBlock->header = node;
		nextBlock->size += node->size;
		node->size = nextBlock->size;

		PhysicsHeader* nextnext = next->next;

		if ( nextnext )
			nextnext->prev = node;

		node->next = nextnext;
	}

	if ( !merged )
	{
		PhysicsFreeBlock block;
		block.header = node;
		block.size = node->size;

		if ( m_freeBlockCount == m_freeBlockCapacity )
		{
			PhysicsFreeBlock* oldBlocks = m_freeBlocks;
			int oldCapacity = m_freeBlockCapacity;

			m_freeBlockCapacity *= 2;
			m_freeBlocks = (PhysicsFreeBlock*)PhysicsAlloc( sizeof( PhysicsFreeBlock ) * m_freeBlockCapacity );

			memcpy( m_freeBlocks, oldBlocks, sizeof( PhysicsFreeBlock ) * oldCapacity );
			PhysicsFree( oldBlocks );
		}

		m_freeBlocks[ m_freeBlockCount++ ] = block;
	}
}

//--------------------------------------------------------------------------------------------------
// PhysicsPagedAllocator
//--------------------------------------------------------------------------------------------------
PhysicsPagedAllocator::PhysicsPagedAllocator( int elementSize, int elementsPerPage )
{
	m_blockSize = elementSize;
	m_blocksPerPage = elementsPerPage;

	m_pages = NULL;
	m_pageCount = 0;

	m_freeList = NULL;
}

//--------------------------------------------------------------------------------------------------
PhysicsPagedAllocator::~PhysicsPagedAllocator( )
{
	Clear( );
}

//--------------------------------------------------------------------------------------------------
void* PhysicsPagedAllocator::Allocate( )
{
	if ( m_freeList )
	{
		PhysicsBlock* data = m_freeList;
		m_freeList = data->next;

		return data;
	}

	else
	{
		PhysicsPage* page = (PhysicsPage*)PhysicsAlloc( m_blockSize * m_blocksPerPage + sizeof( PhysicsPage ) );
		++m_pageCount;

		page->next = m_pages;
		page->data = (PhysicsBlock*)Q3_PTR_ADD( page, sizeof( PhysicsPage ) );
		m_pages = page;

		int blocksPerPageMinusOne = m_blocksPerPage - 1;
		for ( int i = 0; i < blocksPerPageMinusOne; ++i )
		{
			PhysicsBlock *node = Q3_PTR_ADD( page->data, m_blockSize * i );
			PhysicsBlock *next = Q3_PTR_ADD( page->data, m_blockSize * (i + 1) );
			node->next = next;
		}

		PhysicsBlock *last = Q3_PTR_ADD( page->data, m_blockSize * (blocksPerPageMinusOne) );
		last->next = NULL;

		m_freeList = page->data->next;

		return page->data;
	}
}

//--------------------------------------------------------------------------------------------------
void PhysicsPagedAllocator::Free( void* data )
{
#ifdef _DEBUG
	bool found = false;

	for ( PhysicsPage *page = m_pages; page; page = page->next )
	{
		if ( data >= page->data && data < Q3_PTR_ADD( page->data, m_blockSize * m_blocksPerPage ) )
		{
			found = true;
			break;
		}
	}

	// Address of data does not lie within any pages of this allocator.
	assert( found );
#endif // DEBUG

	((PhysicsBlock*)data)->next = m_freeList;
	m_freeList = ((PhysicsBlock*)data);
}

//--------------------------------------------------------------------------------------------------
void PhysicsPagedAllocator::Clear( )
{
	PhysicsPage* page = m_pages;

	for ( int i = 0; i < m_pageCount; ++i )
	{
		PhysicsPage* next = page->next;
		PhysicsFree( page );
		page = next;
	}

	m_freeList = NULL;
	m_pageCount = 0;
}
