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
#pragma once

#include <ndWorld.h>

class PhysicsEntity;
class PhysicsWorld;

class PhysicsEntityManager : public ndList<PhysicsEntity *>
{
public:
	typedef void (*LaunchSDKDemoCallback)(PhysicsEntityManager *const scene);
	typedef void (*UpdateCameraCallback)(PhysicsEntityManager *const manager, void *const context, ndFloat32 timestep);

	PhysicsEntityManager();
	~PhysicsEntityManager();

	void Run();

	void AddEntity(PhysicsEntity *const aPhysicsEntity);
	void RemoveEntity(PhysicsEntity *const aPhysicsEntity);
	PhysicsWorld *GetWorld();

	void ResetTimer();
	void ImportPLYfile(const char *const name);

	bool GetCaptured() const;
	ndInt32 Print(const ndVector &color, const char *fmt, ...) const;

private:
	void BeginFrame();
	void Cleanup();

	ndInt32 ParticleCount() const;
	void SetParticleUpdateMode() const;

	void UpdatePhysics(ndFloat32 timestep);
	ndFloat32 CalculateInteplationParam() const;

	void CalculateFPS(ndFloat32 timestep);

	PhysicsWorld *m_world;

	ndUnsigned64 m_microsecunds;

	ndInt32 m_currentScene;
	ndInt32 m_lastCurrentScene;
	ndInt32 m_framesCount;
	ndInt32 m_physicsFramesCount;
	ndInt32 m_currentPlugin;
	ndInt32 m_solverPasses;
	ndInt32 m_solverSubSteps;
	ndInt32 m_workerThreads;
	ndInt32 m_debugDisplayMode;
	ndInt32 m_collisionDisplayMode;

	bool m_synchronousPhysicsUpdate;
	bool m_suspendPhysicsUpdate;

	ndFloat32 m_fps;
	ndFloat32 m_timestepAcc;
	ndFloat32 m_currentListenerTimestep;
	ndSpinLock m_addDeleteLock;

	ndWorld::ndSolverModes m_solverMode;

	FILE *m_replayLogFile;
	friend class PhysicsWorld;
};
