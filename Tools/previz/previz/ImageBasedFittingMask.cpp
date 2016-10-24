#include "ImageBasedFittingMask.h"
#include "embree_raytracer.h"
#include "Camera.h"
#include "LoadBundleFile.h"
#include "PatchRadiance.h"

#include"ConfigLib.h"
#include "PhotoFlyXmlManager.h"
#include "DataRW.h"
#include "SABrdf.h"

ImageBasedFittingMask::ImageBasedFittingMask( ){

}
ImageBasedFittingMask::~ImageBasedFittingMask(){

}

void ImageBasedFittingMask::runComputation(void){

	try{
		unsigned int idGeom = 0;

		std::shared_ptr<Object3D> obj_3 = Emb::getObject3D(idGeom);


		//load the binary mask
		//cimg_library::CImg<unsigned int> _fitting_mask_1_region("F:/ToysRadiance/mask_segm_32_r.png");

		cimg_library::CImg<unsigned int> _fitting_mask_1_region("F:/ToysRadiance/resize_meanshift_32.png");

		
		//selected view 
		unsigned int cam_id_to_process = 23;

	
		std::vector<unsigned int> range_camera = CConfigLib::getImagesSet();
		std::cout<<"Camera views to unproject "<<range_camera.size()<<". "<<std::endl;

		//--load all cameras
		std::vector< std::shared_ptr<Camera> > _cam_vec;
		_cam_vec =  loadBundleFileHDR(false,false) ;

		std::vector<double> sundir_g = CConfigLib::getSunDirection();
		std::vector<double> suncolor_g = CConfigLib::getSunColor();
		glm::vec3  sundir= glm::normalize( glm::vec3(sundir_g[0],sundir_g[1], sundir_g[2]));
		glm::vec3 sundirection = sundir;
		glm::vec3 suncolor = glm::vec3(suncolor_g[0],suncolor_g[1], suncolor_g[2]);


		//make sure all cameras are ok
		for(unsigned int kc =0; kc < _cam_vec.size();kc++){
			_cam_vec[kc]->fixPrincipalPoint(PhotoFlyXmlManager::getCamInf(kc).cfrm.ppx,PhotoFlyXmlManager::getCamInf(kc).cfrm.ppy);
		}

		if(!Envmap::Instance().isLoaded()){
			//load only once..
			Envmap::Instance().load(CConfigLib::getEnvmap());
		}

		std::cout<<"test" <<CConfigLib::getOutRadianceFolder()<<std::endl;

		//load data
		std::string fn_radiance_bin = CConfigLib::getOutRadianceFolder()+std::string("have_fun_radiance.bin");
		std::string fn_mediane_bin = CConfigLib::getOutRadianceFolder()+std::string("mediane_radiance.bin");
		std::string fn_camviz_bin = CConfigLib::getOutRadianceFolder()+std::string("mesh_transfer_camviz.bin");

		//load the data store in bin
		//std::vector<glm::vec3> normal_array = dataRW::readNormalArray(fn_normal_bin.c_str());
		//std::vector<glm::vec3> position_array = dataRW::readPositionArray(fn_position_bin.c_str());

		std::vector<std::vector<unsigned int>> cam_Viz_array = dataRW::readPerPointCamerasListArray( fn_camviz_bin.c_str());

		std::vector<glm::vec3> mediane_radiance_array = dataRW::readGlmVec3Array( fn_mediane_bin.c_str());

		std::cout<<"[CHECK ARRAY SIZE] "<<cam_Viz_array.size()<<std::endl; //<<" "<<normal_array.size() <<" "<< position_array.size()<<std::endl;

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
		cimg_library::CImg<float> scale_projection_color(scale_projection);
		cimg_library::CImg<float> scale_projection_irradiance(scale_projection);

			cimg_library::CImg<float> scale_projection_specular_term(scale_projection);

		cimg_library::CImg<float> scale_projection_g(scale_projection);
		cimg_library::CImg<float> scale_projection_gg(scale_projection);

		cimg_library::CImg<float> mask_projection(scale_projection);
		cimg_library::CImg<float> scale_projection_mediane(scale_projection);


#pragma omp parallel for num_threads(10), shared( scale_projection, scale_projection_color, scale_projection_irradiance )
		for( int k =0; k <  cam_Viz_array.size() ; k++){

			//check if the point is visible in the view to processs
			for (unsigned int l = 0; l < cam_Viz_array[k].size(); l++){

				if ( cam_id_to_process ==  cam_Viz_array[k][l]){

					//project the points to the image

					glm::vec2 p2d =_cam_vec[cam_id_to_process]->project( obj_3->m_vertices[k]);
					glm::uvec2 up2d(std::floor(p2d.x),std::floor(p2d.y) ); // no splat

					glm::uvec2 ups2d(std::floor(p2d.x/scale_result),std::floor(p2d.y/scale_result) ); // no splat

					if( ups2d.x >0 && ups2d.x <scale_projection_color.width() &&  ups2d.y>0 && ups2d.y <scale_projection_color.height()  ){

						/*
						_copy( up2d.x, up2d.y,0,0) = mediane_radiance_array[k].r; 
						_copy( up2d.x, up2d.y,0,1) = mediane_radiance_array[k].g; 	
						_copy( up2d.x, up2d.y,0,2) = mediane_radiance_array[k].b; 
						*/
						
				glm::vec3 dir_mvs_cam = glm::normalize( _cam_vec[cam_id_to_process]->pos()- obj_3->m_vertices[k] );
				float half_plane = glm::dot ( dir_mvs_cam, _cam_vec[cam_id_to_process]->dir());

				
						if ( /* _fitting_mask_1_region(ups2d.x,ups2d.y,0,0) >128 */(half_plane < 0.0f) ){

							scale_projection_color( ups2d.x, ups2d.y,0,0)  = obj_3->m_color[k].r;
							scale_projection_color( ups2d.x, ups2d.y,0,1)  = obj_3->m_color[k].g;
							scale_projection_color ( ups2d.x, ups2d.y,0,2) = obj_3->m_color[k].b;

							scale_projection_mediane( ups2d.x, ups2d.y,0,0)  =  mediane_radiance_array[k].r;
							scale_projection_mediane( ups2d.x, ups2d.y,0,1)  =  mediane_radiance_array[k].g;
							scale_projection_mediane( ups2d.x, ups2d.y,0,2)  =  mediane_radiance_array[k].b;
						
				
							glm::vec3 _normal_tmp = glm::normalize(obj_3->m_normals[k]);

							embree::LinearSpace3<embree::Vec3<float>>  local_frame_embree = Emb::returnLocalFrame( embree::Vec3<float>(_normal_tmp.x, _normal_tmp.y, _normal_tmp.z) );

							glm::vec3 _irradiance_at_P= Emb::getIrradianceAtPoint(local_frame_embree, k, idGeom, Envmap::Instance() )/65535.0f;

							scale_projection_irradiance( ups2d.x, ups2d.y,0,0) =_irradiance_at_P.r*65535.0f;
							scale_projection_irradiance( ups2d.x, ups2d.y,0,1) = _irradiance_at_P.g*65535.0f;
							scale_projection_irradiance( ups2d.x, ups2d.y,0,2) = _irradiance_at_P.b*65535.0f;

							
							SABrdf experiment_one_point;

							experiment_one_point.setSunDirection(sundirection);
							experiment_one_point.setSunColor(suncolor);

							glm::mat3x3 local_frame_irradiance =glm::inverse( 
								glm::transpose(
								glm::mat3x3(local_frame_embree.row0().x, local_frame_embree.row0().y, local_frame_embree.row0().z, 
								local_frame_embree.row1().x, local_frame_embree.row1().y, local_frame_embree.row1().z, 
								local_frame_embree.row2().x, local_frame_embree.row2().y, local_frame_embree.row2().z)));// we do not vizualize irradiance in this case

							for ( unsigned int m=0; m < obj_3->_cache_radiance_per_points[k].size();m++){

								unsigned int cam_id =  obj_3->_cache_radiance_per_points[k][m].first;
								glm::vec3 radiance = obj_3->_cache_radiance_per_points[k][m].second;

								glm::vec3 normal_cam_dir =glm::normalize( _cam_vec[PhotoFlyXmlManager::getIdBundleToCamera(cam_id)]->pos() -obj_3->m_vertices[k]);
									
								experiment_one_point.addSamples(_normal_tmp, normal_cam_dir, radiance/65535.0f, _irradiance_at_P);

							}
													
								//experiment_one_point.solve();
							experiment_one_point.CERES_Solve();

							glm::vec3 _sample_view_dir = glm::normalize( _cam_vec[PhotoFlyXmlManager::getIdBundleToCamera(cam_id_to_process)]->pos() - obj_3->m_vertices[k])  ;
						

							glm::vec3 reflect = 2.0f * ( glm::dot(_normal_tmp, sundirection )) *_normal_tmp - sundirection;
							//light with sun content
							glm::vec3 fit_diffuse = experiment_one_point.getFitDiffuse()*std::max( 0.0f, glm::dot(_normal_tmp,sundirection ))*suncolor; //
							glm::vec3 fit_indirect= experiment_one_point.getFitDiffuse()*_irradiance_at_P*glm::one_over_pi<float>();
							 
							glm::vec3 fit_specular = experiment_one_point.getFitSpecular()*std::max(0.0f, (float) std::pow(glm::dot(reflect,_sample_view_dir), experiment_one_point.getFitLobeN() ) );

							//glm::vec3 fit_specular_trick = experiment_one_point.getFitSpecular()*std::max(0.0f, (float) std::pow(glm::dot(reflect,-_sample_view_dir), experiment_one_point.getFitLobeN() ) );

							glm::vec3 ncolor = fit_diffuse + fit_indirect + fit_specular  ;//;   + fit_indirect 
							
							//glm::vec3 ncolor_trick = fit_diffuse + fit_indirect +fit_specular_trick  ;//;

							scale_projection( ups2d.x, ups2d.y,0,0) = ncolor.r*65535.0f; 
							scale_projection( ups2d.x, ups2d.y,0,1) = ncolor.g*65535.0f; 	
							scale_projection( ups2d.x, ups2d.y,0,2) = ncolor.b*65535.0f; 

							scale_projection_g( ups2d.x, ups2d.y,0,0) = (fit_diffuse.r )*65535.0f; 
							scale_projection_g( ups2d.x, ups2d.y,0,1) = (fit_diffuse.g )*65535.0f;
							scale_projection_g( ups2d.x, ups2d.y,0,2) = (fit_diffuse.b )*65535.0f;  

							scale_projection_gg( ups2d.x, ups2d.y,0,0) = fit_indirect.r*65535.0f;
							scale_projection_gg( ups2d.x, ups2d.y,0,1) = fit_indirect.g*65535.0f;
							scale_projection_gg( ups2d.x, ups2d.y,0,2) = fit_indirect.b*65535.0f;

							scale_projection_specular_term( ups2d.x, ups2d.y, 0, 0) = fit_specular.r*65535.0f;
							scale_projection_specular_term( ups2d.x, ups2d.y, 0, 1) = fit_specular.g*65535.0f;
							scale_projection_specular_term( ups2d.x, ups2d.y, 0, 2) = fit_specular.b*65535.0f;

							mask_projection( ups2d.x, ups2d.y, 0, 0) = 65535.0f;
							mask_projection( ups2d.x, ups2d.y, 0, 1) = 65535.0f;
							mask_projection( ups2d.x, ups2d.y, 0, 2) = 65535.0f;

							



																	
						}
						else {



						}
					
					}
				

				}

			}

		}

		scale_projection_color.save("scale_projection_color.png");
		scale_projection.save("scale_projection_fit.png");
		scale_projection_mediane.save("scale_projection_mediane.png");

		mask_projection.save("mask.png");

		scale_projection_irradiance.save("scale_projection_irradiance.png");

		scale_projection_g.save("scale_projection_g.png");
		scale_projection_gg.save("scale_projection_gg.png");

		scale_projection_specular_term.save("scale_projection_specular_term.png");
	}
	catch(const std::exception & e) {
		std::cout<<e.what()<<std::endl;

	}
}
