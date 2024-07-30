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

#include "PhysicsEntityManager.hpp"
#include "PhysicsEntityUtils.hpp"

class PhysicsEntity;
class ndAnimKeyframe;
class ndShaderCache;
class ndDemoMeshInterface;

namespace GPGVulkan
{

class PhysicsEntityNotify: public ndBodyNotify
{
	public:
	D_CLASS_REFLECTION(PhysicsEntityNotify);
	PhysicsEntityNotify(const ndLoadSaveBase::ndLoadDescriptor& desc);
	PhysicsEntityNotify(PhysicsEntityManager* const manager, PhysicsEntity* const entity, ndBodyKinematic* const parentBody = nullptr, ndFloat32 gravity = DEMO_GRAVITY);
	virtual ~PhysicsEntityNotify();

	void* GetUserData() const
	{
		return m_entity;
	}

	virtual void OnObjectPick() const;
	virtual void OnTransform(ndInt32 threadIndex, const ndMatrix& matrix);
	virtual void OnApplyExternalForce(ndInt32 threadIndex, ndFloat32 timestep);

	virtual void Save(const ndLoadSaveBase::ndSaveDescriptor& desc) const;

	void RemoveBody();
	bool CheckInWorld(const ndMatrix& matrix) const
	{
		return matrix.m_posit.m_y > -100.0f;
	}

	PhysicsEntity* m_entity;
	ndBodyKinematic* m_parentBody;
	PhysicsEntityManager* m_manager;
};

class ndBindingRagdollEntityNotify : public PhysicsEntityNotify
{
	public:
	D_CLASS_REFLECTION(ndBindingRagdollEntityNotify);
	ndBindingRagdollEntityNotify(const ndLoadSaveBase::ndLoadDescriptor& desc);
	ndBindingRagdollEntityNotify(PhysicsEntityManager* const manager, PhysicsEntity* const entity, ndBodyDynamic* const parentBody, ndFloat32 campSpeed);

	void OnTransform(ndInt32, const ndMatrix& matrix);
	void OnApplyExternalForce(ndInt32 thread, ndFloat32 timestep);

	virtual void Save(const ndLoadSaveBase::ndSaveDescriptor& desc) const;

	ndMatrix m_bindMatrix;
	ndFloat32 m_capSpeed;
};
}

