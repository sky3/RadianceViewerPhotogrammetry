#include "stdafx.h"
#include <PhotoFlyXmlManager.h>

std::vector< std::shared_ptr<MVSPoint> > loadPMVSPoints( std::vector< std::shared_ptr<Camera> > & camera_vec) {

	std::vector< std::shared_ptr<MVSPoint> > mvs_vec;

	unsigned int totalCam=PhotoFlyXmlManager::getNbCam();

	std::string pmvs_str;
	std::vector<std::string> pmvs_file_names;
	std::cout<<"File reading"<<std::endl;

	pmvs_str = CConfigLib::getPatch();

	std::cout<<"path"<<pmvs_str<<std::endl;

	std::ifstream pmvs_file(pmvs_str.c_str());

	assert(pmvs_file.is_open());


	std::stringstream buffer_pmvs_file; // variable contenant l'intégralité du fichier 
        // copier l'intégralité du fichier dans le buffer 
     buffer_pmvs_file << pmvs_file.rdbuf(); 
        // nous n'avons plus besoin du fichier ! 
        pmvs_file.close();

	std::string line;
	int numPoints = 0;
	int get_numPoints =0;

	ZNEAR = 1e2f;
	ZFAR  = 1e-2f;

	std::cout << "\n[PMVSPoints] " << pmvs_str.c_str() << std::endl;
	getline(buffer_pmvs_file, line);
	//std::cout << "\n[PMVSPoints] line 1 , here Autodesk patches require to be edit..." << line.c_str() << std::endl;

	if (  sscanf_s(line.c_str(),"PATCHES %d", &get_numPoints) ==1) {
		sscanf_s(line.c_str(),"PATCHES %d", &numPoints) ;
	}
	else{
		buffer_pmvs_file >> numPoints;
	}
	std::cout<<" NB points to process"<< numPoints<<std::endl;

	//--cache 
	mvs_vec.resize(numPoints);

	int maxId = INT_MIN;

	for (int i=0; i<numPoints; i++) {
		int numCam=0, cam;
		std::vector<unsigned int> camList;
		glm::vec4 p, n, t;

		camList.clear();

		do {
			getline(buffer_pmvs_file, line);
			// std::cout<<line.c_str()<<std::endl;
		} while (line.compare("PATCHS"));

		buffer_pmvs_file >> p[0] >> p[1] >> p[2] >> p[3];
		buffer_pmvs_file >> n[0] >> n[1] >> n[2] >> n[3];
		buffer_pmvs_file >> t[0] >> t[1] >> t[2];
		buffer_pmvs_file >> numCam;

		std::shared_ptr< MVSPoint >  tmp_mvs( new MVSPoint(glm::vec3(p[0],p[1],p[2]),(glm::vec3(n[0],n[1],n[2])),t[0]));

		tmp_mvs->_camList=std::vector<bool>(totalCam,false);
		for (int j=0; j<numCam; j++) {
			buffer_pmvs_file >> cam;
			cam = PhotoFlyXmlManager::getIdBundleToCamera(cam);
			if(cam>maxId)
				maxId=cam;	
			camList.push_back(cam);
			tmp_mvs->addCamera(cam);
			tmp_mvs->_camList[cam]=true;
		}
		getline(buffer_pmvs_file, line); // one integer
		getline(buffer_pmvs_file, line); // some images ID's
		getline(buffer_pmvs_file, line); // blank line

		assert(camList.size() == unsigned int(numCam));

		for (unsigned int j=0; j<camList.size() && t[0]>MIN_MVS_PHOTOCONSISTENCY; j++) {

			int k = camList[j];

			if (camera_vec[k]->active()) {

					//check demi plane... to fix vix6
				glm::vec3 dir_mvs_cam = glm::normalize( tmp_mvs->pos3D() - camera_vec[k]->pos());
				float half_plane = glm::dot ( dir_mvs_cam, camera_vec[k]->dir());

				if ( half_plane >0.0f){

					glm::vec2 p2d = camera_vec[k]->project(tmp_mvs->pos3D());

					glm::uvec2 pixel_incam = glm::uvec2(p2d[0],p2d[1]);

					camera_vec[k]->addMVSPnt(i /*id point cloud */,tmp_mvs->pos3D(),tmp_mvs->normal(), tmp_mvs->confidence(), pixel_incam ); 				
								
					glm::vec3 vec = tmp_mvs->pos3D() - camera_vec[k]->pos();                 /* select the z range */
					float z = glm::dot( glm::normalize(camera_vec[k]->dir()), vec);

					ZNEAR = std::min(ZNEAR, z);
					ZFAR  = std::max(ZFAR, z);
				}
				//tmp_mvs->_camList.push_back()
			}
			//tmp_mvs->addCamera(0);
		}

		//push the MVS point to the stack
		mvs_vec[i] = tmp_mvs;
	}

	//Znear and zfar are approximate now
	std::cout << "[PMVSPoints] " << numPoints << " points" << std::endl;
	std::cout << "[DepthRange] " << ZNEAR << " to " << ZFAR << std::endl;
	std::cout << "Max id " << maxId << std::endl;

	return mvs_vec;
}
