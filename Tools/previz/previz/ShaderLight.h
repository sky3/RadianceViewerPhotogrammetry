#ifndef _SHADER_LIGHT_H_
#define _SHADER_LIGHT_H_

#include "Shader.h"

class ShaderLight: public Shader {
public :
	ShaderLight();
	~ShaderLight();

	GLuint p_matrixMVPID;
	GLuint p_sundirection;

};

#endif