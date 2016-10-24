#version 420 core

// Interpolated values from the vertex shaders
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 vertexNormal;
in vec3 colour_pc;

// Values that stay constant for the whole mesh

uniform vec3 coloro;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

// Ouput data
out vec3 color;

void main(){

	//vertexNormal;
	color = (vertexNormal )/65535.0;
}