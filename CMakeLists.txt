# CMake entry point
cmake_minimum_required (VERSION 3.9.3)
project (OPGLPlayground)

set(CMAKE_CXX_STANDARD 20)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
 

set(CMAKE_DEBUG_POSTFIX d)

# Compile external dependencies 
add_subdirectory (external)

include_directories(
	src/
	external/imgui/
	external/imgui/backends/
	external/glfw/include/
	external/glm/
	external/glew/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(OPGLPlayground 
	main.cpp
	src/AppWindow.cpp
	src/App.cpp
	src/Shader.cpp
	src/Camera.cpp
	src/Transform.cpp
	src/SimpleShape/Square.cpp
	src/SimpleShape/Box.cpp
	src/stb_image.cpp
	src/MyPhysics/PhysicsScene.cpp
	src/MyPhysics/PhysicsMemory.cpp
	src/MyPhysics/PhysicsContactManager.cpp
	src/MyPhysics/PhysicsIsland.cpp
	src/MyPhysics/PhysicsBody.cpp
	src/MyPhysics/PhysicsBox.cpp
	src/MyPhysics/PhysicsContactSolver
	src/MyPhysics/PhysicsBroadPhase
	src/MyPhysics/PhysicsDynamicAABBTree.cpp
	src/MyPhysics/PhysicsContact.cpp
	src/MyPhysics/PhysicsCollide.cpp
	src/MyPhysics/PhysicsClock.cpp
	src/MyPhysics/Common.cpp
	external/imgui/imgui.cpp
	external/imgui/imgui_draw.cpp
	external/imgui/imgui_tables.cpp
	external/imgui/imgui_widgets.cpp
	external/imgui/imgui_demo.cpp
	external/imgui/backends/imgui_impl_glfw.cpp 
	external/imgui/backends/imgui_impl_opengl3.cpp
	
)

target_link_libraries(OPGLPlayground
	${ALL_LIBS}
)
