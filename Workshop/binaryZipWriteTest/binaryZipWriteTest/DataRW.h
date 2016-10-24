
#ifndef _DATA_RW_H_
#define _DATA_RW_H_


#include "stdafx.h"

namespace dataRW{

	//generic writing
	void writeGlmVec3Array(std::vector<glm::vec3> &array_vec, std::string file_out);
	std::vector<glm::vec3> readGlmVec3Array( std::string file_in);

	void writeGlmUVec3Array(std::vector<glm::uvec3> &array_vec, std::string file_out);
	std::vector<glm::uvec3> readGlmUVec3Array( std::string file_in);

	void writeFloatArray(std::vector<float> &array_vec, std::string file_out);
	std::vector<float> readFloatArray(std::string file_in);
	void writeUnsignedIntArray(std::vector<unsigned int> &array_vec, std::string file_out);
	std::vector<unsigned int> readUnsignedIntArray(std::string file_in);

	void writeListPerIndex(std::vector<std::vector<unsigned int>> &arr,std::string file_out );
	std::vector<std::vector< unsigned int>> readListPerIndex(std::string file_in );
	
	void writeListVec3PerIndex(std::vector<std::vector<glm::vec3>> &arr,std::string file_out );
	std::vector<std::vector<glm::vec3>> readListVec3PerIndex(std::string file_in );
	
	//faces array
	void writeIndexTriangleArray(std::vector<glm::uvec3> &position, std::string file_out);
	std::vector<glm::uvec3> readIndexTriangleArray(std::string file_in);

	//Position array
	void writePositionArray(std::vector<glm::vec3> &position, std::string file_out);
	std::vector<glm::vec3> readPositionArray(std::string file_in);

	//Normal array
	void writeNormalArray(std::vector<glm::vec3> &normal,std::string file_out);
	std::vector<glm::vec3> readNormalArray(std::string file_in);

	//correspondence_id
	void writeCorrespondenceArray(std::vector<unsigned int> &confidence,std::string file_out);
	std::vector<unsigned int> readCorrespondenceArray(std::string file_in);

	//Confidence array
	void writeConfidenceArray(std::vector<float> &confidence,std::string file_out);
	std::vector<float> readConfidenceArray(std::string file_in);

	//per point camera list
	void writePerPointCamerasListArray(std::vector<std::vector<unsigned int>> &arr,std::string file_out);
	std::vector<std::vector<unsigned int>> readPerPointCamerasListArray(std::string file_in);

	//per camera points list
	void writeCameraPointsListArray(std::vector<std::vector<unsigned int>> &arr,std::string file_out);
	std::vector<std::vector<unsigned int>> readCameraPointsListArray(std::string file_in);

	//per radiance per points
	std::vector<std::vector<std::pair<unsigned int, glm::vec3>>> readRadiancePerPoints(std::string file_in);
	void writeRadiancePerPoints(std::vector<std::vector<std::pair<unsigned int, glm::vec3>>>  &arr, std::string file_out);


}
#endif