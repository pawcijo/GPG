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
	external/imgui-1.90.4/examples/libs/glfw
	external/glfw-3.4/include/
	external/glm-1.0.1/
	external/glew-2.2.0/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_220
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
	src/SimpleShape/Square.cpp
)

target_link_libraries(OPGLPlayground
	${ALL_LIBS}
)
