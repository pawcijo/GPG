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

	class PhysicsEntity : public ndNodeHierarchy<PhysicsEntity>
	{
	public:
		PhysicsEntity(const PhysicsEntity &copyFrom);
		PhysicsEntity(const ndMatrix &matrix, PhysicsEntity *const parent);
		PhysicsEntity(PhysicsEntityManager *const scene, ndMeshEffectNode *const meshEffect);
		virtual ~PhysicsEntity(void);

		static PhysicsEntity *LoadFbx(const char *const filename, PhysicsEntityManager *const scene);

		ndSharedPtr<ndDemoMeshInterface> GetMesh();
		void SetMesh(ndSharedPtr<ndDemoMeshInterface> mesh, const ndMatrix &meshMatrix = ndGetIdentityMatrix());

		const ndMatrix &GetMeshMatrix() const;
		void SetMeshMatrix(const ndMatrix &matrix);

		PhysicsEntity *CreateClone() const;

		const ndMatrix &GetRenderMatrix() const;
		ndMatrix CalculateGlobalMatrix(const PhysicsEntity *const root = nullptr) const;

		ndMatrix GetNextMatrix() const;
		ndMatrix GetCurrentMatrix() const;
		ndAnimKeyframe GetCurrentTransform() const;
		virtual void SetMatrix(const ndQuaternion &rotation, const ndVector &position);
		virtual void SetNextMatrix(const ndQuaternion &rotation, const ndVector &position);
		virtual void ResetMatrix(const ndMatrix &matrix);
		virtual void InterpolateMatrix(ndFloat32 param);
		ndMatrix CalculateInterpolatedGlobalMatrix(const PhysicsEntity *const root = nullptr) const;

		void RenderBone(PhysicsEntityManager *const scene, const ndMatrix &nodeMatrix) const;

		ndShapeInstance *CreateCollisionFromChildren() const;
		ndShapeInstance *CreateCompoundFromMesh(bool lowDetail = false);

		void RenderSkeleton(PhysicsEntityManager *const scene, const ndMatrix &matrix) const;
		virtual void Render(ndFloat32 timeStep, PhysicsEntityManager *const scene, const ndMatrix &matrix) const;

		PhysicsEntity *Find(const char *const name) const;
		PhysicsEntity *FindBySubString(const char *const subString) const;

		const ndString &GetName() const;
		void SetName(const ndString &name);

	protected:
		mutable ndMatrix m_matrix;	 // interpolated matrix
		ndVector m_curPosition;		 // position one physics simulation step in the future
		ndVector m_nextPosition;	 // position at the current physics simulation step
		ndQuaternion m_curRotation;	 // rotation one physics simulation step in the future
		ndQuaternion m_nextRotation; // rotation at the current physics simulation step

		ndMatrix m_meshMatrix;
		ndSharedPtr<ndDemoMeshInterface> m_mesh;
		ndList<PhysicsEntity *>::ndNode *m_rootNode;
		ndString m_name;
		ndSpinLock m_lock;
		bool m_isDead;
		bool m_isVisible;

		friend class ndPhysicsWorld;
		friend class PhysicsEntityNotify;
		friend class PhysicsEntityManager;
	};

}
