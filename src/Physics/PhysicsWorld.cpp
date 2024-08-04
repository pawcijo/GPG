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

#include "PhysicsWorld.hpp"

#include "Physics/PhysicsEntity.hpp"
#include "Physics/PhysicsEntityNotify.hpp"
#include "Physics/PhysicsEntityManager.hpp"
#include "Physics/PhysicsContactCallback.hpp"
#include "Physics/ArchimedesBuoyancyVolume.hpp"

#include <ndLoadSave.h>
#include <ndArray.h>

#define MAX_PHYSICS_STEPS 1
#define MAX_PHYSICS_FPS 60.0f
// #define MAX_PHYSICS_RECOVER_STEPS	2

class PhysicsWorldSettings : public ndWordSettings
{
public:
	D_CLASS_REFLECTION(PhysicsWorldSettings);

	PhysicsWorldSettings(PhysicsWorld *const world)
		: ndWordSettings(), m_cameraMatrix(ndGetIdentityMatrix()), m_world(world)
	{
	}

	PhysicsWorldSettings(const ndLoadSaveBase::ndLoadDescriptor &desc)
		: ndWordSettings(ndLoadSaveBase::ndLoadDescriptor(desc)), m_world(nullptr)
	{
	}

	virtual void Load(const ndLoadSaveBase::ndLoadDescriptor &desc)
	{
		ndLoadSaveBase::ndLoadDescriptor childDesc(desc);
		ndWordSettings::Load(childDesc);

		// load application specific settings here
		m_cameraMatrix = xmlGetMatrix(desc.m_rootNode, "cameraMatrix");
	}

	ndMatrix m_cameraMatrix;
	PhysicsWorld *m_world;
};

DefferentDeleteEntities::DefferentDeleteEntities(PhysicsEntityManager *const manager)
	: ndArray<PhysicsEntity *>(), m_manager(manager)
{
}

void DefferentDeleteEntities::Update()
{
	for (ndInt32 i = 0; i < GetCount(); ++i)
	{
		RemoveEntity((*this)[i]);
	}
	SetCount(0);
}

void DefferentDeleteEntities::RemoveEntity(PhysicsEntity *const entity)
{
	ndAssert(entity->m_rootNode);

	m_manager->RemoveEntity(entity);
	delete entity;

	ndScopeSpinLock lock(entity->m_lock);
	if (!entity->m_isDead)
	{
		entity->m_isDead = true;
		PushBack(entity);
	}
}

D_CLASS_REFLECTION_IMPLEMENT_LOADER(PhysicsWorldSettings);

PhysicsWorld::PhysicsWorld(PhysicsEntityManager *const manager)
	: ndWorld(), m_manager(manager), m_timeAccumulator(0.0f),m_deadEntities(manager)
{
	ClearCache();
	SetContactNotify(new PhysicsContactCallback);
}

PhysicsWorld::~PhysicsWorld()
{
	CleanUp();
}

void PhysicsWorld::CleanUp()
{
	ndWorld::CleanUp();
}

PhysicsEntityManager *PhysicsWorld::GetManager()
{
	return m_manager;
}

void PhysicsWorld::OnPostUpdate(ndFloat32 timestep)
{
}

void PhysicsWorld::SaveScene(const char *const path)
{
	ndLoadSave loadScene;
	PhysicsWorldSettings setting(this);

	loadScene.SaveScene(path, this, &setting);
}

void PhysicsWorld::SaveSceneModel(const char *const path)
{
}

void PhysicsWorld::RemoveEntity(PhysicsEntity *const entity)
{
	ndAssert(entity->m_rootNode);
	m_deadEntities.RemoveEntity(entity);
}

bool PhysicsWorld::LoadScene(const char *const path)
{
	ndAssert(0);

	return true;
}

void PhysicsWorld::AdvanceTime(ndFloat32 timestep)
{
	D_TRACKTIME();
	const ndFloat32 descreteStep = (1.0f / MAX_PHYSICS_FPS);

	ndInt32 maxSteps = MAX_PHYSICS_STEPS;
	m_timeAccumulator += timestep;

	// if the time step is more than max timestep par frame, throw away the extra steps.
	if (m_timeAccumulator > descreteStep * (ndFloat32)maxSteps)
	{
		ndFloat32 steps = ndFloor(m_timeAccumulator / descreteStep) - (ndFloat32)maxSteps;
		ndAssert(steps >= 0.0f);
		m_timeAccumulator -= descreteStep * steps;
	}

	while (m_timeAccumulator > descreteStep)
	{
		Update(descreteStep);
		m_timeAccumulator -= descreteStep;
	}
	if (m_manager->m_synchronousPhysicsUpdate)
	{
		Sync();
	}
}
