#version 420 core

// Interpolated values from the vertex shaders

in vec3 vertexColor;

// Values that stay constant for the whole mesh

// Ouput data
out vec3 color;

void main(){

	//vertexColor;
	color = (vertexColor )/65535.0;
}