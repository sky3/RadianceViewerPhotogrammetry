// preScene.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"


#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>

pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_mvs_xyz;
std::vector<glm::vec3> cloud_mvs_normal;
std::vector<glm::vec3> cloud_mvs_position;
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

float ZNEAR =100000.0f;
float ZFAR = 0.000001f;

int main(int argc, char** argv){

	try {

		CConfigLib::option(argc,argv);
		CConfigLib::_assert();
		
		std::string  path_radiance_data = CConfigLib::getOutRadianceFolder();

		std::vector<unsigned int> _xml_image_vec_id =CConfigLib::getImagesSet();

		std::vector< std::shared_ptr<Camera> > _cam_vec;
		std::vector<float> _clipping_planes;
		_clipping_planes.resize(2);
		
		clock_t t;
		
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

		unsigned int numPoints = (unsigned int)_mvs_list.size();

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

		unsigned int nbObj = (unsigned int) CConfigLib::getObj().size();

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

			//we don t know the effect of approximating some transfer, some issue in the future maybe.
			dataRW::writePositionArray(mesh_point, std::string( path_radiance_data+"mesh_vertices.bin"));
			dataRW::writeNormalArray(mesh_transfer_normal,std::string( path_radiance_data+"mesh_transfer_normal.bin"));
			dataRW::writeIndexTriangleArray(mesh_face,std::string( path_radiance_data+"mesh_transfer_index.bin"));
			dataRW::writePerPointCamerasListArray(mesh_camVisibility,std::string( path_radiance_data+"mesh_transfer_camviz.bin"));

			//correspondence between mesh and the point cloud
			dataRW::writeCorrespondenceArray(mesh_transfer_id,std::string( path_radiance_data+"mesh_correspondence_to_point_cloud.bin"));

			//store the patch files in binary also...
			dataRW::writePositionArray(cloud_mvs_position,std::string( path_radiance_data+"cloud_vertices.bin"));
			dataRW::writeNormalArray(cloud_mvs_normal,std::string( path_radiance_data+"cloud_normal.bin"));
			dataRW::writeConfidenceArray(cloud_mvs_confidence,std::string( path_radiance_data+"cloud_confidence.bin"));
			dataRW::writePerPointCamerasListArray(camVisibility,std::string( path_radiance_data+"cloud_cam_viz.bin"));

			//save clipping planes
			dataRW::writeFloatArray(_clipping_planes,std::string( path_radiance_data+"clipping_planes.bin"));

			/*
			for ( unsigned int cams =0; cams <_xml_image_vec_id.size(); cams++){
				std::string filecam=std::string( path_radiance_data+"camviz_"+std::to_string(_xml_image_vec_id[cams])+".bin");
				
				if ( _cam_vec[_xml_image_vec_id[cams]]->getPointCloudVizPointsId().size() >0 ){
					dataRW::writeUnsignedIntArray(_cam_vec[_xml_image_vec_id[cams]]->getPointCloudVizPointsId(),filecam);
				}
			}
			*/

			

		}

	}
	catch(const std::exception & e) {
		std::cout<<e.what()<<std::endl;
	}

	return 0;
}