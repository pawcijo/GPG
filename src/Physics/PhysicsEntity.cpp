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

#include "PhysicsEntity.hpp"

PhysicsEntity::PhysicsEntity(const ndMatrix &matrix, PhysicsEntity *const parent)
	: ndNodeHierarchy<PhysicsEntity>(), m_matrix(matrix), m_curPosition(matrix.m_posit), m_nextPosition(matrix.m_posit), m_curRotation(matrix), m_nextRotation(matrix), m_meshMatrix(ndGetIdentityMatrix()), m_mesh(), m_rootNode(nullptr), m_name(nullptr), m_lock(), m_isDead(false), m_isVisible(true)
{
	if (parent)
	{
		Attach(parent);
	}
}

PhysicsEntity::PhysicsEntity(PhysicsEntityManager *const scene, ndMeshEffectNode *const meshEffectNode)
	: ndNodeHierarchy<PhysicsEntity>(), m_matrix(meshEffectNode->m_matrix), m_curPosition(meshEffectNode->m_matrix.m_posit), m_nextPosition(meshEffectNode->m_matrix.m_posit), m_curRotation(meshEffectNode->m_matrix), m_nextRotation(m_curRotation), m_meshMatrix(ndGetIdentityMatrix()), m_mesh(), m_rootNode(nullptr), m_name(nullptr), m_lock(), m_isDead(false), m_isVisible(true)
{
	ndInt32 stack = 1;
	PhysicsEntity *parentEntityBuffer[256];
	ndMeshEffectNode *effectNodeBuffer[256];
	struct EntityMeshPair
	{
		EntityMeshPair()
		{
		}

		EntityMeshPair(PhysicsEntity *const entity, ndMeshEffectNode *const effectNode)
			: m_entity(entity), m_effectNode(effectNode)
		{
		}
		PhysicsEntity *m_entity;
		ndMeshEffectNode *m_effectNode;
	};
	ndFixSizeArray<EntityMeshPair, 1024> meshArray;

	bool isRoot = true;
	effectNodeBuffer[0] = meshEffectNode;
	parentEntityBuffer[0] = nullptr;
	while (stack)
	{
		stack--;
		PhysicsEntity *const parent = parentEntityBuffer[stack];
		ndMeshEffectNode *const effectNode = effectNodeBuffer[stack];

		PhysicsEntity *const entity = isRoot ? this : new PhysicsEntity(effectNode->m_matrix, parent);
		isRoot = false;

		entity->SetName(effectNode->GetName().GetStr());

		ndSharedPtr<ndMeshEffect> meshEffect = effectNode->GetMesh();
		if (*meshEffect)
		{
			// ndDemoMeshInterface* mesh = nullptr;
			// if (!meshEffect->GetCluster().GetCount())
			//{
			//	mesh = new ndDemoMesh(effectNode->GetName().GetStr(), *meshEffect, scene->GetShaderCache());
			// }
			// else
			//{
			//	mesh = new ndDemoSkinMesh(entity, *meshEffect, scene->GetShaderCache());
			// }
			// entity->SetMesh(ndSharedPtr<ndDemoMeshInterface>(mesh), effectNode->m_meshMatrix);
			//
			// if ((effectNode->GetName().Find("hidden") >= 0) || (effectNode->GetName().Find("Hidden") >= 0))
			//{
			//	mesh->m_isVisible = false;
			// }

			meshArray.PushBack(EntityMeshPair(entity, effectNode));
		}

		for (ndMeshEffectNode *child = effectNode->GetLastChild(); child; child = child->GetPrev())
		{
			effectNodeBuffer[stack] = child;
			parentEntityBuffer[stack] = entity;
			stack++;
		}
	}
}

PhysicsEntity::PhysicsEntity(const PhysicsEntity &copyFrom)
	: ndNodeHierarchy<PhysicsEntity>(copyFrom), m_matrix(copyFrom.m_matrix), m_curPosition(copyFrom.m_curPosition), m_nextPosition(copyFrom.m_nextPosition), m_curRotation(copyFrom.m_curRotation), m_nextRotation(copyFrom.m_nextRotation), m_meshMatrix(copyFrom.m_meshMatrix), m_mesh(copyFrom.m_mesh), m_rootNode(nullptr), m_name(copyFrom.m_name), m_lock(), m_isDead(false), m_isVisible(copyFrom.m_isVisible)
{
}

PhysicsEntity::~PhysicsEntity(void)
{
}

const ndString &PhysicsEntity::GetName() const
{
	return m_name;
}

void PhysicsEntity::SetName(const ndString &name)
{
	m_name = name;
}

PhysicsEntity *PhysicsEntity::CreateClone() const
{
	return new PhysicsEntity(*this);
}

ndSharedPtr<ndDemoMeshInterface> PhysicsEntity::GetMesh()
{
	return m_mesh;
}

void PhysicsEntity::SetMesh(ndSharedPtr<ndDemoMeshInterface> mesh, const ndMatrix &meshMatrix)
{
	m_mesh = mesh;
	m_meshMatrix = meshMatrix;
}

const ndMatrix &PhysicsEntity::GetMeshMatrix() const
{
	return m_meshMatrix;
}

void PhysicsEntity::SetMeshMatrix(const ndMatrix &matrix)
{
	m_meshMatrix = matrix;
}

ndMatrix PhysicsEntity::GetCurrentMatrix() const
{
	return ndMatrix(m_curRotation, m_curPosition);
}

ndMatrix PhysicsEntity::GetNextMatrix() const
{
	return ndMatrix(m_nextRotation, m_nextPosition);
}

ndMatrix PhysicsEntity::CalculateGlobalMatrix(const PhysicsEntity *const root) const
{
	ndMatrix matrix(ndGetIdentityMatrix());
	for (const PhysicsEntity *ptr = this; ptr != root; ptr = ptr->GetParent())
	{
		matrix = matrix * ptr->GetCurrentMatrix();
	}
	return matrix;
}

ndMatrix PhysicsEntity::CalculateInterpolatedGlobalMatrix(const PhysicsEntity *const root) const
{
	ndMatrix matrix(ndGetIdentityMatrix());
	for (const PhysicsEntity *ptr = this; ptr != root; ptr = ptr->GetParent())
	{
		matrix = matrix * ptr->m_matrix;
	}
	return matrix;
}

void PhysicsEntity::SetMatrix(const ndQuaternion &rotation, const ndVector &position)
{
	ndScopeSpinLock lock(m_lock);
	m_curPosition = m_nextPosition;
	m_curRotation = m_nextRotation;

	m_nextPosition = position;
	m_nextRotation = rotation;
	ndAssert(position.m_w == ndFloat32(1.0f));

	ndFloat32 angle = m_curRotation.DotProduct(m_nextRotation).GetScalar();
	if (angle < 0.0f)
	{
		m_curRotation = m_curRotation.Scale(ndFloat32(-1.0f));
	}
}

void PhysicsEntity::SetNextMatrix(const ndQuaternion &rotation, const ndVector &position)
{
	// read the data in a critical section to prevent race condition from other thread
	m_nextPosition = position;
	m_nextRotation = rotation;
	ndAssert(position.m_w == ndFloat32(1.0f));

	ndFloat32 angle = m_curRotation.DotProduct(m_nextRotation).GetScalar();
	if (angle < 0.0f)
	{
		m_curRotation = m_curRotation.Scale(ndFloat32(-1.0f));
	}
}

void PhysicsEntity::InterpolateMatrix(ndFloat32 param)
{
	{
		ndScopeSpinLock lock(m_lock);
		ndVector p0(m_curPosition);
		ndVector p1(m_nextPosition);
		ndQuaternion r0(m_curRotation);
		ndQuaternion r1(m_nextRotation);

		ndVector posit(p0 + (p1 - p0).Scale(param));
		ndQuaternion rotation(r0.Slerp(r1, param));
		m_matrix = ndMatrix(rotation, posit);
	}

	for (PhysicsEntity *child = GetFirstChild(); child; child = child->GetNext())
	{
		child->InterpolateMatrix(param);
	}
}

void PhysicsEntity::ResetMatrix(const ndMatrix &matrix)
{
	ndQuaternion rot(matrix);
	SetMatrix(rot, matrix.m_posit);
	SetMatrix(rot, matrix.m_posit);
	InterpolateMatrix(ndFloat32(0.0f));
}

PhysicsEntity *PhysicsEntity::Find(const char *const name) const
{
	ndString string(name);

	ndInt32 stack = 1;
	const PhysicsEntity *pool[1024 * 4];
	pool[0] = this;
	while (stack)
	{
		stack--;
		const PhysicsEntity *const entity = pool[stack];
		if (entity->GetName() == string)
		{
			return (PhysicsEntity *)entity;
		}

		for (PhysicsEntity *child = entity->GetFirstChild(); child; child = child->GetNext())
		{
			pool[stack] = child;
			stack++;
			ndAssert(stack < sizeof(pool) / sizeof(pool[0]));
		}
	}
	return nullptr;
}

PhysicsEntity *PhysicsEntity::FindBySubString(const char *const subString) const
{
	// for (PhysicsEntity* child = GetFirstChild(); child; child = child->GetNext())
	//{
	//	ndString tmpName(child->GetName());
	//	tmpName.ToLower();
	//	const char* const name = tmpName.GetStr();
	//	if (strstr(name, subString))
	//	{
	//		return child;
	//	}
	// }

	ndInt32 stack = 1;
	const PhysicsEntity *pool[1024 * 4];
	pool[0] = this;
	while (stack)
	{
		stack--;
		const PhysicsEntity *const entity = pool[stack];
		ndString tmpName(entity->GetName());
		tmpName.ToLower();
		const char *const name = tmpName.GetStr();
		if (strstr(name, subString))
		{
			return (PhysicsEntity *)entity;
		}

		for (PhysicsEntity *child = entity->GetFirstChild(); child; child = child->GetNext())
		{
			pool[stack] = child;
			stack++;
			ndAssert(stack < sizeof(pool) / sizeof(pool[0]));
		}
	}

	return nullptr;
}
