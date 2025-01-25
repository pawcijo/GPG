/* Copyright (c) <2003-2022> <Newton Game Dynamics>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely
 */

#include "PhysicsEntityManager.hpp"

#include "Physics/PhysicsEntity.hpp"
#include "Physics/PhysicsWorld.hpp"
#include "Physics/PhysicsUtils.hpp"
#include "Physics/PhysicsEntityNotify.hpp"

#include "Physics/PhysicsHighResolutionTimer.h"

#include <ndBodyListView.h>

#include <iostream>

// #define ENABLE_REPLAY
#ifdef ENABLE_REPLAY
// #define REPLAY_RECORD
#endif

#define DEFAULT_SCENE 0 // basic rigidbody

// demos forward declaration
void ndBasicRigidBody(PhysicsEntityManager *const scene);

// ImGui - standalone example application for Glfw + OpenGL 2, using fixed pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
PhysicsEntityManager::PhysicsEntityManager()
	: m_world(nullptr), m_microsecunds(0), m_currentScene(DEFAULT_SCENE),
	  m_lastCurrentScene(DEFAULT_SCENE), m_framesCount(0), m_physicsFramesCount(0), m_currentPlugin(0), m_solverPasses(4),
	  m_solverSubSteps(2), m_workerThreads(1), m_debugDisplayMode(0), m_collisionDisplayMode(0), m_fps(0.0f),
	  m_timestepAcc(0.0f), m_currentListenerTimestep(0.0f), m_addDeleteLock(), m_synchronousPhysicsUpdate(false),
	  m_solverMode(ndWorld::ndSimdSoaSolver), m_replayLogFile(nullptr)
{

	std::cout<<"PhysicsEntityManager Initialised. \n";

	m_solverMode = ndWorld::ndStandardSolver;
	m_workerThreads = 1;

	Cleanup();
	ResetTimer();

#ifdef ENABLE_REPLAY
#ifdef REPLAY_RECORD
	m_replayLogFile = fopen("replayLog.bin", "wb");
#else
	m_replayLogFile = fopen("replayLog.bin", "rb");
#endif
#endif

	// Test0__();
	// Test1__();

	// ndSharedPtr<PhysicsEntityManager> xxx(this);
	// PhysicsEntityManager* xxx1 = *xxx;
	// PhysicsEntityManager* xxx2 = *xxx;
}

PhysicsEntityManager::~PhysicsEntityManager()
{
	if (m_replayLogFile)
	{
		fclose(m_replayLogFile);
	}

	Cleanup();

	// destroy the empty world
	if (m_world)
	{
		delete m_world;
	}
}

void PhysicsEntityManager::ResetTimer()
{
	dResetTimer();
	m_microsecunds = ndGetTimeInMicroseconds();
}

PhysicsWorld* PhysicsEntityManager::GetWorld()
{
	return m_world;
}

void PhysicsEntityManager::AddEntity(PhysicsEntity *const aPhysicsEntity)
{
	ndScopeSpinLock lock(m_addDeleteLock);
	ndAssert(!aPhysicsEntity->m_rootNode);
	aPhysicsEntity->m_rootNode = Append(aPhysicsEntity);
}

void PhysicsEntityManager::RemoveEntity(PhysicsEntity *const aPhysicsEntity)
{
	ndScopeSpinLock lock(m_addDeleteLock);
	ndAssert(aPhysicsEntity->m_rootNode);
	Remove(aPhysicsEntity->m_rootNode);
}

void PhysicsEntityManager::Cleanup()
{
	// is we are run asynchronous we need make sure no update in on flight.
	if (m_world)
	{
		m_world->Sync();
	}

	// destroy all remaining visual objects
	while (GetFirst())
	{
		PhysicsEntity *const ent = GetFirst()->GetInfo();
		RemoveEntity(ent);
		delete ent;
	}

	// create the newton world
	m_world = new PhysicsWorld(this);
	;
}

void PhysicsEntityManager::CalculateFPS(ndFloat32 timestep)
{
	m_framesCount++;
	m_timestepAcc += timestep;

	// this probably happing on loading of and a pause, just rest counters
	if ((m_timestepAcc <= 0.0f) || (m_timestepAcc > 4.0f))
	{
		m_timestepAcc = 0;
		m_framesCount = 0;
	}

	// update fps every quarter of a second
	const ndFloat32 movingAverage = 0.5f;
	if (m_timestepAcc >= movingAverage)
	{
		m_fps = ndFloat32(m_framesCount) / m_timestepAcc;
		m_timestepAcc -= movingAverage;
		m_framesCount = 0;
	}
}

// void PhysicsEntityManager::ImportPLYfile (const char* const fileName)
void PhysicsEntityManager::ImportPLYfile(const char *const)
{
	ndAssert(0);
	// m_collisionDisplayMode = 2;
	// CreatePLYMesh (this, fileName, true);
}

void PhysicsEntityManager::UpdatePhysics(ndFloat32 timestep)
{
	// update the physics
	if (m_world && !m_suspendPhysicsUpdate)
	{
		m_world->AdvanceTime(timestep);
	}
}

ndFloat32 PhysicsEntityManager::CalculateInteplationParam() const
{
	ndUnsigned64 timeStep = ndGetTimeInMicroseconds() - m_microsecunds;
	ndFloat32 param = (ndFloat32(timeStep) * MAX_PHYSICS_FPS) / 1.0e6f;
	ndAssert(param >= 0.0f);
	if (param > 1.0f)
	{
		param = 1.0f;
	}
	return param;
}
