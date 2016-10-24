#ifndef _SHADER_BRDF_H_
#define _SHADER_BRDF_H_

#include "Shader.h"

class ShaderBRDF: public Shader {

public :
	ShaderBRDF();
	~ShaderBRDF();

	GLuint p_matrixMVPID;
	GLuint p_sundirection;
	GLuint p_position;

};

#endif //_SHADER_BRDF_H_