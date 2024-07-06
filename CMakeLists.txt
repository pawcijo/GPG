# CMake entry point
cmake_minimum_required (VERSION 3.9.3)
project (GPGPlayground)

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

set(IMGUI_DIR external/imgui/)

add_executable(${PROJECT_NAME} 
	main.cpp
	src/Vulkan/VulkanPipeLine.cpp
	src/Vulkan/VulkanApp.cpp
	src/Vulkan/VulkanShader.cpp
	src/Vulkan/VulkanValidation.hpp
	src/Vulkan/VulkanUtis.hpp

	src/tiny_obj_loader.h

	src/Common/Transform.cpp
	src/Common/Camera.cpp
	
	src/Common/Scene.cpp
	src/Common/SceneObject.cpp
	src/Vulkan/Model.cpp
	src/MyPhysics/PhysicsTimer.cpp

	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp 
	${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp 
	${IMGUI_DIR}/imgui.cpp 
	${IMGUI_DIR}/imgui_draw.cpp 
	${IMGUI_DIR}/imgui_demo.cpp 
	${IMGUI_DIR}/imgui_tables.cpp 
	${IMGUI_DIR}/imgui_widgets.cpp

)

target_link_libraries(${PROJECT_NAME}
	${ALL_LIBS}
)
