// transferRadiance.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

//std::vector<glm::vec3> cloud_mvs_normal;
//std::vector<glm::vec3> cloud_mvs_position;
//std::vector<std::vector<unsigned int> > camVisibility;

float ZNEAR= 110000.0f;
float ZFAR =0.0f;

int main(int argc, char** argv){

	CConfigLib::option(argc,argv);
	CConfigLib::_assert();

	std::string  path_radiance_data = CConfigLib::getOutRadianceFolder();


	std::vector<unsigned int> _xml_image_vec_id =CConfigLib::getImagesSet();

	std::vector< std::shared_ptr<Camera> > _cam_vec;
	std::vector<float> _clipping_planes;
	_clipping_planes.resize(2);

	clock_t t;
//	int f;
	t = clock();
	printf ("Calculating ... \n");
	_cam_vec =  loadBundleFileHDR(false,false) ;
	
	t = clock();
	printf ("Calculating...\n");
	//lire les nuages de points tout frais...
	std::vector<glm::vec3> _positionv =  dataRW::readPositionArray(path_radiance_data+"mesh_vertices.bin");
	std::vector<glm::vec3> _normalv = dataRW::readNormalArray(path_radiance_data+"mesh_transfer_normal.bin");
	std::vector<std::vector<unsigned int> > _points_viz =dataRW::readPerPointCamerasListArray(path_radiance_data+"mesh_transfer_camviz.bin");
	
	std::vector<std::vector<std::pair<unsigned int, glm::vec3>>> _cache_radiance_per_points;

	//std::vector<unsigned int> mesh_transfer_id= dataRW::readCorrespondenceArray(path_radiance_data+"mesh_correspondence_to_point_cloud.bin");

	//cloud_mvs_position	= dataRW::readPositionArray(path_radiance_data+"cloud_vertices.bin");
	//cloud_mvs_normal =	dataRW::readNormalArray(path_radiance_data+"cloud_normal.bin");
	//camVisibility   =	dataRW::readPerPointCamerasListArray(path_radiance_data+"cloud_cam_viz.bin");

	t = clock() - t;
	printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);

	//transfer RADIANCE CODE, on the mesh for now :P
	//project points cost only 7s per image...even on stupid way
	
	_cache_radiance_per_points.clear();
	_cache_radiance_per_points.resize(_positionv.size());

	glm::vec2 p3d;
	unsigned int current_cam;
	unsigned int nbpoints_viz_in_cam;

	std::cout<<"_cam_vec.size()"<<_cam_vec.size()<<std::endl;
	for ( unsigned int cam_list_c =0; cam_list_c< _cam_vec.size() ; cam_list_c++) {

		t = clock();

		current_cam = cam_list_c;

		std::cout<<"cam process ..."<<cam_list_c<<std::endl;
		if(_cam_vec[current_cam]->active())
		{
			nbpoints_viz_in_cam=0;
			_cam_vec[current_cam]->fixPrincipalPoint((float)PhotoFlyXmlManager::getCamInf(current_cam).cfrm.ppx, (float) PhotoFlyXmlManager::getCamInf(current_cam).cfrm.ppy);

			cimg_library::CImg<float> transfer_current_cam(_cam_vec[current_cam]->getImgFilename().c_str());

			//go trough all the point clouds
			for( unsigned int kc=0; kc < _positionv.size();kc++){

				for ( unsigned int c=0; c< _points_viz[kc].size(); c++){

					p3d =_cam_vec[current_cam]->project( _positionv[kc]);
					if ( _points_viz[kc][c] == current_cam && p3d.x < _cam_vec[current_cam]->w() && p3d.y < _cam_vec[current_cam]->h() &&  p3d.y > 0 && p3d.x >0){

						_cache_radiance_per_points[kc].push_back(
							std::make_pair(  
							current_cam,	
							glm::vec3(
							transfer_current_cam( p3d.x, p3d.y,0,0),
							transfer_current_cam( p3d.x, p3d.y,0,1),
							transfer_current_cam( p3d.x, p3d.y,0,2) ) 
							));
						nbpoints_viz_in_cam++;
					}
				}
			}
			t = clock() - t;
			printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
				printf ("Nbpoints in this cam %d ",nbpoints_viz_in_cam);
		}
	}

	t = clock() - t;
	printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
	///test_projection_pc.save("have_fun_pc.tiff");
	
	dataRW::writeRadiancePerPoints(_cache_radiance_per_points,path_radiance_data+"have_fun_radiance.bin");


	return 0;
}

