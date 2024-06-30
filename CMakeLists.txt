# CMake entry point
cmake_minimum_required (VERSION 3.9.3)
project (OPGLPlayground)

set(CMAKE_CXX_STANDARD 23)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)


set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)

find_library(NEWTON_LIB ndNewton REQUIRED)
 
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
	external/newton-4.00/sdk/dCore/
	external/newton-4.00/sdk/dNewton/
	external/newton-4.00/sdk/dCollision/
	${SDL2_INCLUDE_DIRS}
	${CMAKE_INSTALL_PREFIX}/include/ndNewton
	${Vulkan_INCLUDE_DIRS}
	.
)

if(APPLE)
set(ALL_LIBS
	${OPENGL_LIBRARY}
	Vulkan::Vulkan
	glfw
	${SDL2_LIBRARIES}
	${NEWTON_LIB} 
	pthread
)
else()
set(ALL_LIBS
	${OPENGL_LIBRARY}
	Vulkan::Vulkan
	glfw
	GLEW
	${SDL2_LIBRARIES}
	${NEWTON_LIB} 
	pthread
)
endif()

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(OPGLPlayground 
	main.cpp
	src/AppWindowVulkan.cpp
	src/AppVulkan.cpp
	src/Vulkan/VulkanShader.cpp
	src/VulkanValidation.hpp
	src/Vulkan/VulkanUtis.hpp
	src/tiny_obj_loader.h
	
	#src/AppWindow.cpp
	#src/App.cpp
	#src/Shader.cpp
	#src/Camera.cpp
	#src/Transform.cpp
	#src/SimpleShape/Square.cpp
	#src/SimpleShape/Box.cpp
	#src/stb_image.cpp

	#src/MyPhysics/PhysicsScene.cpp
	#src/MyPhysics/PhysicsMemory.cpp
	#src/MyPhysics/PhysicsContactManager.cpp
	#src/MyPhysics/PhysicsIsland.cpp
	#src/MyPhysics/PhysicsBody.cpp
	#src/MyPhysics/PhysicsBox.cpp
	#src/MyPhysics/PhysicsContactSolver
	#src/MyPhysics/PhysicsBroadPhase
	#src/MyPhysics/PhysicsDynamicAABBTree.cpp
	#src/MyPhysics/PhysicsContact.cpp
	#src/MyPhysics/PhysicsCollide.cpp
	#src/MyPhysics/PhysicsClock.cpp
	#src/MyPhysics/PhysicsTimer.cpp
	#src/MyPhysics/Common.cpp
	#src/GameObject/GameObject.cpp
	#src/GameObject/GameObjectManager.cpp
	#src/Mesh/Mesh.cpp
	#external/imgui/imgui.cpp
	#external/imgui/imgui_draw.cpp
	#external/imgui/imgui_tables.cpp
	#external/imgui/imgui_widgets.cpp
	#external/imgui/imgui_demo.cpp
	#external/imgui/backends/imgui_impl_glfw.cpp 
	#external/imgui/backends/imgui_impl_opengl3.cpp
	
)

target_link_libraries(OPGLPlayground
	${ALL_LIBS}
)
