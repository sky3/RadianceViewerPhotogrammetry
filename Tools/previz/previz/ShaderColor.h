#ifndef _SHADER_COLOR_H_
#define _SHADER_COLOR_H_

#include "Shader.h"

class ShaderColor: public Shader {
public :
	ShaderColor();
	~ShaderColor();

	GLuint p_matrixMVPID;

};

#endif