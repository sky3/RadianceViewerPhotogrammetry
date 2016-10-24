#ifndef LOAD_OBJ_MESH_H_
#define LOAD_OBJ_MESH_H_

#include "common.h"

void loadOBJMesh(std::string path_to_obj_mesh, std::vector<glm::vec3> & mesh_point, 
				 std::vector<glm::vec3> & mesh_normal, std::vector<glm::uvec3> & mesh_face);
				 
#endif