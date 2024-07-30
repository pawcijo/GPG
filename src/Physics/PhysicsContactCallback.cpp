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

#include "PhysicsContactCallback.hpp"

namespace GPGVulkan
{

	ndApplicationMaterial::ndApplicationMaterial()
		: ndMaterial()
	{
	}

	ndApplicationMaterial::ndApplicationMaterial(const ndApplicationMaterial &copy)
		: ndMaterial(copy)
	{
	}

	ndApplicationMaterial::~ndApplicationMaterial()
	{
	}

	// bool ndApplicationMaterial::OnAabbOverlap(const ndContact* const contactJoint, ndFloat32) const
	bool ndApplicationMaterial::OnAabbOverlap(const ndContact *const, ndFloat32, const ndShapeInstance &instanceShape0, const ndShapeInstance &instanceShape1) const
	{
		// const ndBodyKinematic* const body0 = contactJoint->GetBody0();
		// const ndBodyKinematic* const body1 = contactJoint->GetBody1();
		// const ndShapeInstance& instanceShape0 = body0->GetCollisionShape();
		// const ndShapeInstance& instanceShape1 = body1->GetCollisionShape();

		if ((instanceShape0.GetUserDataID() == ndApplicationMaterial::m_dedris) && (instanceShape1.GetUserDataID() == ndApplicationMaterial::m_dedris))
		{
			return false;
		}

		return true;
	}

	// void ndApplicationMaterial::OnContactCallback(const ndContact* const joint, ndFloat32) const
	void ndApplicationMaterial::OnContactCallback(const ndContact *const, ndFloat32) const
	{
		if (m_userFlags & ndApplicationMaterial::m_playSound)
		{
			// PlaySoundTest(joint);
		}
	}

	ndMaterialGraph::ndMaterialGraph()
		: ndTree<ndApplicationMaterial *, ndMaterailKey>()
	{
	}

	ndMaterialGraph::~ndMaterialGraph()
	{
		Iterator it(*this);
		for (it.Begin(); it; it++)
		{
			ndApplicationMaterial *const material = it.GetNode()->GetInfo();
			delete material;
		}
	}

	ndContactCallback::ndContactCallback()
		: ndContactNotify(), m_materialGraph(), m_defaultMaterial()
	{
	}

	void ndContactCallback::OnBodyAdded(ndBodyKinematic *const) const
	{
	}

	void ndContactCallback::OnBodyRemoved(ndBodyKinematic *const) const
	{
	}

	void ndContactCallback::PlaySoundTest(const ndContact *const contactJoint)
	{
		const ndBodyKinematic *const body0 = contactJoint->GetBody0();
		const ndBodyKinematic *const body1 = contactJoint->GetBody1();
		const ndContactPointList &contactPoints = contactJoint->GetContactPoints();

		ndFloat32 maxNornalSpeed = ndFloat32(0.0f);
		ndFloat32 maxTangentSpeed = ndFloat32(0.0f);
		// const ndContactMaterial* normalContact = nullptr;
		// const ndContactMaterial* tangentContact = nullptr;
		for (ndContactPointList::ndNode *contactNode = contactPoints.GetFirst(); contactNode; contactNode = contactNode->GetNext())
		{
			const ndContactMaterial &contactPoint = contactNode->GetInfo();
			const ndVector pointVeloc0(body0->GetVelocityAtPoint(contactPoint.m_point));
			const ndVector pointVeloc1(body1->GetVelocityAtPoint(contactPoint.m_point));
			const ndVector veloc(pointVeloc1 - pointVeloc0);

			const ndFloat32 verticalSpeed = contactPoint.m_normal.DotProduct(veloc).GetScalar();
			const ndFloat32 nornalSpeed = ndAbs(verticalSpeed);
			if (nornalSpeed > maxNornalSpeed)
			{
				maxNornalSpeed = nornalSpeed;
				// normalContact = &contactPoint;
			}

			ndVector tangVeloc(veloc - contactPoint.m_normal.Scale(verticalSpeed));
			const ndFloat32 tangentSpeed = tangVeloc.DotProduct(tangVeloc).GetScalar();
			if (tangentSpeed > maxTangentSpeed)
			{
				maxTangentSpeed = tangentSpeed;
				// tangentContact = &contactPoint;
			}
		}

		const ndShapeInstance &instance0 = body0->GetCollisionShape();
		const ndShapeInstance &instance1 = body1->GetCollisionShape();
		const ndFloat32 speedThreshold = ndMax(instance0.GetMaterial().m_userParam[m_soundSpeedThreshold].m_floatData, instance1.GetMaterial().m_userParam[m_soundSpeedThreshold].m_floatData);
		if (maxNornalSpeed > speedThreshold)
		{
			// play impact sound here;
		}

		maxTangentSpeed = ndSqrt(maxTangentSpeed);
		if (maxTangentSpeed > speedThreshold)
		{
			// play scratching sound here;
		}
	}

	ndApplicationMaterial &ndContactCallback::RegisterMaterial(const ndApplicationMaterial &material, ndUnsigned32 id0, ndUnsigned32 id1)
	{
		ndMaterailKey key(id0, id1);
		ndMaterialGraph::ndNode *node = m_materialGraph.Find(key);
		if (!node)
		{
			ndApplicationMaterial *const materialCopy = material.Clone();
			node = m_materialGraph.Insert(materialCopy, key);
		}
		return *node->GetInfo();
	}

	ndMaterial *ndContactCallback::GetMaterial(const ndContact *const, const ndShapeInstance &instance0, const ndShapeInstance &instance1) const
	{
		ndMaterailKey key(ndUnsigned32(instance0.GetMaterial().m_userId), ndUnsigned32(instance1.GetMaterial().m_userId));
		ndMaterialGraph::ndNode *const node = m_materialGraph.Find(key);
		return node ? node->GetInfo() : (ndMaterial *)&m_defaultMaterial;
	}

	bool ndContactCallback::OnAabbOverlap(const ndContact *const contactJoint, ndFloat32 timestep)
	{
		const ndApplicationMaterial *const material = (ndApplicationMaterial *)contactJoint->GetMaterial();
		ndAssert(material);

		const ndBodyKinematic *const body0 = contactJoint->GetBody0();
		const ndBodyKinematic *const body1 = contactJoint->GetBody1();
		const ndShapeInstance &instanceShape0 = body0->GetCollisionShape();
		const ndShapeInstance &instanceShape1 = body1->GetCollisionShape();
		return material->OnAabbOverlap(contactJoint, timestep, instanceShape0, instanceShape1);
	}

	// bool ndContactCallback::OnCompoundSubShapeOverlap(const ndContact* const contactJoint, const ndShapeInstance& instance0, const ndShapeInstance& instance1)
	bool ndContactCallback::OnCompoundSubShapeOverlap(const ndContact *const contactJoint, ndFloat32 timestep, const ndShapeInstance *const instance0, const ndShapeInstance *const instance1)
	{
		const ndApplicationMaterial *const material = (ndApplicationMaterial *)contactJoint->GetMaterial();
		ndAssert(material);
		return material->OnAabbOverlap(contactJoint, timestep, *instance0, *instance1);
	}

	// void ndContactCallback::OnContactCallback(ndInt32 threadIndex, const ndContact* const contactJoint, ndFloat32 timestep)
	void ndContactCallback::OnContactCallback(const ndContact *const contactJoint, ndFloat32 timestep)
	{
		const ndApplicationMaterial *const material = (ndApplicationMaterial *)contactJoint->GetMaterial();
		ndAssert(material);
		material->OnContactCallback(contactJoint, timestep);
	}

}
