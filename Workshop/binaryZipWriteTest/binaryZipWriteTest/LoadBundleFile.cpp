#include "stdafx.h"
#include <PhotoFlyXmlManager.h>

////////////////////////////////////new//////////////////////////////////////////////////////////////////
std::vector< std::shared_ptr<Camera> > loadBundleFile(bool display,bool loadImage) {

	CConfigLib::_assert();

	std::vector< std::shared_ptr<Camera> >   camera_vec;//create the storage

	std::ifstream bundle_file;
	std::string bundle_file_str;
	std::string image_file_str;
	std::string line;

	int numCamera = 0;
	int numPoints = 0;

	/*-----------------------------------------------------------------------------
	*  Parse bundle.out file for camera calibration parameters
	*-----------------------------------------------------------------------------*/
	bundle_file_str = CConfigLib::getBundle();
	bundle_file.open(bundle_file_str.c_str());
	assert(bundle_file.is_open());

	std::cout << "[BundleFile] loading from " << bundle_file_str.c_str() << std::endl;
	getline(bundle_file, line);
	bundle_file >> numCamera >> numPoints;

	if(display)
		std::cout << "[BundleFile] loading from " << bundle_file_str.c_str() << std::endl;
	//getline(bundle_file, line);
	//bundle_file >> numCamera >> numPoints;

	/*-----------------------------------------------------------------------------
	*  Load the list of active image id's from active_images.txt
	*-----------------------------------------------------------------------------*/
	std::vector<bool> use_flag;
	std::vector<unsigned int> use_images = CConfigLib::getImagesSet();

	if(use_images.size()==0)
		use_flag.resize(numCamera,true);
	else
		use_flag.resize(numCamera,false);

	for(int i=0;i<use_images.size();i++)
		if(use_images[i]<=numCamera && PhotoFlyXmlManager::getCamInf(use_images[i]).used )
			use_flag[use_images[i]]=true;


	camera_vec.resize(numCamera);

	float m[15];

	for (int i=0; i<numCamera; i++) {

		if(i==0 || PhotoFlyXmlManager::getCamInf(i-1).used)
		{
			bundle_file >> m[0] >> m[1] >> m[2];
			bundle_file >> m[3] >> m[4] >> m[5];
			bundle_file >> m[6] >> m[7] >> m[8];
			bundle_file >> m[9] >> m[10]>> m[11]; 
			bundle_file >> m[12]>> m[13]>> m[14];
		}

		/*
		char buffer[128];
		//sprintf_s (buffer, "HDR_undistorted/%08d.tiff", i);
		sprintf_s(buffer, "%08d.jpg", i);
		image_file_str = image_dir + buffer;
		*/
		char buffer[128];
		sprintf_s(buffer, "%08d", i);

		std::ostringstream oss;
		oss<<CConfigLib::getJpgPath(true)<<buffer<<"."<<CConfigLib::getJpgExt();
		image_file_str=oss.str();


		if (use_flag[i]) {

			if(PhotoFlyXmlManager::getCamInf(i).used)
			{
				//std::shared_ptr< cimg_library::CImg<float> >  tmp_c( new cimg_library::CImg<float> (image_file_str.c_str()));


				std::shared_ptr< Camera >  tmp_cam( new Camera(i,m,PhotoFlyXmlManager::getCamInf(i).width,PhotoFlyXmlManager::getCamInf(i).height,use_flag[i]));

				tmp_cam->setImgFilename(image_file_str);
				camera_vec[i] = tmp_cam;

				if(display)
				{
					std::cout << "[BundleFile] " << image_file_str.c_str() << " " << tmp_cam->w()<<" "<<tmp_cam->h()<<" ";
					std::cout << (use_flag[i] ?  " active ": " ---- ") << std::endl;
				}
			}
			else
			{
				std::shared_ptr< Camera >  tmp_cam( new Camera(i,m,PhotoFlyXmlManager::getCamInf(i).width,PhotoFlyXmlManager::getCamInf(i).height,use_flag[i]));
				tmp_cam->setImgFilename(image_file_str);
				camera_vec[i] = tmp_cam;
			
				if(!PhotoFlyXmlManager::getCamInf(i).used)
					tmp_cam->setActive(false);

				if(display)
				{
					std::cout << "[BundleFile] " << image_file_str.c_str() << " " << PhotoFlyXmlManager::getCamInf(i).width<<" "<<PhotoFlyXmlManager::getCamInf(i).height<<" ";
					std::cout << (use_flag[i] ?  " active ": " ---- ") << std::endl;
				}
			}

		} else {
			std::shared_ptr< Camera >  tmp_cam( new Camera(i,m,0,0,use_flag[i]));
			camera_vec[i] = tmp_cam;
		}

	}

	if(display)
		std::cout << "[BundleFile] " << camera_vec.size() << " / "  << numCamera << " images" << std::endl;
	return camera_vec;
}


std::vector< std::shared_ptr<Camera> > loadBundleFileHDR(bool useAll,bool loadImage) {

	std::vector< std::shared_ptr<Camera> >   camera_vec;//create the storage

	std::ifstream bundle_file;
	std::string bundle_file_str;
	std::string image_file_str;
	std::string line;

	int numCamera = 0;
	int numPoints = 0;

	/*-----------------------------------------------------------------------------
	*  Parse bundle.out file for camera calibration parameters
	*-----------------------------------------------------------------------------*/
	bundle_file_str = CConfigLib::getBundle();
	bundle_file.open(bundle_file_str.c_str());
	assert(bundle_file.is_open());

	std::cout << "[BundleFile] loading from " << bundle_file_str.c_str() << std::endl;
	getline(bundle_file, line);
	bundle_file >> numCamera >> numPoints;

	/*-----------------------------------------------------------------------------
	*  Load the list of active image id's from active_images.txt
	*-----------------------------------------------------------------------------*/
	std::vector<bool> use_flag;
	std::vector<unsigned int> use_images = CConfigLib::getImagesSet();
	numCamera=PhotoFlyXmlManager::getCamInf().size();

	if(use_images.size()==0 || useAll){
		use_flag.resize(numCamera,true);
	}
	else{
		use_flag.resize(numCamera,false);
	}

	if ( !useAll ){
		for(int i=0;i<use_images.size();i++){
			if(use_images[i]<=numCamera && PhotoFlyXmlManager::getCamInf(use_images[i]).used)
			{
				use_flag[use_images[i]]=true;
			}
		}
	}

	camera_vec.resize(numCamera);

	float m[15];

	for (int i=0; i<numCamera; i++) {

		if(i==0 || PhotoFlyXmlManager::getCamInf(i-1).used)
		{
			bundle_file >> m[0] >> m[1] >> m[2];
			bundle_file >> m[3] >> m[4] >> m[5];
			bundle_file >> m[6] >> m[7] >> m[8];
			bundle_file >> m[9] >> m[10]>> m[11]; 
			bundle_file >> m[12]>> m[13]>> m[14];
		}

		/*
		char buffer[128];
		sprintf_s(buffer, "%08d", i);

		std::ostringstream oss;
		oss<<CConfigLib::getTifPath(true)<<buffer<<"."<<CConfigLib::getTifExt();
		image_file_str=oss.str();
		*/

		image_file_str=CConfigLib::getPngPath(true)+PhotoFlyXmlManager::getCamInf(i).name+".png";


		if (use_flag[i]) {
	
			std::shared_ptr< Camera >  tmp_cam( new Camera(i,m,PhotoFlyXmlManager::getCamInf(i).width,PhotoFlyXmlManager::getCamInf(i).height,use_flag[i]));
			tmp_cam->setImgFilename(image_file_str);
				
			if(!PhotoFlyXmlManager::getCamInf(i).used)
				tmp_cam->setActive(false);

			camera_vec[i] = tmp_cam;			

			std::cout << "[BundleFile] " << image_file_str.c_str() << " " << PhotoFlyXmlManager::getCamInf(i).width<<" "<<PhotoFlyXmlManager::getCamInf(i).height<<" ";
			std::cout << (use_flag[i] ?  " active ": " ---- ") << std::endl;

		} else {
			std::shared_ptr< Camera >  tmp_cam( new Camera(i,m,0,0,use_flag[i]));
			camera_vec[i] = tmp_cam;
		}
	}

	std::cout << "[BundleFile] " << camera_vec.size() << " / "  << numCamera << " images" << std::endl;
	return camera_vec;
}