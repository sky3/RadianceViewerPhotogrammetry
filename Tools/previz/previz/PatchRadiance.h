#ifndef _PATCH_RADIANCE_H_
#define _PATCH_RADIANCE_H_

#include "common.h"
#include "CImg.h"
#include "qimage.h"

class PatchRadiance{

public :
	PatchRadiance(unsigned int grid_size =128 ); //size of the patch
	~PatchRadiance();

	void projectOnPatch( const glm::mat3x3 &local_frame, const glm::vec3 &normal_patch, const glm::vec3 &direction_in, const glm::vec3 &color, const  int & splat_size =0 ) ;

	cimg_library::CImg<float> getGridToImage(void);
	QImage getImg();

private :

	glm::uvec2 projectDirectionOnHemisphere( const glm::mat3x3 &local_frame, const glm::vec3 &normal_patch, const glm::vec3 &direction_in);
	void rasterize();

	std::vector<std::vector<glm::vec3>> _grid;
	unsigned int _resGrid; //square size

	//glm::vec3 _patch_normal;

};

#endif