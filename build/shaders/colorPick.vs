#version 450 core

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 aPos;

// Values that stay constant for the whole mesh.
uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  projection * view * transform * vec4(aPos,1);
	
}
