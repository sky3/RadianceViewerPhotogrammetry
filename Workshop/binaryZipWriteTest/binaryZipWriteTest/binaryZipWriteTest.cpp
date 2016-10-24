// binaryZipWriteTest.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"


/*
NEVER CHANGE TYPE WRITTEN IN FILE...

POSITION -> N elements (int), array ( float, float, float)
NORMAL   -> N elements (int), array ( float, float, float)

*/

float ZNEAR= 110000.0f;
float ZFAR =0.0f;

#include <time.h>

#include "CImg.h"

#include "NN_header.h"




pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_mvs_xyz;
std::vector<glm::vec3> cloud_mvs_position;
std::vector<glm::vec3> cloud_mvs_normal;
std::vector<glm::vec3> cloud_mvs_color;
std::vector<float> cloud_mvs_confidence;
std::vector<std::vector<unsigned int> > camVisibility;

pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_mesh_xyz;

std::vector<glm::vec3> mesh_point;
std::vector<glm::vec3> mesh_normal;
std::vector<glm::uvec3> mesh_face;
std::vector<std::vector<unsigned int> > mesh_camVisibility;

std::vector<glm::vec4> mesh_transfer_color;///color + confidence
std::vector<glm::vec3> mesh_transfer_normal;///normal

std::ofstream ply_file;
std::ofstream variance_file;


int main(int argc, char** argv){

	
	CConfigLib::option(argc,argv);
	CConfigLib::_assert();

	std::vector<unsigned int> _xml_image_vec_id =CConfigLib::getImagesSet();

	std::vector< std::shared_ptr<Camera> > _cam_vec;
	std::vector<float> _clipping_planes;
	_clipping_planes.resize(2);

	clock_t t;
	int f;
	t = clock();
	printf ("Calculating ... \n");
	_cam_vec =  loadBundleFileHDR(false,false) ;
	
	unsigned int cam_to_test =12;
	_cam_vec[cam_to_test]->fixPrincipalPoint(PhotoFlyXmlManager::getCamInf(cam_to_test).cfrm.ppx,PhotoFlyXmlManager::getCamInf(cam_to_test).cfrm.ppy);


	t = clock();
	printf ("Calculating...\n");
	//lire les nuages de points tout frais...
	std::vector<glm::vec3> _positionv =  dataRW::readPositionArray("mesh_vertices.bin");
	std::vector<glm::vec3> _normalv = dataRW::readNormalArray("mesh_transfer_normal.bin");
	std::vector<std::vector<unsigned int> > _points_viz =dataRW::readPerPointCamerasListArray("mesh_transfer_camviz.bin");
	
	std::vector<std::vector<std::pair<unsigned int, glm::vec3>>> _cache_radiance_per_points;

	std::vector<unsigned int> mesh_transfer_id= dataRW::readCorrespondenceArray("mesh_correspondence_to_point_cloud.bin");

	cloud_mvs_position	= dataRW::readPositionArray("cloud_vertices.bin");
	cloud_mvs_normal	=	dataRW::readNormalArray("cloud_normal.bin");
	camVisibility =	dataRW::readPerPointCamerasListArray("cloud_cam_viz.bin");

	t = clock() - t;
	printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
	bool exist;


	/*
	//read an image
	cimg_library::CImg<float> test_projection_pc(_cam_vec[cam_to_test]->getImgFilename().c_str());
	cimg_library::CImg<float> test_projection_mesh(_cam_vec[cam_to_test]->getImgFilename().c_str());
	
	for ( unsigned int s=0; s< _points_viz.size(); s++)
		for( unsigned int kc=0; kc < _points_viz[s].size();kc++){
			if ( _points_viz[s][kc] ==cam_to_test){
				glm::vec2 p3d =_cam_vec[cam_to_test]->project( _positionv[s]);
				if ( p3d.x < _cam_vec[cam_to_test]->w() && p3d.y < _cam_vec[cam_to_test]->h() &&  p3d.y > 0 && p3d.x >0){
					test_projection_mesh( p3d.x, p3d.y,0,0) = _normalv[s].x*2.0-1.0; 
					test_projection_mesh( p3d.x, p3d.y,0,1) = _normalv[s].y*2.0-1.0; 
					test_projection_mesh( p3d.x, p3d.y,0,2) = _normalv[s].z*2.0-1.0; 
				}
			}

		}
			t = clock() - t;
	printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
	test_projection_mesh.save("have_fun_mesh.tiff");

	for( unsigned int kc=0; kc < cloud_mvs_position.size();kc++){

		glm::vec2 p3d =_cam_vec[cam_to_test]->project( cloud_mvs_position[kc]);

		exist = false;
		for ( unsigned int c=0; c< camVisibility[kc].size(); c++){

			if (camVisibility[kc][c] == cam_to_test) exist= true;

			if ( exist && p3d.x < _cam_vec[cam_to_test]->w() && p3d.y < _cam_vec[cam_to_test]->h() &&  p3d.y > 0 && p3d.x >0){
				test_projection_pc( p3d.x, p3d.y,0,0) = cloud_mvs_normal[kc].x*2.0-1.0; 
				test_projection_pc( p3d.x, p3d.y,0,1) = cloud_mvs_normal[kc].y*2.0-1.0; 
				test_projection_pc( p3d.x, p3d.y,0,2) = cloud_mvs_normal[kc].z*2.0-1.0; 
			}
		}

	}

		t = clock() - t;
	printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
	test_projection_pc.save("have_fun_pc.tiff");
	*/
	
	//transfer RADIANCE CODE, on the mesh for now :P
	//project points cost only 7s per image...even on stupid way
		t = clock();
	_cache_radiance_per_points.clear();
	_cache_radiance_per_points.resize(_positionv.size());

	glm::vec2 p3d;
	for ( unsigned int cam_list_c =0; cam_list_c< _xml_image_vec_id.size() ; cam_list_c++) {

		std::cout<<"cam process ..."<<cam_list_c<<std::endl;

		cimg_library::CImg<float> transfer_current_cam(_cam_vec[_xml_image_vec_id[cam_list_c]]->getImgFilename().c_str());
	
		//go trough all the point clouds
		for( unsigned int kc=0; kc < cloud_mvs_position.size();kc++){

			exist = false;
			for ( unsigned int c=0; c< camVisibility[kc].size(); c++){

				if (camVisibility[kc][c] == cam_to_test) exist= true;

				p3d =_cam_vec[cam_list_c]->project( cloud_mvs_position[kc]);

				if ( exist && p3d.x < _cam_vec[cam_to_test]->w() && p3d.y < _cam_vec[cam_to_test]->h() &&  p3d.y > 0 && p3d.x >0){

					_cache_radiance_per_points[kc].push_back(
						std::make_pair(  
						cam_list_c,	
						glm::vec3(
						transfer_current_cam( p3d.x, p3d.y,0,0),
						transfer_current_cam( p3d.x, p3d.y,0,1),
						transfer_current_cam( p3d.x, p3d.y,0,2) ) 
						));

				;
				}
			}
		}
			t = clock() - t;
			printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
	}

	t = clock() - t;
	printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
	///test_projection_pc.save("have_fun_pc.tiff");
	
	dataRW::writeRadiancePerPoints(_cache_radiance_per_points,"have_fun_radiance.bin");



	
	//_cam_vec
	/*

	//data to dump 
	int nb_element =1500005*3;

	std::vector<float> pos;
	pos.resize(nb_element);

	for ( int i=0; i < nb_element ; i++){
	pos[i] = i;
	}

	float * tmp_ptr_float=&pos[0];

	//float a = 1.2;


	std::string name_file_bin="toto_bin";

	//-- WRITING

	std::ofstream sortie (name_file_bin.c_str(), std::ios::out | std::ios::binary);

	sortie.write((char *)&nb_element, sizeof(int));
	sortie.write((char *)tmp_ptr_float, sizeof(float)*nb_element);

	sortie.close();


	pos.clear();

	nb_element =0;


	//-- READING 

	std::ifstream entree (name_file_bin.c_str(), std::ios::out | std::ios::binary);

	entree.read((char *)&nb_element, sizeof(int));

	float *  _pos =  new float[nb_element];

	pos.resize(nb_element);

	//entree.read((char *)_pos.get(), sizeof(float)*nb_element);
	float a=0.0;
	for ( unsigned int i =0 ; i < nb_element; i++){
	//_pos[i].get()
	entree.read((char *)&a, sizeof(float));
	_pos[i] = a; 
	}
	entree.close();

	//-- AFFICHAGE
	std::cout <<"nb_element"<<nb_element <<" "<< " a  = " << _pos[0]<<" "<< 	_pos[4]<<std::endl;

	delete [] _pos;

	*/
	//here fun with GLM 

#ifdef  UNARY_BINARY_TEST

	std::vector<glm::vec3>  fun;
	fun.resize( 100);
	for( int i =0; i< fun.size(); i++){

		fun[i].x = i*1;
		fun[i].y = i*2;
		fun[i].z = i*3;

	}

	dataRW::writePositionArray(fun,"dodo.bin");

	std::vector<glm::vec3> dodo_back = dataRW::readPositionArray("dodo.bin");

	for( int i =0; i< dodo_back.size(); i++){

		std::cout<<dodo_back[i].x<<" "<<dodo_back[i].y<<" "<<dodo_back[i].z<<std::endl;

	}


	//color index array 

	std::vector<std::vector<glm::vec3>> fun_color;
	fun_color.resize(30);

	for( int i =0; i< fun_color.size(); i++){

		fun_color[i].resize(i);
		for( int j =0; j< fun_color[i].size(); j++){

			fun_color[i][j].x = i*1.0 +j;
			fun_color[i][j].y = i*2.05 +j;
			fun_color[i][j].z = i*3.56556 +j ;
		}


	}

	std::vector<std::vector<unsigned int>> fun_camera;
	fun_camera.resize(30);

	for( int i =0; i< fun_camera.size(); i++){

		fun_camera[i].resize(i);
		for( int j =0; j< fun_camera[i].size(); j++){

			fun_camera[i][j] = i +j;

		}


	}

	dataRW::writeListPerIndex(fun_camera,"haha.bin");
	fun_camera.clear();


	fun_camera = dataRW::readListPerIndex("haha.bin");
	for( int i =0; i< fun_camera.size(); i++){

		for( int j =0; j< fun_camera[i].size(); j++){

			std::cout<<fun_camera[i][j]<<" c ";
		}
		std::cout<<" "<<std::endl;
	}
#endif 

//#define WRITE_BINARY 1
#ifdef  WRITE_BINARY
	//here start experiment
	try {

		CConfigLib::option(argc,argv);
		CConfigLib::_assert();
		std::vector< std::shared_ptr<Camera> > _cam_vec;
		std::vector<float> _clipping_planes;
		_clipping_planes.resize(2);

		clock_t t;
		int f;
		t = clock();
		printf ("Calculating... \n");
		_cam_vec =  loadBundleFileHDR(false,false) ;

		std::vector< std::shared_ptr<MVSPoint> >_mvs_list= loadPMVSPoints(_cam_vec);

		std::cout<<"MVS Point cloud size"<<_mvs_list.size()<<std::endl;
		
		cloud_mvs_xyz.reset(new pcl::PointCloud<pcl::PointXYZ>);


		cloud_mvs_xyz->clear();
		cloud_mvs_normal.clear();
		cloud_mvs_color.clear();
		cloud_mvs_confidence.clear();
		camVisibility.clear();

		int numPoints = _mvs_list.size();

		cloud_mvs_position.resize(numPoints);
		cloud_mvs_normal.resize(numPoints);
		cloud_mvs_color.resize(numPoints);
		cloud_mvs_confidence.resize(numPoints);
		camVisibility.resize(numPoints);


		// Generate pointcloud data
		cloud_mvs_xyz->width = numPoints;
		cloud_mvs_xyz->height = 1;
		cloud_mvs_xyz->points.resize (cloud_mvs_xyz->width * cloud_mvs_xyz->height);


		
		for (int i=0; i<numPoints; i++) {

			unsigned int nbCam=0;

			//maybe a cast could workd...
			cloud_mvs_xyz->points[i].x = _mvs_list[i]->pos3D().x;
			cloud_mvs_xyz->points[i].y = _mvs_list[i]->pos3D().y;
			cloud_mvs_xyz->points[i].z = _mvs_list[i]->pos3D().z;

			cloud_mvs_position[i][0] = _mvs_list[i]->pos3D().x;
			cloud_mvs_position[i][1] = _mvs_list[i]->pos3D().y;
			cloud_mvs_position[i][2] = _mvs_list[i]->pos3D().z;

			cloud_mvs_normal[i][0] = _mvs_list[i]->normal().x;
			cloud_mvs_normal[i][1] = _mvs_list[i]->normal().y;
			cloud_mvs_normal[i][2] = _mvs_list[i]->normal().z;

			camVisibility[i] = _mvs_list[i]->cameraList();
			cloud_mvs_confidence[i] = _mvs_list[i]->confidence();

		}

		
		t = clock() - t;
		printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);

		//TO DUMP : clipping planes ZNEAR ZFAR
		std::cout<<"Znear "<<ZNEAR<<" Zfar "<<ZFAR<<std::endl;

		_clipping_planes[0] = ZNEAR;
		_clipping_planes[1] = ZFAR;

		int nbObj = CConfigLib::getObj().size();

		std::cout<<nbObj<<" different size to compute"<<std::endl;

		std::cout<<nbObj<<" different size to compute"<<std::endl;

		//for(int idObj=0;idObj<nbObj;idObj++)
		int idObj=0;
		{

			mesh_point.clear();
			mesh_normal.clear();
			mesh_face.clear();
			mesh_camVisibility.clear();

			//std::vector<glm::vec4> mesh_transfer_color;///color + confidence

			std::vector<glm::vec3> mesh_transfer_normal;///normal
			std::vector<unsigned int> mesh_transfer_id;///normal

			std::cout<<CConfigLib::getObj()[idObj].objPath<<std::endl;

			t = clock() - t;
			printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);

			//load the obj
			loadOBJMesh(CConfigLib::getObj()[idObj].objPath,mesh_point, mesh_normal, mesh_face);

			t = clock() - t;
			printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);

			//mesh_transfer_color.resize(mesh_point.size());
			mesh_transfer_normal.resize(mesh_point.size());
			mesh_transfer_id.resize( mesh_point.size());
			mesh_camVisibility.resize(mesh_point.size());

			pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
			kdtree.setInputCloud (cloud_mvs_xyz);


				//for each point
		pcl::PointXYZ searchPoint;
		for ( unsigned int search_point_id =0;search_point_id <  mesh_point.size() ; search_point_id++){

			searchPoint.x = mesh_point[search_point_id][0];
			searchPoint.y = mesh_point[search_point_id][1];
			searchPoint.z = mesh_point[search_point_id][2];

			// K nearest neighbor search
			int K = 1;

			std::vector<int> pointIdxNKNSearch(K);
			std::vector<float> pointNKNSquaredDistance(K);

			//std::cout << "K nearest neighbor search at (" << searchPoint.x << " " << searchPoint.y << " " << searchPoint.z<< ") with K=" << K << std::endl;

			if ( kdtree.nearestKSearch (searchPoint, K, pointIdxNKNSearch, pointNKNSquaredDistance) > 0 ){
				for (size_t i = 0; i < pointIdxNKNSearch.size (); ++i){

					//mesh_transfer_color[search_point_id][0] =cloud_mvs_color[pointIdxNKNSearch[i]][0];//--r
					//mesh_transfer_color[search_point_id][1] =cloud_mvs_color[pointIdxNKNSearch[i]][1];//--g
					//mesh_transfer_color[search_point_id][2] =cloud_mvs_color[pointIdxNKNSearch[i]][2];//--b
					//mesh_transfer_color[search_point_id][3] =cloud_mvs_confidence[pointIdxNKNSearch[i]];//--confidence

					mesh_transfer_normal[search_point_id][0] =cloud_mvs_normal[pointIdxNKNSearch[i]][0];//--nx
					mesh_transfer_normal[search_point_id][1] =cloud_mvs_normal[pointIdxNKNSearch[i]][1];//--ny
					mesh_transfer_normal[search_point_id][2] =cloud_mvs_normal[pointIdxNKNSearch[i]][2];//--nz

					mesh_transfer_id[search_point_id]=pointIdxNKNSearch[i];//--id du point

					//bonus... not really required anymore
					mesh_camVisibility[search_point_id]=camVisibility[pointIdxNKNSearch[i]];
				}
			}
		}

			t = clock() - t;
			printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);

			//dump obj with transfer data
			//check size
			assert( mesh_transfer_normal.size() == writePositionArray.size());

			//we don t know the effect of approximating some transfer, some issue in the future maybe.
			dataRW::writePositionArray(mesh_point,"mesh_vertices.bin");
			dataRW::writeNormalArray(mesh_transfer_normal,"mesh_transfer_normal.bin");
			dataRW::writeIndexTriangleArray(mesh_face,"mesh_transfer_index.bin");
			dataRW::writePerPointCamerasListArray(mesh_camVisibility,"mesh_transfer_camviz.bin");

			//correspondence between mesh and the point cloud
			dataRW::writeCorrespondenceArray(mesh_transfer_id,"mesh_correspondence_to_point_cloud.bin");

			//store the patch files in binary also...
			dataRW::writePositionArray(cloud_mvs_position,"cloud_vertices.bin");
			dataRW::writeNormalArray(cloud_mvs_normal,"cloud_normal.bin");
			dataRW::writeConfidenceArray(cloud_mvs_confidence,"cloud_confidence.bin");
			dataRW::writePerPointCamerasListArray(camVisibility,"cloud_cam_viz.bin");

			for ( unsigned int cams =0; cams <_cam_vec.size(); cams++){
				std::string filecam="camviz_"+std::to_string(cams)+".bin";
				dataRW::writeUnsignedIntArray(_cam_vec[cams]->getPointCloudVizPointsId(),filecam);
			}
			
			//save clipping planes
			dataRW::writeFloatArray(_clipping_planes,"clipping_planes.bin");
				
		}
		return 0;
	}
	catch(const std::exception & e) {
		std::cout<<e.what()<<std::endl;
	}

#endif

	return 0;
}

