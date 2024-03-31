# CMake entry point
cmake_minimum_required (VERSION 3.9.3)
project (OPGLPlayground)

set(CMAKE_CXX_STANDARD 20)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
 
# Compile external dependencies 
add_subdirectory (external)

include_directories(
	src/
	external/imgui/examples/libs/glfw
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
)

target_link_libraries(OPGLPlayground
	${ALL_LIBS}
)
