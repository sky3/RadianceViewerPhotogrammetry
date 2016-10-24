#ifndef SHADER_H
#define SHADER_H

#include <GL\glew.h>

#include <QGLWidget>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
public:
	Shader(void);
	~Shader(void);
	GLuint id;
	bool LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

	GLuint uniformLocation(const char *uniform);
	void bind(){glUseProgram(id);};
	void unBind(){glUseProgram(0);};
};

#endif // SHADER_H

