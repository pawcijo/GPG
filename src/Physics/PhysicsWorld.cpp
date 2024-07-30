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


#include "Physics/PhysicsEntityNotify.hpp"
#include "Physics/PhysicsEntityManager.hpp"
#include "Physics/PhysicsContactCallback.hpp"

#include "ArchimedesBuoyancyVolume.hpp"

#include <ndLoadSave.h>

#define MAX_PHYSICS_STEPS			1
#define MAX_PHYSICS_FPS				60.0f
//#define MAX_PHYSICS_RECOVER_STEPS	2

namespace GPGVulkan
{

class PhysicsWorldSettings : public ndWordSettings
{
	public:
	D_CLASS_REFLECTION(PhysicsWorldSettings);

	PhysicsWorldSettings(PhysicsWorld* const world)
		:ndWordSettings()
		,m_cameraMatrix(ndGetIdentityMatrix())
		,m_world(world)
	{
	}

	PhysicsWorldSettings(const ndLoadSaveBase::ndLoadDescriptor& desc)
		:ndWordSettings(ndLoadSaveBase::ndLoadDescriptor(desc))
		,m_world(nullptr)
	{
	}

	virtual void Load(const ndLoadSaveBase::ndLoadDescriptor& desc)
	{
		ndLoadSaveBase::ndLoadDescriptor childDesc(desc);
		ndWordSettings::Load(childDesc);
		
		// load application specific settings here
		m_cameraMatrix = xmlGetMatrix(desc.m_rootNode, "cameraMatrix");
	}

	virtual void Save(const ndLoadSaveBase::ndSaveDescriptor& desc) const
	{
		nd::TiXmlElement* const childNode = new nd::TiXmlElement(ClassName());
		desc.m_rootNode->LinkEndChild(childNode);
		ndWordSettings::Save(ndLoadSaveBase::ndSaveDescriptor(desc, childNode));

		auto  manager = m_world->GetManager();
		ndDemoCamera* const camera = manager->GetCamera();

		ndMatrix cameraMatrix (camera->GetCurrentMatrix());
		xmlSaveParam(childNode, "description", "string", "this scene was saved from Newton 4.0 sandbox demos");
		xmlSaveParam(childNode, "cameraMatrix", cameraMatrix);
	}
	
	ndMatrix m_cameraMatrix;
	PhysicsWorld* m_world;
};

D_CLASS_REFLECTION_IMPLEMENT_LOADER(PhysicsWorldSettings);

PhysicsWorld::ndDefferentDeleteEntities::ndDefferentDeleteEntities(PhysicsEntityManager* const manager)
	:ndArray<ndDemoEntity*>()
	,m_manager(manager)
	,m_renderThreadId(std::this_thread::get_id())
{
}

void PhysicsWorld::ndDefferentDeleteEntities::Update()
{
	for (ndInt32 i = 0; i < GetCount(); ++i)
	{
		RemoveEntity((*this)[i]);
	}
	SetCount(0);
}

void PhysicsWorld::ndDefferentDeleteEntities::RemoveEntity(ndDemoEntity* const entity)
{
	ndAssert(entity->m_rootNode);
	if (m_renderThreadId == std::this_thread::get_id())
	{
		m_manager->RemoveEntity(entity);
		delete entity;
	}
	else
	{
		ndScopeSpinLock lock(entity->m_lock);
		if (!entity->m_isDead)
		{
			entity->m_isDead = true;
			PushBack(entity);
		}
	}
}

PhysicsWorld::PhysicsWorld(PhysicsEntityManager* const manager)
	:ndWorld()
	,m_manager(manager)
	,m_soundManager(new ndSoundManager(manager))
	,m_timeAccumulator(0.0f)
	,m_deadEntities(manager)
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
	if (m_soundManager)
	{
		delete m_soundManager;
		m_soundManager = nullptr;
	}
}

void PhysicsWorld::RemoveEntity(ndDemoEntity* const entity)
{
	ndAssert(entity->m_rootNode);
	m_deadEntities.RemoveEntity(entity);
}

PhysicsEntityManager* PhysicsWorld::GetManager() 
{
	return m_manager;
}

ndSoundManager* PhysicsWorld::GetSoundManager() const
{
	return m_soundManager;
}

void PhysicsWorld::OnPostUpdate(ndFloat32 timestep)
{
	m_manager->m_cameraManager->FixUpdate(m_manager, timestep);
	if (m_manager->m_updateCamera)
	{
		m_manager->m_updateCamera(m_manager, m_manager->m_updateCameraContext, timestep);
	}

	if (m_soundManager)
	{
		m_soundManager->Update(this, timestep);
	}
}

void PhysicsWorld::SaveScene(const char* const path)
{
	ndLoadSave loadScene;
	PhysicsWorldSettings setting(this);
	
	loadScene.SaveScene(path, this, &setting);
}

void PhysicsWorld::SaveSceneModel(const char* const path)
{
	ndLoadSave loadScene;
	if (m_manager->m_selectedModel)
	{
		loadScene.SaveModel(path, m_manager->m_selectedModel);
	}
}

bool PhysicsWorld::LoadScene(const char* const path)
{
	ndAssert(0);
	//ndLoadSave loadScene;
	//loadScene.LoadScene(path);
	//
	//// iterate over the loaded scene and add all objects to the world.
	//if (loadScene.m_setting && (strcmp("PhysicsWorldSettings", loadScene.m_setting->SubClassName()) == 0))
	//{
	//	PhysicsWorldSettings* const settings = (PhysicsWorldSettings*)loadScene.m_setting;
	//	PhysicsEntityManager* const manager = GetManager();
	//	manager->SetCameraMatrix(settings->m_cameraMatrix, settings->m_cameraMatrix.m_posit);
	//}
	//
	//ndBodyLoaderCache::Iterator bodyIter(loadScene.m_bodyMap);
	//for (bodyIter.Begin(); bodyIter; bodyIter++)
	//{
	//	const ndBody* const body = (ndBody*)bodyIter.GetNode()->GetInfo();
	//	AddBody((ndBody*)body);
	//}
	//
	//ndJointLoaderCache::Iterator jointIter(loadScene.m_jointMap);
	//for (jointIter.Begin(); jointIter; jointIter++)
	//{
	//	const ndJointBilateralConstraint* const joint = (ndJointBilateralConstraint*)jointIter.GetNode()->GetInfo();
	//	AddJoint((ndJointBilateralConstraint*)joint);
	//}
	//
	//ndModelLoaderCache::Iterator modelIter(loadScene.m_modelMap);
	//for (modelIter.Begin(); modelIter; modelIter++)
	//{
	//	const ndModel* const model = modelIter.GetNode()->GetInfo();
	//	AddModel((ndModel*)model);
	//}
	//
	//// add some visualization
	//ndMatrix scale(ndGetIdentityMatrix());
	//scale[0][0] = 0.5f;
	//scale[1][1] = 0.5f;
	//scale[2][2] = 0.5f;
	//for (bodyIter.Begin(); bodyIter; bodyIter++)
	//{
	//	ndBodyKinematic* const body = (ndBodyKinematic*)bodyIter.GetNode()->GetInfo();
	//	ndAssert(body->GetAsBodyKinematic());
	//	const ndShapeInstance& collision = body->GetCollisionShape();
	//
	//	ndDemoEntity* const entity = new ndDemoEntity(body->GetMatrix(), nullptr);
	//
	//	ndShape* const shape = (ndShape*)collision.GetShape();
	//	if (!shape->GetAsShapeStaticProceduralMesh())
	//	{
	//		ndSharedPtr<ndDemoMeshInterface> mesh (new ndDemoMesh("importMesh", m_manager->GetShaderCache(), &collision, "marbleCheckBoard.tga", "marbleCheckBoard.tga", "marbleCheckBoard.tga", 1.0f, scale));
	//		entity->SetMesh(mesh);
	//	}
	//
	//	m_manager->AddEntity(entity);
	//
	//	body->SetNotifyCallback(new ndDemoEntityNotify(m_manager, entity));
	//}

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

	m_deadEntities.Update();
}

}