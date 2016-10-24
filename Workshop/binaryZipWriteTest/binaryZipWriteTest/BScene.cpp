include "stdafx.h"
/*#include "BScene.h"

BScene::BScene(std::string _scene_path){

}

BScene::~BScene(){

}


float * BScene::getPositionArrayPtr(){
	return _position;
}

float * BScene::getNormalArrayPtr(){
	return _normal;
}
int * BScene::getMeshTriangleIndexPtr(){

	return _mesh_index;
}

	int getNPoints();


	//Position array
	void writePositionArray(std::string file_out);
	void readPositionArray(std::string file_in);

	//Position array
	void writeNormalArray(std::string file_out);
	void readNormalArray(std::string file_in);

void writePositionArray(float* position_tab,int Nb_elements, std::string file_out ){

	std::ofstream out_bin (file_out.c_str(), std::ios::out | std::ios::binary);
	out_bin.write((char *)&Nb_elements, sizeof(int));
	out_bin.write((char *)position_tab, sizeof(float)*POSITION_FILE_VEC_SIZE*Nb_elements);
	
	out_bin.close();
}

float* readPositionArray(std::string file_in){
	
}
*/