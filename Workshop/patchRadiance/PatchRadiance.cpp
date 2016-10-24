#include "stdafx.h"


PatchRadiance::PatchRadiance(const glm::vec3 & norm_patch, unsigned int grid_size):_resGrid(grid_size),
	_patch_normal(norm_patch){

		_grid.resize(grid_size);
		for (unsigned int k=0; k < grid_size;k++){
			_grid[k].resize(grid_size);
		}
}

PatchRadiance::~PatchRadiance(){

}

cimg_library::CImg<float> PatchRadiance::getGridToImage(void){

	cimg_library::CImg<float> viz(_resGrid,_resGrid,1,3);

	for (unsigned int x=0; x < _resGrid;x++){
		for (unsigned int y=0; y < _resGrid;y++){

			viz(x,y,0,0) = _grid[x][y].r*65535;
			viz(x,y,0,1) = _grid[x][y].g*65535;
			viz(x,y,0,2) = _grid[x][y].b*65535;
		}
	}
	return viz;
}

void PatchRadiance::projectOnPatch(const glm::vec3 &direction_in, const glm::vec3 &color, const int & splat_size ){

	glm::uvec2 coord_xy = projectDirectionOnHemisphere(direction_in);

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
	//nothing for nwo :P
}

glm::uvec2 PatchRadiance::projectDirectionOnHemisphere(const glm::vec3 &direction_in ){

	glm::vec3 norm_dir = glm::normalize(direction_in);//no bullshit...

	if ( true /*glm::dot( _patch_normal, norm_dir) >=0*/){ //hemisphere superior only
		float p = 2.0f*std::sqrt(norm_dir.x*norm_dir.x + norm_dir.z*norm_dir.z + (norm_dir.y)*(norm_dir.y) );

		float s = (0.5f +  norm_dir.x /p);
		float t = (0.5f +  norm_dir.y /p);

		// s, t [0; 1]
		return glm::uvec2( (unsigned int) (s * (float)_resGrid),  (unsigned int) (t * (float)_resGrid));
	}

	else {
		return glm::uvec2(_resGrid*2,_resGrid*2); //discard sample 
	}
}