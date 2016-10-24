#ifndef _NN_HEADER_H
#define _NN_HEADER_H

//NEAREST NEIGHBOUR
#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>

extern pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_mvs_xyz;
extern std::vector<glm::vec3> cloud_mvs_normal;
extern std::vector<glm::vec3> cloud_mvs_position;
extern std::vector<glm::vec3> cloud_mvs_color;
extern std::vector<float> cloud_mvs_confidence;
extern std::vector<std::vector<unsigned int> > camVisibility;

extern pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_mesh_xyz;
/*
extern std::vector<glm::vec3> mesh_point;
extern std::vector<glm::vec3> mesh_normal;
extern std::vector<glm::uvec3> mesh_face;
*/

extern void loadOBJMesh(std::string );

#endif 