// brdfReRendering.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

float ZNEAR  = 15000.0f;
float ZFAR = 0.00001f;
cimg_library::CImg <float> pallet ;


int main(int argc, char *argv[])
{

	//load the binary mask
	cimg_library::CImg<unsigned int> _fitting_mask_1_region("F:/ToysRadiance/mask_segm_32_r.png");


	CConfigLib::option(argc,argv);
	CConfigLib::_assert();

	std::vector<unsigned int> range_camera = CConfigLib::getImagesSet();
	std::cout<<"Camera views to unproject "<<range_camera.size()<<". "<<std::endl;

	//--load all cameras
	std::vector< std::shared_ptr<Camera> > _cam_vec;

	_cam_vec =  loadBundleFileHDR(false,false) ;

	for(unsigned int kc =0; kc < _cam_vec.size();kc++){
		_cam_vec[kc]->fixPrincipalPoint(PhotoFlyXmlManager::getCamInf(kc).cfrm.ppx,PhotoFlyXmlManager::getCamInf(kc).cfrm.ppy);
	}


	std::cout<<"test" <<CConfigLib::getOutRadianceFolder()<<std::endl;

	//load data
	std::string fn_radiance_bin = CConfigLib::getOutRadianceFolder()+std::string("have_fun_radiance.bin");
	std::string fn_normal_bin = CConfigLib::getOutRadianceFolder()+std::string("mesh_transfer_normal.bin");
	std::string fn_position_bin = CConfigLib::getOutRadianceFolder()+std::string("mesh_vertices.bin");

	std::string fn_mediane_bin = CConfigLib::getOutRadianceFolder()+std::string("mediane_radiance.bin");


	std::string fn_camviz_bin = CConfigLib::getOutRadianceFolder()+std::string("mesh_transfer_camviz.bin");



	//load the data store in bin
	std::vector<glm::vec3> normal_array = dataRW::readNormalArray(fn_normal_bin.c_str());
	std::vector<glm::vec3> position_array = dataRW::readPositionArray(fn_position_bin.c_str());

	std::vector<std::vector<unsigned int>> cam_Viz_array = dataRW::readPerPointCamerasListArray( fn_camviz_bin.c_str());

	std::vector<glm::vec3> mediane_radiance_array = dataRW::readGlmVec3Array( fn_mediane_bin.c_str());


	std::cout<<"[CHECK ARRAY SIZE]" <<mediane_radiance_array.size()<<" "<<cam_Viz_array.size()<<" "<<normal_array.size() <<" "<< position_array.size()<<std::endl;

	//selected view 
	unsigned int cam_id_to_process = 32;

	//create an image

	std::cout<<"[load image filename]" <<_cam_vec[cam_id_to_process]->getImgFilename()<<std::endl;


	cimg_library::CImg<float> _input_image( _cam_vec[cam_id_to_process]->getImgFilename().c_str());

	//duplicate
	cimg_library::CImg<float> _copy(_input_image);
	_copy.fill(0);

	cimg_library::CImg<float> scale_projection(_copy);

	//http://cimg.sourceforge.net/reference/structcimg__library_1_1CImg.html 
	unsigned int linear_interpolation = 3;

	float scale_result = 5.0f;
	scale_projection.resize(_input_image.width()/scale_result, _input_image.height()/scale_result, _input_image.depth(), _input_image.spectrum(), linear_interpolation);

	for( unsigned int k =0; k <  mediane_radiance_array.size() ; k++){

		//check if the point is visible in the view to processs
		for (unsigned int l = 0; l < cam_Viz_array[k].size(); l++){

			if ( cam_id_to_process ==  cam_Viz_array[k][l]){

				//project the points to the image

				glm::vec2 p2d =_cam_vec[cam_id_to_process]->project( position_array[k]);
				glm::uvec2 up2d(std::floor(p2d.x),std::floor(p2d.y) ); // no splat


				glm::uvec2 ups2d(std::floor(p2d.x/scale_result),std::floor(p2d.y/scale_result) ); // no splat

				if( up2d.x >0 && up2d.x <_copy.width() &&  up2d.y>0 && up2d.y <_copy.height()  ){

					_copy( up2d.x, up2d.y,0,0) = mediane_radiance_array[k].r; 
					_copy( up2d.x, up2d.y,0,1) = mediane_radiance_array[k].g; 	
					_copy( up2d.x, up2d.y,0,2) = mediane_radiance_array[k].b; 


					if (  _fitting_mask_1_region(ups2d.x,ups2d.y,0,0) >128 ){

						scale_projection( ups2d.x, ups2d.y,0,0) = mediane_radiance_array[k].r; 
						scale_projection( ups2d.x, ups2d.y,0,1) = mediane_radiance_array[k].g; 	
						scale_projection( ups2d.x, ups2d.y,0,2) = mediane_radiance_array[k].b; 
					}

				}
			}

		}

	}


	_copy.save("toto.png");
	scale_projection.save("toto_scale.png");


	//std::vector<std::vector<std::pair<unsigned int, glm::vec3>>>  radiance_per_points = dataRW::readRadiancePerPoints(fn_radiance_bin.c_str());



	//load an image


	//load the camera

	//project mediane to test


	//

	return 0;
}

