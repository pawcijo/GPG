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

#include "PhysicsEntity.h"
#include "PhysicsWorld.h"
#include "PhysicsUtils.h"
#include "PhysicsEntityNotify.h"
#include "PhysicsEntityManager.h"

#include "ndHighResolutionTimer.h"

// #define ENABLE_REPLAY
#ifdef ENABLE_REPLAY
// #define REPLAY_RECORD
#endif

#define DEFAULT_SCENE 0 // basic rigidbody

namespace GPGVulkan
{

	// demos forward declaration
	void ndBasicRigidBody(PhysicsEntityManager *const scene);

	ndInt32 PhysicsEntityManager::ButtonKey::UpdateTrigger(bool triggerValue)
	{
		m_memory0 = m_memory1;
		m_memory1 = triggerValue;
		return (!m_memory0 & m_memory1) ? 1 : 0;
	}

	void Test0__()
	{
		ndFloat32 x[] = {1.0f, -2.0f, 1.0f, 2.5f, 3.0f, -1.0f};
		// ndFloat32 x[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
		ndFloat32 A[6][6];
		ndFloat32 B[6];

		ndCovarianceMatrix<ndFloat32>(6, &A[0][0], x, x);
		for (ndInt32 i = 0; i < 6; ++i)
		{
			A[i][i] *= 1.1f;
		}
		ndAssert(ndTestPSDmatrix(6, 6, &A[0][0]));

		ndMatrixTimeVector<ndFloat32>(6, &A[0][0], x, B);
		for (ndInt32 i = 0; i < 6; ++i)
		{
			x[i] = 0;
		}

		ndFloat32 precond[6 * 2];
		ndConjugateGradient<ndFloat32> cgd;
		cgd.Solve(6, 1.0e-5f, x, B, &A[0][0], precond);
	}

	void Test1__()
	{
		// ndFloat32 A[2][2];
		// ndFloat32 x[2];
		// ndFloat32 b[2];
		// ndFloat32 l[2];
		// ndFloat32 h[2];
		//
		// A[0][0] = 2.0f;
		// A[0][1] = 1.0f;
		// A[1][0] = 1.0f;
		// A[1][1] = 2.0f;
		// b[0] = 1.0f;
		// b[1] = 1.0f;
		// x[0] = 1;
		// x[1] = 2;
		//
		// l[0] = 0.0f;
		// l[1] = 0.0f;
		// h[0] = 0.25f;
		// h[1] = 1.0f;
		//
		// ndMatrixTimeVector(2, &A[0][0], x, b);
		// dSolveDantzigLCP(2, &A[0][0], x, b, l, h);
		//
		// ndInt32 xxx = 0;
		// const ndInt32 xxxxxx = 450;
		// dDownHeap<ndInt32, unsigned> xxxxx (xxxxxx + 2);
		// for (ndInt32 i = 0; i < xxxxxx; ++i)
		//{
		//	xxxxx.Push (xxx, i);
		// }
		//
		// for (ndInt32 i = 0; i < 10000; ++i)
		//{
		//	ndInt32 index = dRandInt() % xxxxxx;
		//	ndInt32 key = xxxxx.Value(index);
		//	xxxxx.Remove (index);
		//	xxxxx.Push (xxx, key);
		// }
	}

	// ImGui - standalone example application for Glfw + OpenGL 2, using fixed pipeline
	// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
	PhysicsEntityManager::PhysicsEntityManager()
		: m_mainFrame(nullptr), m_world(nullptr), m_cameraManager(nullptr), m_microsecunds(0), m_transparentHeap(), m_currentScene(DEFAULT_SCENE), m_lastCurrentScene(DEFAULT_SCENE), m_framesCount(0), m_physicsFramesCount(0), m_currentPlugin(0), m_solverPasses(4), m_solverSubSteps(2), m_workerThreads(1), m_debugDisplayMode(0), m_collisionDisplayMode(0), m_selectedModel(nullptr), m_fps(0.0f), m_timestepAcc(0.0f), m_currentListenerTimestep(0.0f), m_addDeleteLock(), m_suspendPhysicsUpdate(false), m_synchronousPhysicsUpdate(false), m_synchronousParticlesUpdate(false), m_solverMode(ndWorld::ndSimdSoaSolver), m_replayLogFile(nullptr)
	{

		// initialized the physics world for the new scene
		// m_showUI = false;
		// m_showAABB = true;
		// m_hideVisualMeshes = true;
		// m_showScene = true;
		// m_showConcaveEdge = true;
		// m_autoSleepMode = false;
		// m_solverMode = ndWorld::ndOpenclSolver1;
		// m_solverMode = ndWorld::ndOpenclSolver2;
		// m_solverMode = ndWorld::ndSimdSoaSolver;
		// m_solverMode = ndWorld::ndCudaSolver;
		// m_solverMode = ndWorld::ndSimdAvx2Solver;
		m_solverMode = ndWorld::ndStandardSolver;
		// m_solverPasses = 4;
		m_workerThreads = 1;
		// m_solverSubSteps = 2;
		// m_showRaycastHit = true;
		// m_showCenterOfMass = false;
		// m_showNormalForces = true;
		// m_showContactPoints = true;
		// m_showJointDebugInfo = true;
		// m_showModelsDebugInfo = true;
		// m_collisionDisplayMode = 1;
		// m_collisionDisplayMode = 2;
		// m_collisionDisplayMode = 3;		// solid wire frame
		// m_synchronousPhysicsUpdate = false;

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
		ndTestDeedBrian();

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

		if (m_debugShapeCache)
		{
			delete m_debugShapeCache;
		}

		// destroy the empty world
		if (m_world)
		{
			delete m_world;
		}
	}

#ifdef _DEBUG
	void PhysicsEntityManager::OpenMessageCallback(GLenum source,
												   GLenum type,
												   GLuint id,
												   GLenum severity,
												   GLsizei length,
												   const GLchar *message,
												   const void *userParam)
	{
		if (userParam)
		{
			switch (id)
			{
			case 131185: // nvidia driver report will use VIDEO memory as the source for buffer object operations
				return;
			}
			ndTrace(("GL CALLBACK: %s source = 0x%x, type = 0x%x, id = %d, severity = 0x%x, message = %s, length = %d \n",
					 (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), source, type, id, severity, message, length));
			ndAssert(0);
		}
	}
#endif

	ndDemoCamera *PhysicsEntityManager::GetCamera() const
	{
		return m_cameraManager->GetCamera();
	}

	bool PhysicsEntityManager::GetKeyState(ndInt32 key) const
	{
		const ImGuiIO &io = ImGui::GetIO();
		bool state = io.KeysDown[key];
#ifdef ENABLE_REPLAY
#ifdef REPLAY_RECORD
		ndInt32 value = state;
		fwrite(&value, sizeof(ndInt32), 1, m_replayLogFile);
		fflush(m_replayLogFile);
#else
		ndInt32 value;
		fread(&value, sizeof(ndInt32), 1, m_replayLogFile);
		state = value ? 1 : 0;
#endif
#endif
		return state;
	}

	ndAnimationSequence *PhysicsEntityManager::GetAnimationSequence(const char *const fileName)
	{
		ndTree<ndAnimationSequence *, ndString>::ndNode *node = m_animationCache.Find(fileName);
		if (!node)
		{
			ndAnimationSequence *const sequence = LoadFbxAnimation(fileName);
			if (sequence)
			{
				node = m_animationCache.Insert(sequence, fileName);
			}
		}
		return node ? node->GetInfo() : nullptr;
	}

	bool PhysicsEntityManager::IsShiftKeyDown() const
	{
		const ImGuiIO &io = ImGui::GetIO();
		bool state = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		return state;
	}

	bool PhysicsEntityManager::IsControlKeyDown() const
	{
		const ImGuiIO &io = ImGui::GetIO();
		bool state = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		return state;
	}

	bool PhysicsEntityManager::GetCaptured() const
	{
		ImGuiIO &io = ImGui::GetIO();
		return io.WantCaptureMouse;
	}

	bool PhysicsEntityManager::GetMouseKeyState(ndInt32 button) const
	{
		ImGuiIO &io = ImGui::GetIO();
		return io.MouseDown[button];
	}

	void PhysicsEntityManager::Set2DDisplayRenderFunction(ndSharedPtr<ndUIEntity> &demoGui)
	{
		m_renderDemoGUI = demoGui;
	}

	void *PhysicsEntityManager::GetUpdateCameraContext() const
	{
		return m_updateCameraContext;
	}

	void PhysicsEntityManager::SetUpdateCameraFunction(UpdateCameraCallback callback, void *const context)
	{
		m_updateCamera = callback;
		m_updateCameraContext = context;
	}

	bool PhysicsEntityManager::JoystickDetected() const
	{
		return glfwJoystickPresent(0) ? true : false;
	}

	void PhysicsEntityManager::GetJoystickAxis(ndFixSizeArray<ndFloat32, 8> &axisValues)
	{
		ndAssert(JoystickDetected());
		ndInt32 axisCount = 0;
		axisValues.SetCount(0);
		const float *const axis = glfwGetJoystickAxes(0, &axisCount);
		axisCount = ndMin(axisCount, axisValues.GetCapacity());
		for (ndInt32 i = 0; i < axisCount; ++i)
		{
			axisValues.PushBack(axis[i]);
		}

#ifdef ENABLE_REPLAY
#ifdef REPLAY_RECORD
		fwrite(&axisCount, sizeof(axisCount), 1, m_replayLogFile);
		fwrite(&axisValues[0], sizeof(ndFloat32) * axisValues.GetCapacity(), 1, m_replayLogFile);
		fflush(m_replayLogFile);
#else
		fread(&axisCount, sizeof(axisCount), 1, m_replayLogFile);
		fread(&axisValues[0], sizeof(ndFloat32) * axisValues.GetCapacity(), 1, m_replayLogFile);
#endif
#endif
	}

	void PhysicsEntityManager::GetJoystickButtons(ndFixSizeArray<char, 32> &axisbuttons)
	{
		ndAssert(JoystickDetected());
		ndInt32 buttonsCount = 0;
		axisbuttons.SetCount(0);
		const unsigned char *const buttons = glfwGetJoystickButtons(0, &buttonsCount);
		buttonsCount = ndMin(buttonsCount, axisbuttons.GetCapacity());

		for (ndInt32 i = 0; i < buttonsCount; ++i)
		{
			axisbuttons.PushBack(char(buttons[i]));
			// if (buttons[i]) ndTrace(("%d %d\n", i, buttons[i]));
		}

#ifdef ENABLE_REPLAY
#ifdef REPLAY_RECORD
		fwrite(&buttonsCount, sizeof(buttonsCount), 1, m_replayLogFile);
		fwrite(&axisbuttons[0], sizeof(axisbuttons.GetCapacity()), 1, m_replayLogFile);
		fflush(m_replayLogFile);
#else
		fread(&buttonsCount, sizeof(buttonsCount), 1, m_replayLogFile);
		fread(&axisbuttons[0], sizeof(axisbuttons.GetCapacity()), 1, m_replayLogFile);
#endif
#endif
	}

	void PhysicsEntityManager::ResetTimer()
	{
		dResetTimer();
		m_microsecunds = ndGetTimeInMicroseconds();
	}

	void PhysicsEntityManager::AddEntity(PhysicsEntity *const ent)
	{
		ndScopeSpinLock lock(m_addDeleteLock);
		ndAssert(!ent->m_rootNode);
		ent->m_rootNode = Append(ent);
	}

	void PhysicsEntityManager::RemoveEntity(PhysicsEntity *const ent)
	{
		ndScopeSpinLock lock(m_addDeleteLock);
		ndAssert(ent->m_rootNode);
		Remove(ent->m_rootNode);
	}

	void PhysicsEntityManager::Cleanup()
	{
		// is we are run asynchronous we need make sure no update in on flight.
		if (m_world)
		{
			m_world->Sync();
		}

		// destroy the Newton world
		if (m_world)
		{
			const ndBodyListView &bodyList = m_world->GetBodyList();
			for (ndBodyListView::ndNode *bodyNode = bodyList.GetFirst(); bodyNode; bodyNode = bodyNode->GetNext())
			{
				ndBodyKinematic *const body = bodyNode->GetInfo()->GetAsBodyKinematic();
				PhysicsEntityNotify *const callback = (PhysicsEntityNotify *)body->GetNotifyCallback();
				if (callback)
				{
					callback->m_entity = nullptr;
				}
			}

			// get serialization call back before destroying the world
			m_world->CleanUp();
			delete m_world;
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

		ApplyMenuOptions();
	}

	void PhysicsEntityManager::ApplyMenuOptions()
	{
		m_world->Sync();
		m_world->SetSubSteps(m_solverSubSteps);
		m_world->SetSolverIterations(m_solverPasses);
		m_world->SetThreadCount(m_workerThreads);

		bool state = m_autoSleepMode ? true : false;
		const ndBodyListView &bodyList = m_world->GetBodyList();
		for (ndBodyListView::ndNode *node = bodyList.GetFirst(); node; node = node->GetNext())
		{
			ndBodyKinematic *const body = node->GetInfo()->GetAsBodyKinematic();
			body->SetAutoSleep(state);
		}

		SetParticleUpdateMode();
		m_world->SelectSolver(m_solverMode);
		m_solverMode = m_world->GetSelectedSolver();
	}

	void PhysicsEntityManager::ErrorCallback(ndInt32 error, const char *description)
	{
		ndTrace(("Error %d: %s\n", error, description));
		fprintf(stderr, "Error %d: %s\n", error, description);
		ndAssert(0);
	}

	void PhysicsEntityManager::MouseButtonCallback(GLFWwindow *, ndInt32 button, ndInt32 action, ndInt32)
	{
		if (button >= 0 && button < 3)
		{
			ImGuiIO &io = ImGui::GetIO();
			if (action == GLFW_PRESS)
			{
				io.MouseDown[button] = true;
			}
			else if (action == GLFW_RELEASE)
			{
				io.MouseDown[button] = false;
			}
		}
	}

	void PhysicsEntityManager::MouseScrollCallback(GLFWwindow *const, double, double y)
	{
		ImGuiIO &io = ImGui::GetIO();
		io.MouseWheel += float(y);
	}

	void PhysicsEntityManager::CursorposCallback(GLFWwindow *, double x, double y)
	{
		ImGuiIO &io = ImGui::GetIO();
		io.MousePos = ImVec2((float)x, (float)y);
	}

	bool PhysicsEntityManager::GetMouseSpeed(ndFloat32 &speedX, ndFloat32 &speedY) const
	{
		ImVec2 speed(ImGui::GetMouseDragDelta(0, 0.0f));
		speedX = speed.x;
		speedY = speed.y;
		return true;
	}

	bool PhysicsEntityManager::GetMousePosition(ndFloat32 &posX, ndFloat32 &posY) const
	{
		ImVec2 posit(ImGui::GetMousePos());
		posX = posit.x;
		posY = posit.y;
		return true;
	}

	void PhysicsEntityManager::CharCallback(GLFWwindow *, ndUnsigned32 ch)
	{
		ImGuiIO &io = ImGui::GetIO();
		io.AddInputCharacter((unsigned short)ch);
	}

	void PhysicsEntityManager::KeyCallback(GLFWwindow *const window, ndInt32 key, ndInt32, ndInt32 action, ndInt32 mods)
	{
		ImGuiIO &io = ImGui::GetIO();
		if (action == GLFW_PRESS)
			io.KeysDown[key] = true;
		if (action == GLFW_RELEASE)
			io.KeysDown[key] = false;

		(void)mods; // Modifiers are not reliable across systems
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		static ndInt32 prevKey;
		PhysicsEntityManager *const manager = (PhysicsEntityManager *)glfwGetWindowUserPointer(window);
		if ((key == GLFW_KEY_F10) && (key != prevKey))
		{
			manager->m_profilerMode = true;
		}

		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, 1);
		}

		if (key == GLFW_KEY_F1)
		{
			ndMatrix cameraMatrix(manager->GetCamera()->GetCurrentMatrix());
			manager->LoadDemo(manager->m_lastCurrentScene);
			manager->SetCameraMatrix(cameraMatrix, cameraMatrix.m_posit);
		}

		prevKey = io.KeysDown[key] ? key : 0;
	}

	void PhysicsEntityManager::ToggleProfiler()
	{
#ifdef D_PROFILER
		ndAssert(m_world);
		ndTrace(("profiler Enable\n"));
		m_world->Sync();
		dProfilerEnableProling();
#endif
	}

	void PhysicsEntityManager::BeginFrame()
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		ImGuiIO &io = ImGui::GetIO();

		// Setup display size (every frame to accommodate for window resizing)
		ndInt32 w, h;
		ndInt32 display_w, display_h;
		glfwGetWindowSize(m_mainFrame, &w, &h);
		glfwGetFramebufferSize(m_mainFrame, &display_w, &display_h);
		io.DisplaySize = ImVec2((ndReal)w, (ndReal)h);
		io.DisplayFramebufferScale = ImVec2(w > 0 ? ((ndReal)display_w / (ndReal)w) : 0, h > 0 ? ((ndReal)display_h / (ndReal)h) : 0);

		// int display_w, display_h;
		// glfwGetFramebufferSize(m_mainFrame, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	ndInt32 PhysicsEntityManager::ParticleCount() const
	{
		ndInt32 count = 0;
		const ndBodyList &particles = m_world->GetParticleList();
		for (ndBodyList::ndNode *node = particles.GetFirst(); node; node = node->GetNext())
		{
			ndBodyParticleSet *const set = node->GetInfo()->GetAsBodyParticleSet();
			count += set->GetPositions().GetCount();
		}
		return count;
	}

	void PhysicsEntityManager::SetParticleUpdateMode() const
	{
		const ndBodyList &particles = m_world->GetParticleList();
		for (ndBodyList::ndNode *node = particles.GetFirst(); node; node = node->GetNext())
		{
			ndBodyParticleSet *const set = node->GetInfo()->GetAsBodyParticleSet();
			set->SetAsynUpdate(!m_synchronousParticlesUpdate);
		}
	}

	void PhysicsEntityManager::RenderStats()
	{
		if (m_showStats)
		{
			char text[1024];

			if (ImGui::Begin("statistics", &m_showStats))
			{
				sprintf(text, "fps:            %6.3f", m_fps);
				ImGui::Text(text, "");

				sprintf(text, "physics time:  %6.3f ms", m_world->GetAverageUpdateTime() * 1.0e3f);
				ImGui::Text(text, "");

				if (m_world->IsGPU())
				{
					sprintf(text, "gpu     time:  %6.3f ms", m_world->GetExtensionAverageUpdateTime() * 1.0e3f);
					ImGui::Text(text, "");
				}

				sprintf(text, "update mode:    %s", m_synchronousPhysicsUpdate ? "synchronous" : "asynchronous");
				ImGui::Text(text, "");

				sprintf(text, "particle mode:  %s", m_synchronousParticlesUpdate ? "synchronous" : "asynchronous");
				ImGui::Text(text, "");

				sprintf(text, "bodies:         %d", m_world->GetBodyList().GetCount());
				ImGui::Text(text, "");

				sprintf(text, "joints:         %d", m_world->GetJointList().GetCount());
				ImGui::Text(text, "");

				sprintf(text, "contact joints: %d", m_world->GetContactList().GetCount());
				ImGui::Text(text, "");

				sprintf(text, "particles:      %d", ParticleCount());
				ImGui::Text(text, "");

				sprintf(text, "memory used:   %6.3f mbytes", ndFloat32(ndFloat64(ndMemory::GetMemoryUsed()) / (1024 * 1024)));
				ImGui::Text(text, "");

				sprintf(text, "threads:        %d", m_world->GetThreadCount());
				ImGui::Text(text, "");

				sprintf(text, "iterations:     %d", m_world->GetSolverIterations());
				ImGui::Text(text, "");

				sprintf(text, "Substeps:       %d", m_world->GetSubSteps());
				ImGui::Text(text, "");

				sprintf(text, "solver:         %s", m_world->GetSolverString());
				ImGui::Text(text, "");

				m_suspendPhysicsUpdate = m_suspendPhysicsUpdate || (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0));
				ImGui::End();
			}
		}

		if (m_showUI && *m_renderDemoGUI)
		{
			if (ImGui::Begin("User Interface", &m_showUI))
			{
				m_renderDemoGUI->RenderHelp();
				ImGui::End();
			}
		}

		ShowMainMenuBar();
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

	void PhysicsEntityManager::PushTransparentMesh(const ndDemoMeshInterface *const mesh, const ndMatrix &modelMatrix)
	{
		ndVector dist(m_cameraManager->GetCamera()->GetViewMatrix().TransformVector(modelMatrix.m_posit));
		TransparentMesh entry(modelMatrix, (ndDemoMesh *)mesh);
		m_transparentHeap.Push(entry, dist.m_z);
	}

	// void PhysicsEntityManager::ImportPLYfile (const char* const fileName)
	void PhysicsEntityManager::ImportPLYfile(const char *const)
	{
		ndAssert(0);
		// m_collisionDisplayMode = 2;
		// CreatePLYMesh (this, fileName, true);
	}

	ndInt32 PhysicsEntityManager::Print(const ndVector &, const char *fmt, ...) const
	{
		va_list argptr;
		char string[1024];

		va_start(argptr, fmt);
		vsprintf(string, fmt, argptr);
		va_end(argptr);
		ImGui::Text(string, "");
		return 0;
	}

	void PhysicsEntityManager::SetCameraMatrix(const ndQuaternion &rotation, const ndVector &position)
	{
		m_cameraManager->SetCameraMatrix(rotation, position);
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

	void PhysicsEntityManager::RenderScene(ImDrawData *const draw_data)
	{
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		ImGuiIO &io = ImGui::GetIO();

		ndInt32 fb_width = (ndInt32)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		ndInt32 fb_height = (ndInt32)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
		if (fb_width == 0 || fb_height == 0)
		{
			return;
		}

		PhysicsEntityManager *const window = (PhysicsEntityManager *)io.UserData;

		ImVec4 clearColor = ImColor(114, 144, 154);
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);

		window->RenderScene();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_TEXTURE_2D);
		// glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context

		// Setup viewport, orthographic projection matrix
		glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		if (*window->m_renderDemoGUI)
		{
			window->m_renderDemoGUI->RenderUI();
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		// Render command lists
		draw_data->ScaleClipRects(io.DisplayFramebufferScale);
		// #define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
		for (ndInt32 n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList *cmd_list = draw_data->CmdLists[n];
			const ImDrawVert *vtx_buffer = cmd_list->VtxBuffer.Data;
			const ImDrawIdx *idx_buffer = cmd_list->IdxBuffer.Data;
			glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void *)((char *)vtx_buffer + OFFSETOF(ImDrawVert, pos)));
			glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void *)((char *)vtx_buffer + OFFSETOF(ImDrawVert, uv)));
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void *)((char *)vtx_buffer + OFFSETOF(ImDrawVert, col)));

			for (ndInt32 cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					glScissor((ndInt32)pcmd->ClipRect.x, (ndInt32)((ndFloat32)fb_height - pcmd->ClipRect.w), (ndInt32)(pcmd->ClipRect.z - pcmd->ClipRect.x), (ndInt32)(pcmd->ClipRect.w - pcmd->ClipRect.y));
					glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
				}
				idx_buffer += pcmd->ElemCount;
			}
		}
#undef OFFSETOF

		// Restore modified state
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();
	}

	void PhysicsEntityManager::RenderScene()
	{
		D_TRACKTIME();
		ndFloat32 timestep = dGetElapsedSeconds();
		CalculateFPS(timestep);
		UpdatePhysics(timestep);

		// Get the interpolated location of each body in the scene
		ndFloat32 interpolateParam = CalculateInteplationParam();
		m_cameraManager->InterpolateMatrices(this, interpolateParam);

		ImGuiIO &io = ImGui::GetIO();
		ndInt32 display_w = (ndInt32)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		ndInt32 display_h = (ndInt32)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
		glViewport(0, 0, display_w, display_h);
		glScissor(0, 0, display_w, display_h);
		glEnable(GL_SCISSOR_TEST);

		// Culling.
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);

		//	glEnable(GL_DITHER);
		// z buffer test
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Setup camera matrix
		m_cameraManager->GetCamera()->SetViewMatrix(display_w, display_h);

		// render all entities
		const ndMatrix globalMatrix(ndGetIdentityMatrix());
		if (m_hideVisualMeshes)
		{
			if (m_sky)
			{
				m_sky->Render(timestep, this, globalMatrix);
			}
		}
		else
		{
			for (ndNode *node = ndList<PhysicsEntity *>::GetFirst(); node; node = node->GetNext())
			{
				PhysicsEntity *const entity = node->GetInfo();
				entity->Render(timestep, this, globalMatrix);
			}

			while (m_transparentHeap.GetCount())
			{
				const TransparentMesh &transparentMesh = m_transparentHeap[0];
				transparentMesh.m_mesh->RenderTransparency(this, transparentMesh.m_matrix);
				m_transparentHeap.Pop();
			}
		}

		if (m_showMeshSkeleton)
		{
			for (ndNode *node = ndList<PhysicsEntity *>::GetFirst(); node; node = node->GetNext())
			{
				PhysicsEntity *const entity = node->GetInfo();
				entity->RenderSkeleton(this, globalMatrix);
			}
		}

		if (m_showContactPoints)
		{
			m_world->Sync();
			RenderContactPoints(this);
		}

		if (m_showAABB)
		{
			m_world->Sync();
			RenderBodiesAABB(this);
		}

		if (m_showScene)
		{
			m_world->Sync();
			RenderWorldScene(this);
		}

		// if (m_showRaycastHit) {
		//	RenderRayCastHit(m_world);
		// }

		if (m_showJointDebugInfo)
		{
			m_world->Sync();
			RenderJointsDebugInfo(this);
		}

		if (m_showModelsDebugInfo)
		{
			m_world->Sync();
			RenderModelsDebugInfo(this);
		}

		if (m_showBodyFrame)
		{
			m_world->Sync();
			RenderBodyFrame(this);
		}

		if (m_showCenterOfMass)
		{
			m_world->Sync();
			RenderCenterOfMass(this);
		}

		if (m_showNormalForces)
		{
			m_world->Sync();
			RenderContactPoints(this);
			RenderNormalForces(this);
		}

		if (m_collisionDisplayMode)
		{
			m_world->Sync();
			DrawDebugShapes();
		}
	}

	void PhysicsEntityManager::TestImGui()
	{
		// Main loop
		bool show_demo_window = true;
		bool show_another_window = false;

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
		{
			ImGui::ShowDemoWindow(&show_demo_window);
		}

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		if (1)
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");		   // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
	}

	void PhysicsEntityManager::Run()
	{
		// Main loop
		ndFloatExceptions exception;
		while (!glfwWindowShouldClose(m_mainFrame))
		{
			if (m_profilerMode)
			{
				ToggleProfiler();
				m_profilerMode = false;
			}

			m_suspendPhysicsUpdate = false;
			D_TRACKTIME();

			BeginFrame();
			RenderStats();

			// TestImGui();

			// Rendering
			ImGui::Render();
			RenderScene(ImGui::GetDrawData());

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(m_mainFrame);
		}
	}

}