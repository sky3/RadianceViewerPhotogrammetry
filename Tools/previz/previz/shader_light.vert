#version 420 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec3 vertexColor_modelspace;

// Output data ; will be interpolated for each fragment.
out vec3 vertexColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform vec3 Sundirection;
void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	float scal  = dot( vertexNormal_modelspace, Sundirection);
	vertexColor = (vertexNormal_modelspace +vec3(1.0,1.0,1.0))*0.5*scal;

}

