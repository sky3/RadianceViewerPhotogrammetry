#include "PatchRadiance.h"
#include <qimage>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include "embree_raytracer.h"

#define GLM_FORCE_RADIANS
PatchRadiance::PatchRadiance( unsigned int grid_size):_resGrid(grid_size)
	{
		//_patch_normal(norm_patch)
		_grid.resize(grid_size);
		for (unsigned int k=0; k < grid_size;k++){
			_grid[k].resize(grid_size);
		}
}

PatchRadiance::~PatchRadiance(){

}

QImage PatchRadiance::getImg(){

	QImage _patch((int)_resGrid,(int)_resGrid,QImage::Format_RGB32);
	
	for (unsigned int x=0; x < _resGrid;x++){
		for (unsigned int y=0; y < _resGrid;y++){

			QRgb value = qRgb(std::min(_grid[x][y].r/255.0f,255.0f), std::min(_grid[x][y].g/255.0f,255.0f), std::min(_grid[x][y].b/255.0f,255.0f));
			_patch.setPixel(x,y,value);
		}
	}
	return _patch;
}

cimg_library::CImg<float> PatchRadiance::getGridToImage(void){

	cimg_library::CImg<float> viz(_resGrid,_resGrid,1,3);

	for (unsigned int x=0; x < _resGrid;x++){
		for (unsigned int y=0; y < _resGrid;y++){

			viz(x,y,0,0) = _grid[x][y].r;//*65535;
			viz(x,y,0,1) = _grid[x][y].g;//*65535;
			viz(x,y,0,2) = _grid[x][y].b;//*65535;
		}
	}
	return viz;
}

void PatchRadiance::projectOnPatch(const glm::mat3x3 &local_frame, const glm::vec3 &normal_patch, const glm::vec3 &direction_in, const glm::vec3 &color, const int & splat_size ){

	glm::uvec2 coord_xy = projectDirectionOnHemisphere(local_frame,normal_patch,direction_in);

	if( coord_xy.x < _resGrid &&  coord_xy.y <_resGrid && coord_xy.y >= 0 && coord_xy.x >= 0){

		//last splat wins...
		_grid[coord_xy.x][coord_xy.y] = color;

		if (splat_size >= 1){
			for ( int k1=-splat_size ; k1< splat_size; k1++)
				for ( int k2=-splat_size ; k2< splat_size; k2++){

					if ( coord_xy.x +k1 >= 0 &&  coord_xy.y +k2 >= 0 && coord_xy.x + k1 < _resGrid &&  coord_xy.y + k2 < _resGrid ){
						_grid[coord_xy.x+k1][coord_xy.y+k2] = color;
					}
				}
		}
	}
}

void PatchRadiance::rasterize(void){
	//nothing for now :P
}



glm::uvec2 PatchRadiance::projectDirectionOnHemisphere(const glm::mat3x3 &local_frame, const glm::vec3 &normal_patch, const glm::vec3 &direction_in  ){

	glm::vec3 norm_dir = glm::normalize(direction_in);
	glm::vec3 n = glm::normalize(normal_patch);

	embree::LinearSpace3<embree::Vec3<float>>  local_frame_embree = Emb::returnLocalFrame(embree::Vec3<float>(n.x,n.y,n.z) );

	if ( glm::dot( n,  norm_dir) >=0){ //hemisphere superior only

		norm_dir = local_frame*norm_dir;

		float s = 0.5f+glm::dot( norm_dir, glm::vec3(1.0,0.0,0.0))/( 2.0f* glm::length(norm_dir));
		float t = 0.5f+glm::dot( norm_dir, glm::vec3(0.0,1.0,0.0))/( 2.0f* glm::length(norm_dir));

		// s, t [0; 1]
		return glm::uvec2( (unsigned int) (s * (float)_resGrid),  (unsigned int) (t * (float)_resGrid));
	}

	else {
		return glm::uvec2(_resGrid*2,_resGrid*2); //discard sample 
	}
}

 #ifdef boumbo 

		/*
		glm::vec3 dx0 = glm::cross( glm::vec3(1.0f, 0.0f, 0.0f), n);
		glm::vec3 dx1 = glm::cross( glm::vec3(0.0f, 1.0f, 0.0f), n);

		dx0 = glm::normalize(dx0);
		dx1 = glm::normalize(dx1);

		glm::vec3 dx = dx0;

		if ( glm::dot( dx0,dx0) < glm::dot( dx1,dx1) ){
			dx = dx1;
		}
		
		glm::vec3 dy  =glm::normalize( glm::cross( n, dx));

		glm::mat3 rot33 =glm::transpose (glm::mat3( dx.x, dx.y ,dx.z, dy.x, dy.y ,dy.z, n.x,n.y,n.z));

		norm_dir = (rot33)*norm_dir; //ok 1st rotatipn

		*/
		/*
		embree::Vec3<float> embreeDirGlob =  embree::Vec3<float>(norm_dir.x, norm_dir.y, norm_dir.z);
		//embree::Vec3<float> embreeDirLoc = local_frame_embree.transposed()*embreeDirGlob;
		embree::Vec3<float> embreeDirLoc = local_frame_embree*embreeDirGlob;
		norm_dir = glm::vec3(embreeDirLoc.x, embreeDirLoc.y, embreeDirLoc.z);
		

		//2nd rotation
		//=> ici calcul de la seconde rotation
		/*
		glm::vec3 ab =rot33*dy;
		glm::vec3 aa =rot33*dx;

		float alpha = glm::acos(glm::dot(n*glm::vec3(1.0,1.0,0.0), glm::vec3(0.0,1.0,0.0)));

		glm::mat3x3 rot33y_axis = glm::mat3(glm::cos(alpha), 0,  glm::sin(alpha),
			0.0f,1.0f,0.0f, -glm::sin(alpha), 0, glm::cos(alpha)
			);
		glm::mat3x3 rot33z_axis = glm::mat3(glm::cos(alpha), -glm::sin(alpha),0,
			glm::sin(alpha), glm::cos(alpha), 0,
			0,0,1
			);

		glm::mat3x3  rot33x_axis = glm::mat3(1.0f,0.0f,0.0f,
			0, glm::cos(alpha) , -glm::sin(alpha),
			0, glm::sin(alpha), glm::cos(alpha));

			glm::mat3x3 rot33z_axis2 = glm::mat3(-glm::sin(alpha), glm::cos(alpha),0,
			glm::cos(alpha), glm::sin(alpha), 0,
			0,0,1
			);
			*/
		//norm_dir = 	rot33z_axis*norm_dir;

			//fin de la seconde rotation
			/*
		float p = 2.0f* glm::length(norm_dir); 
				
		float y_axis = norm_dir.y;

		float s = (0.5f +  norm_dir.x /p);
		float t = (0.5f +  y_axis /p);
		*/


glm::uvec2 PatchRadiance::projectDirectionOnHemisphere(const glm::vec3 &normal_patch, const glm::vec3 &direction_in  ){

	glm::vec3 norm_dir = glm::normalize(direction_in);//no bullshit...

	if ( glm::dot( normal_patch, norm_dir) >=0){ //hemisphere superior only
		float p = 2.0f*std::sqrt(norm_dir.x*norm_dir.x + norm_dir.z*norm_dir.z + (norm_dir.y)*(norm_dir.y) );

		float y_axis = 1.0f;
		if (std::abs(normal_patch.z)  > std::abs(normal_patch.y)){
			y_axis = norm_dir.y;
		}
		else{
			y_axis = - norm_dir.z;
		}

		float s = (0.5f +  norm_dir.x /p);
		float t = (0.5f +  y_axis /p);

		// s, t [0; 1]
		return glm::uvec2( (unsigned int) (s * (float)_resGrid),  (unsigned int) (t * (float)_resGrid));
	}

	else {
		return glm::uvec2(_resGrid*2,_resGrid*2); //discard sample 
	}
}
glm::uvec2 PatchRadiance::projectDirectionOnHemisphere(const glm::vec3 &normal_patch, const glm::vec3 &direction_in  ){

	glm::vec3 norm_dir = glm::normalize(direction_in);//no bullshit...
	
	glm::vec3  normal_p = glm::normalize(normal_patch);

	glm::vec3 dx0 = glm::cross(glm::vec3(1.0f,0.0f,0.0f),normal_p);
	glm::vec3 dx1 = glm::cross(glm::vec3(0.0f,0.0f,1.0f),normal_p);

	glm::vec3 dx;
	if ( glm::dot(dx0,dx0) > glm::dot(dx1,dx1)){
		dx = dx0;
	}
	else { 
		dx = dx1;
	}
		dx = dx0;

	glm::vec3 dy = glm::cross(normal_p,dx);

	float z  = glm::dot( norm_dir, glm::normalize(glm::vec3(dx +dy ) )); 
	float sa  = glm::dot( norm_dir, glm::normalize(glm::vec3(normal_p +dx ) )); 


	if ( glm::dot( normal_p, norm_dir) >=0){ //hemisphere superior only
		float p = std::sqrt(norm_dir.x*norm_dir.x + norm_dir.z*norm_dir.z + (norm_dir.y)*(norm_dir.y) );

		float s = ( 0.5 + sa/p);
		float t = ( 0.5 + z/p);

		// s, t [0; 1]
		return glm::uvec2( (unsigned int) (s * (float)_resGrid),  (unsigned int) (t * (float)_resGrid));
	}

	else {
		return glm::uvec2(_resGrid*2,_resGrid*2); //discard sample 
	}
}
#endif