#ifndef ENVMAP_H_
#define ENVMAP_H_
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <embree2\rtcore.h>
//#include <embree2\rtcore_ray.h>
//#include <embree2\rtcore_scene.h>
#include <math/vec3.h>
#include <math/linearspace3.h> 


class Envmap{
 
public :
	static  Envmap& Envmap::Instance();

	void load(std::string filename);

	unsigned int W();
	unsigned int H();

	glm::vec3 getEnvmapColor(glm::vec3 dir) const;
	bool isLoaded(void);

private:

	Envmap(const Envmap &e);
	Envmap& operator= (const Envmap &);

	static Envmap m_instance;

	unsigned int _width;
	unsigned int _height;

	float * envmap_array; // yes... it s an array of float
	std::string _filename;

	bool _loaded;

	Envmap();
	~Envmap();

};

#endif