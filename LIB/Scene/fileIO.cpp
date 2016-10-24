#include "stdafx.h"

#include "NN_header.h"

void loadOBJMesh(std::string path_to_obj_mesh, std::vector<glm::vec3> & mesh_point, 
				 std::vector<glm::vec3> & mesh_normal, std::vector<glm::uvec3> & mesh_face){

	std::cout << "Load Geometry" << std::endl;
	
	/*
	
	extern std::vector<glm::vec3> mesh_point;
	extern std::vector<glm::vec3> mesh_normal;
	extern std::vector<glm::uvec3> mesh_face;

	std::vector<glm::vec3> mesh_point;
	std::vector<glm::vec3> mesh_normal;
	std::vector<glm::uvec3> mesh_face;

	*/

	//--read obj file
	std::ifstream obj_file;
	obj_file.open(path_to_obj_mesh.c_str(), std::ios::in);

	std::string line;

	assert(obj_file.is_open());

	std::string test;
	std::string comment = "#";

	unsigned int lign_f = 0;
	while (getline(obj_file, line)) 
	{
		lign_f++;
		if(line.size()>0)
		{
			test = line[0];
			if (test.compare(comment) != 0) 
			{

				//vertices info cases
				if (line[0] == 'v') {
					if (line[1] == ' ') { //Vertex

						char x[255], y[255], z[255];
						std::sscanf(line.c_str(), "v %s %s %s", x, y, z);
						mesh_point.push_back(glm::vec3(std::strtod(x, NULL), std::strtod(y, NULL), std::strtod(z, NULL)));
					}

					if (line[1] == 'n') { //Normal

						char x[255], y[255], z[255];
						std::sscanf(line.c_str(), "vn %s %s %s", x, y, z);
						mesh_normal.push_back(glm::vec3(std::strtod(x, NULL), std::strtod(y, NULL), std::strtod(z, NULL)));
					}

					//we don t care about tc ... they don t exist :P

				}
				//faces
				if (line[0] == 'f') 
				{
					//OBJ INDICES START AT 1...

					int x,y,z, xn,yn,zn ;

					if (std::sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &x, &xn, &y, &yn, &z, &zn)== 6 ) {
						mesh_face.push_back(glm::uvec3((unsigned int)x -1, (unsigned int)y -1,(unsigned int)z -1));
					}
					else if (std::sscanf(line.c_str(), "f %d %d %d", &x, &y, &z)  == 3 ) {
							mesh_face.push_back(glm::uvec3( (unsigned int)x -1,(unsigned int) y -1,(unsigned int)z -1));
					}
					else if (std::sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &x, &xn, &y, &yn, &z, &zn)== 6 ) {
						mesh_face.push_back(glm::uvec3((unsigned int)x -1, (unsigned int)y -1,(unsigned int)z -1));
					}
	
			/*

					char x[255], y[255], z[255];
					char xn[255], yn[255], zn[255];
					std::sscanf(line.c_str(), "f %s %s %s %s %s %s", x, xn, y, yn, z, zn);
					mesh_face.push_back(
						glm::uvec3((int) std::strtod(x, NULL), (int) std::strtod(y, NULL),
						(int) std::strtod(z, NULL)));
						*/
				}

			}
		}
	}

	std::cout << " size vector" << mesh_point.size() << std::endl;
	std::cout << " size normal vec" << mesh_normal.size() << std::endl;
	std::cout << " faces vector" << mesh_face.size() << std::endl;


		// resize the cloud of point structure to store MVS points
	cloud_mesh_xyz.reset(new pcl::PointCloud<pcl::PointXYZ>);

	// Generate pointcloud data
	cloud_mesh_xyz->width =  mesh_point.size();
	cloud_mesh_xyz->height = 1;
	cloud_mesh_xyz->points.resize (cloud_mesh_xyz->width * cloud_mesh_xyz->height);

	for (size_t i = 0; i < cloud_mesh_xyz->points.size (); ++i){

		cloud_mesh_xyz->points[i].x = mesh_point[i][0] ;
		cloud_mesh_xyz->points[i].y = mesh_point[i][1] ;
		cloud_mesh_xyz->points[i].z = mesh_point[i][2] ;
	}
}