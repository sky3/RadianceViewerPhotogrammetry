// patchRadiance.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

float ZNEAR =1000.0f;
float ZFAR =0.0f;

int main(int argc, char** argv){

	//each points has a radiance patch and irradiance patch...

	PatchRadiance _patch_test(glm::vec3(0.0,1.0,0.0),256);
	glm::vec3 normalcolor;
	glm::vec3 direction;

	float step =0.01f;

	float resolution =1.0f;
	for(float nz =-resolution;nz <resolution; nz+=step){
		for(float nx =-resolution;nx <resolution; nx+=step){
			for(float ny =-resolution; ny <resolution; ny+=step){

				direction = glm::vec3(nx,ny,nz);
				direction = glm::normalize(direction);
				normalcolor = glm::vec3(
					(direction.x +1.0f)/2.0f,
					(direction.y +1.0f)/2.0f,
					(direction.z +1.0f)/2.0f);

				_patch_test.projectOnPatch(direction,normalcolor,1);
			}}}

	cimg_library::CImg<float>  _test =_patch_test.getGridToImage();
	_test.save("bidon.png");
	#ifdef UNARY_TEST



	CConfigLib::option(argc,argv);
	CConfigLib::_assert();

	std::string  path_radiance_data = CConfigLib::getOutRadianceFolder();

	std::vector< std::shared_ptr<Camera> > _cam_vec;	
	std::vector<std::vector<std::pair<unsigned int, glm::vec3>>>  _cache_radiance_per_points ;
	std::vector<float> _clipping_planes; 

	std::vector<glm::vec3> _normal_per_points;
	std::vector<glm::vec3> _position_per_points;
	//load point radiance and cameras

	_cam_vec =  loadBundleFileHDR(false,false) ;
	_cache_radiance_per_points  = dataRW::readRadiancePerPoints(path_radiance_data+"have_fun_radiance.bin");
	_normal_per_points= dataRW::readNormalArray(path_radiance_data+"mesh_transfer_normal.bin");
	_position_per_points= dataRW::readNormalArray(path_radiance_data+"mesh_vertices.bin");

	std::vector<unsigned int> _xml_image_vec_id =CConfigLib::getImagesSet();


	//render a few points png...

	for ( unsigned int id =20000;  id< 21000/* _cache_radiance_per_points.size()*/; id++){

		PatchRadiance _patch_test(glm::vec3(0.0,1.0,0.0),256);

		for ( unsigned int k=0; k < _cache_radiance_per_points[id].size();k++){

			unsigned int cam_id = _cache_radiance_per_points[id][k].first;
			glm::vec3 radiance =_cache_radiance_per_points[id][k].second;
			glm::vec3 normal =_cam_vec[cam_id]->pos() - _position_per_points[id] ;

			normal = glm::normalize(normal);
			//---
			_patch_test.projectOnPatch(normal, radiance,3);
		}
		
		std::stringstream name_out;
		name_out<<"test_proj_";
		name_out<<id<"_";
		name_out<<".png";
	
		_patch_test.getGridToImage().save(name_out.str().c_str());
	}
	#endif UNARY_TEST
	return 0;
}

