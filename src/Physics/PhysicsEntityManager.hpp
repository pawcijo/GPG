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

class ndDemoMesh;
class ndUIEntity;
class PhysicsEntity;
class ndAnimationSequence;
class ndDemoMeshInterface;
class ndWireFrameDebugMesh;
class ndFlatShadedDebugMesh;

#include "Physics/PhysicsWorld.hpp"

namespace GPGVulkan
{

	class PhysicsEntityManager : public ndList<PhysicsEntity *>
	{
	public:
		typedef void (*LaunchSDKDemoCallback)(PhysicsEntityManager *const scene);
		typedef void (*UpdateCameraCallback)(PhysicsEntityManager *const manager, void *const context, ndFloat32 timestep);

		enum ndMenuSelection
		{
			m_new,
			m_load,
			m_save,
			m_saveModel,
			m_none,
		};

		class ndKeyTrigger
		{
		public:
			ndKeyTrigger()
				: m_memory(false)
			{
			}

			bool Update(bool value)
			{
				bool ret = !m_memory & value;
				m_memory = value;
				return ret;
			}

			bool m_memory;
		};

		class ndLightSource
		{
		public:
			ndVector m_position;
			ndVector m_ambient;
			ndVector m_diffuse;
			ndVector m_specular;
			ndFloat32 m_shininess;
		};

		class TransparentMesh
		{
		public:
			TransparentMesh()
				: m_matrix(ndGetIdentityMatrix()), m_mesh(nullptr)
			{
			}

			TransparentMesh(const ndMatrix &matrix, ndDemoMesh *const mesh)
				: m_matrix(matrix), m_mesh(mesh)
			{
			}

			ndMatrix m_matrix;
			ndDemoMesh *m_mesh;
		};

		class TransparentHeap : public ndUpHeap<TransparentMesh, ndFloat32>
		{
		public:
			TransparentHeap()
				: ndUpHeap<TransparentMesh, ndFloat32>(2048)
			{
			}
		};

		class ndDebuMesh;
		class ndDebugMeshCache;

		class SDKDemos
		{
		public:
			const char *m_name;
			LaunchSDKDemoCallback m_launchDemoCallback;
		};

		class ButtonKey
		{
		public:
			ButtonKey(bool initialState);
			ndInt32 UpdateTrigger(bool triggerValue);
			ndInt32 UpdatePushButton(bool triggerValue);
			ndInt32 GetPushButtonState() const { return m_state ? 1 : 0; }

		private:
			bool m_state;
			bool m_memory0;
			bool m_memory1;
		};

		PhysicsEntityManager();
		~PhysicsEntityManager();

		void Run();

		void AddEntity(PhysicsEntity *const ent);
		void RemoveEntity(PhysicsEntity *const ent);
		PhysicsWorld  *GetWorld();

		void ResetTimer();
		void ImportPLYfile(const char *const name);

		void PushTransparentMesh(const ndDemoMeshInterface *const mesh, const ndMatrix &modelMatrix);
		void Set2DDisplayRenderFunction(ndSharedPtr<ndUIEntity> &demoGui);

		bool GetCaptured() const;
		ndInt32 Print(const ndVector &color, const char *fmt, ...) const;

	private:
		void BeginFrame();
		void Cleanup();

		void RenderScene();
		ndInt32 ParticleCount() const;
		void SetParticleUpdateMode() const;

		void UpdatePhysics(ndFloat32 timestep);
		ndFloat32 CalculateInteplationParam() const;

		void CalculateFPS(ndFloat32 timestep);

		PhysicsWorld *m_world;

		ndUnsigned64 m_microsecunds;
		TransparentHeap m_transparentHeap;

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

		ndFloat32 m_fps;
		ndFloat32 m_timestepAcc;
		ndFloat32 m_currentListenerTimestep;
		ndSpinLock m_addDeleteLock;

		ndWorld::ndSolverModes m_solverMode;

		FILE *m_replayLogFile;
		friend class PhysicsWorld;
	};

	PhysicsWorld  *PhysicsEntityManager::GetWorld() 
	{
		return m_world;
	}

}
