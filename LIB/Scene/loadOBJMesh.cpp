#include "loadOBJMesh.h"

void loadOBJMesh(std::string path_to_obj_mesh, std::vector<glm::vec3> & mesh_point, 
				 std::vector<glm::vec3> & mesh_normal, std::vector<glm::uvec3> & mesh_face){

	std::cout << "Load Geometry" << std::endl;
	
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

}