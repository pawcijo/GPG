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

#define MAX_PHYSICS_FPS 60.0f

class PhysicsEntityManager;
class PhysicsEntity;

class DefferentDeleteEntities : public ndArray<PhysicsEntity *>
{
public:
	DefferentDeleteEntities(PhysicsEntityManager *const manager);

	void Update();
	void RemoveEntity(PhysicsEntity *const entity);

	PhysicsEntityManager *m_manager;
};

class PhysicsWorld : public ndWorld
{
public:
	PhysicsWorld(PhysicsEntityManager *const manager);
	virtual ~PhysicsWorld();
	virtual void CleanUp();

	void AdvanceTime(ndFloat32 timestep);
	PhysicsEntityManager *GetManager();

	bool LoadScene(const char *const path);
	void SaveScene(const char *const path);
	void SaveSceneModel(const char *const path);

	void RemoveEntity(PhysicsEntity *const entity);

private:
	void OnPostUpdate(ndFloat32 timestep);

	PhysicsEntityManager *m_manager;
	ndFloat32 m_timeAccumulator;
	DefferentDeleteEntities m_deadEntities;
};
