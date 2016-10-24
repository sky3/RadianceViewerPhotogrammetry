#include "stdafx.h"
#include "DataRW.h"
void dataRW::writeGlmVec3Array(std::vector<glm::vec3> &array_vec, std::string file_out){

	std::ofstream file_to_write (file_out.c_str(), std::ios::out | std::ios::binary);

	int nb_element = array_vec.size();
	file_to_write.write((char *)&nb_element, sizeof(int));
	file_to_write.write((char *)&array_vec[0], sizeof(glm::vec3)*nb_element);

	file_to_write.close();
}

std::vector<glm::vec3> dataRW::readGlmVec3Array( std::string file_in){

	std::vector<glm::vec3> array_vec;

	std::ifstream file_to_read (file_in.c_str(), std::ios::out | std::ios::binary);

	if ( file_to_read.is_open()){
		unsigned int nb_element=0;
		file_to_read.read((char *)&nb_element, sizeof(unsigned int));

		array_vec.resize(nb_element);

		//entree.read((char *)_pos.get(), sizeof(float)*nb_element);
		for ( unsigned unsigned int i =0 ; i < nb_element; i++){
			file_to_read.read((char *)&array_vec[0], sizeof(glm::vec3)*nb_element);
		}
		file_to_read.close();
	}else {
		array_vec.clear();
	}
	return array_vec;
}

void dataRW::writeGlmUVec3Array(std::vector<glm::uvec3> &array_vec, std::string file_out){
	std::ofstream file_to_write (file_out.c_str(), std::ios::out | std::ios::binary);

	unsigned int nb_element = array_vec.size();
	file_to_write.write((char *)&nb_element, sizeof(unsigned int));
	file_to_write.write((char *)&array_vec[0], sizeof(glm::uvec3)*nb_element);

	file_to_write.close();
}

std::vector<glm::uvec3> dataRW::readGlmUVec3Array( std::string file_in){
	std::vector<glm::uvec3> array_vec;

	std::ifstream file_to_read (file_in.c_str(), std::ios::out | std::ios::binary);

	if ( file_to_read.is_open()){
		unsigned int nb_element=0;
		file_to_read.read((char *)&nb_element, sizeof(unsigned int));

		array_vec.resize(nb_element);

		//entree.read((char *)_pos.get(), sizeof(float)*nb_element);
		for ( unsigned  int i =0 ; i < nb_element; i++){
			file_to_read.read((char *)&array_vec[0], sizeof(glm::uvec3)*nb_element);
		}
		file_to_read.close();
		return array_vec;
	}else {
		array_vec.clear();
	}
}



void dataRW::writeFloatArray(std::vector<float> &array_vec, std::string file_out){

	std::ofstream file_to_write (file_out.c_str(), std::ios::out | std::ios::binary);

	unsigned int nb_element = array_vec.size();
	file_to_write.write((char *)&nb_element, sizeof(unsigned int));
	file_to_write.write((char *)&array_vec[0], sizeof(float)*nb_element);

	file_to_write.close();
}

std::vector<float> dataRW::readFloatArray(std::string file_in){
	std::vector<float> array_vec;

	std::ifstream file_to_read (file_in.c_str(), std::ios::out | std::ios::binary);

	if ( file_to_read.is_open()){
		unsigned int nb_element=0;
		file_to_read.read((char *)&nb_element, sizeof(unsigned int));

		array_vec.resize(nb_element);

		//entree.read((char *)_pos.get(), sizeof(float)*nb_element);
		for ( unsigned  int i =0 ; i < nb_element; i++){
			file_to_read.read((char *)&array_vec[0], sizeof(float)*nb_element);
		}
		file_to_read.close();
		return array_vec;
	}else {
		array_vec.clear();
	}

}

void dataRW::writeUnsignedIntArray(std::vector<unsigned int> &array_vec, std::string file_out){
	std::ofstream file_to_write (file_out.c_str(), std::ios::out | std::ios::binary);

	unsigned int nb_element = array_vec.size();
	file_to_write.write((char *)&nb_element, sizeof(unsigned int));
	file_to_write.write((char *)&array_vec[0], sizeof(unsigned int)*nb_element);

	file_to_write.close();
}
std::vector<unsigned int> dataRW::readUnsignedIntArray(std::string file_in){

	std::vector<unsigned int> array_vec;

	std::ifstream file_to_read (file_in.c_str(), std::ios::out | std::ios::binary);

	if ( file_to_read.is_open()){
		unsigned int nb_element=0;
		file_to_read.read((char *)&nb_element, sizeof(unsigned int));

		array_vec.resize(nb_element);

		//entree.read((char *)_pos.get(), sizeof(float)*nb_element);
		for ( unsigned  int i =0 ; i < nb_element; i++){
			file_to_read.read((char *)&array_vec[0], sizeof(unsigned int)*nb_element);
		}
		file_to_read.close();
	
	}else {
		array_vec.clear();
	}
		return array_vec;
}

void dataRW::writeListPerIndex(std::vector<std::vector<unsigned int>> &arr,std::string file_out ){

	std::ofstream file_to_write (file_out.c_str(), std::ios::out | std::ios::binary);

	unsigned int nb_element = arr.size();
	unsigned int nb_element_line =0;

	file_to_write.write((char *)&nb_element, sizeof(unsigned int));


	for ( unsigned int k=0; k < nb_element; k++){

		nb_element_line = arr[k].size();
		file_to_write.write((char *)&nb_element_line, sizeof(unsigned int));

		if ( nb_element_line > 0 ){
			for( unsigned int kc = 0; kc < nb_element_line;kc++){
				file_to_write.write((char *)&arr[k][kc], sizeof(unsigned int));
			}
		}
		else {
			//unsigned int no_data =11111111;
			//nothing
			//file_to_write.write((char *)&no_data, sizeof(unsigned int));
		}

	}

	file_to_write.close();
}


std::vector<std::vector<unsigned int>> dataRW::readListPerIndex(std::string file_in ){

	std::vector<std::vector<unsigned int>> array_vec;

	std::ifstream file_to_read (file_in.c_str(), std::ios::out | std::ios::binary);

	if ( file_to_read.is_open()){

		unsigned int nb_element=0;
		file_to_read.read((char *)&nb_element, sizeof(unsigned int));

		array_vec.resize(nb_element);

		unsigned int line_nb_element=0;
		unsigned int check_error =0;
		for ( unsigned int k=0; k < nb_element; k++){
			//line N elements ?
			file_to_read.read((char *)&line_nb_element, sizeof(unsigned int));

			if (line_nb_element!=0) {
				array_vec[k].resize( line_nb_element);
				
				for ( unsigned int l =0; l< line_nb_element; l++){
					file_to_read.read((char *)&array_vec[k][l], sizeof(unsigned int));
				}

				//file_to_read.read((char *)&array_vec[k][0], sizeof(unsigned int)*line_nb_element);
			}
		}
		file_to_read.close();

	}else {
		array_vec.clear();

	}
	return array_vec;
}


void dataRW::writeListVec3PerIndex(std::vector<std::vector<glm::vec3>> &arr,std::string file_out ){

	std::ofstream file_to_write (file_out.c_str(), std::ios::out | std::ios::binary);

	unsigned int nb_element = arr.size();
	unsigned int nb_element_line =0;

	file_to_write.write((char *)&nb_element, sizeof(unsigned int));


	for ( unsigned int k=0; k < nb_element; k++){

		nb_element_line = arr[k].size();
		file_to_write.write((char *)&nb_element_line, sizeof(unsigned int));

		if ( nb_element_line > 0 ){
			file_to_write.write((char *)&arr[k][0], sizeof(glm::vec3)*nb_element_line);
		}
		else {

			//nothing
			///file_to_write.write((char *)&dataRW::no_data, sizeof(int));
		}

	}

	file_to_write.close();
}
std::vector<std::vector<glm::vec3>> dataRW::readListVec3PerIndex(std::string file_in ){

	std::vector<std::vector<glm::vec3>> array_vec;

	std::ifstream file_to_read (file_in.c_str(), std::ios::out | std::ios::binary);

	if ( file_to_read.is_open()){

		int nb_element=0;
		file_to_read.read((char *)&nb_element, sizeof(int));

		array_vec.resize(nb_element);

		int line_nb_element=0;
		int check_error =0;
		for ( int k=0; k < nb_element; k++){
			//line N elements ?
			file_to_read.read((char *)&line_nb_element, sizeof(int));

			if (line_nb_element!=0) {
				array_vec[k].resize( line_nb_element);
				file_to_read.read((char *)&array_vec[k][0], sizeof(glm::vec3)*line_nb_element);
			}
		}
		file_to_read.close();
		return array_vec;
	}else {
		array_vec.clear();

	}
}

void  dataRW::writeIndexTriangleArray(std::vector<glm::uvec3> &index, std::string file_out){
	dataRW::writeGlmUVec3Array(index,file_out);
}

std::vector<glm::uvec3>  dataRW::readIndexTriangleArray(std::string file_in){
	return dataRW::readGlmUVec3Array(file_in);
}

void dataRW::writePositionArray(std::vector<glm::vec3> &position, std::string file_out){
	dataRW::writeGlmVec3Array(position,file_out);
}

std::vector<glm::vec3> dataRW::readPositionArray(std::string file_in){
	return dataRW::readGlmVec3Array(file_in);
}

void dataRW::writeNormalArray(std::vector<glm::vec3> &position, std::string file_out){
	dataRW::writeGlmVec3Array(position,file_out);
}

std::vector<glm::vec3> dataRW::readNormalArray(std::string file_in){
	return dataRW::readGlmVec3Array(file_in);
}

void dataRW::writeConfidenceArray(std::vector<float> &confidence,std::string file_out){
	dataRW::writeFloatArray(confidence,file_out);
}
std::vector<float>  dataRW::readConfidenceArray(std::string file_in){
	return dataRW::readFloatArray(file_in);
}

void dataRW::writeCorrespondenceArray(std::vector<unsigned int> &confidence,std::string file_out){
	dataRW::writeUnsignedIntArray(confidence,file_out);
}

std::vector<unsigned int>  dataRW::readCorrespondenceArray(std::string file_in){
	return dataRW::readUnsignedIntArray(file_in);
}

void dataRW::writePerPointCamerasListArray(std::vector<std::vector<unsigned int>> &arr,std::string file_out){
	dataRW::writeListPerIndex(arr, file_out );
}
std::vector<std::vector<unsigned int>> dataRW::readPerPointCamerasListArray(std::string file_in){
	return dataRW::readListPerIndex(file_in);
}

//per radiance per points
std::vector<std::vector<std::pair<unsigned int, glm::vec3>>> dataRW::readRadiancePerPoints(std::string file_in){

	std::vector<std::vector<std::pair<unsigned int, glm::vec3>>> _rad_per_points;

	std::ifstream file_to_read (file_in.c_str(), std::ios::out | std::ios::binary);

	if ( file_to_read.is_open()){

		unsigned int nb_element=0;
		file_to_read.read((char *)&nb_element, sizeof(unsigned int));

		_rad_per_points.resize(nb_element);

		unsigned int line_nb_element=0;
		unsigned int check_error =0;
		for ( unsigned int k=0; k < nb_element; k++){
			//line N elements ?
			file_to_read.read((char *)&line_nb_element, sizeof(unsigned int));

			if (line_nb_element!=0) {
				_rad_per_points[k].resize( line_nb_element);

				for ( unsigned int l =0; l< line_nb_element; l++){

					unsigned int cam_id;
					glm::vec3 radiance;

					file_to_read.read((char *)&cam_id, sizeof(unsigned int));
					file_to_read.read((char *)&radiance, sizeof(glm::vec3));
					_rad_per_points[k][l].first= cam_id;
					_rad_per_points[k][l].second= radiance;
				}

			}
		}
		file_to_read.close();

	}
	return _rad_per_points;


}

void dataRW::writeRadiancePerPoints(std::vector<std::vector<std::pair<unsigned int, glm::vec3>>>  &arr, std::string file_out){

	//std::vector<std::vector<std::pair<unsigned int, glm::vec3>>> 
	//vector of points id
	//------/vector of pair of camera id + radiance
	//-------------/pair camera id + rgb 

	std::ofstream file_to_write (file_out.c_str(), std::ios::out | std::ios::binary);

	unsigned int nb_element = arr.size();
	unsigned int nb_element_line =0;

	file_to_write.write((char *)&nb_element, sizeof(unsigned int));//nb elements

	for ( unsigned int k=0; k < nb_element; k++){ //for all points

		nb_element_line = arr[k].size();
		file_to_write.write((char *)&nb_element_line, sizeof(unsigned int));//nb elements per line

		if ( nb_element_line > 0 ){
			//for all pair of radiance

			for ( unsigned int l =0; l< nb_element_line; l++){

				//	arr[k][l].first camera id
				//	arr[k][l].second radiance
				file_to_write.write((char *)&arr[k][l].first, sizeof(unsigned int));
				file_to_write.write((char *)&arr[k][l].second, sizeof(glm::vec3));
			}
		}
		else {

			//nothing
			///file_to_write.write((char *)&dataRW::no_data, sizeof(int));
		}

	}

	file_to_write.close();

}
