#ifndef _SHADER_NORMAL_H_
#define _SHADER_NORMAL_H_

#include "Shader.h"

class ShaderNormal: public Shader {
public :
	ShaderNormal();
	~ShaderNormal();

	GLuint p_matrixMVPID;

};

#endif