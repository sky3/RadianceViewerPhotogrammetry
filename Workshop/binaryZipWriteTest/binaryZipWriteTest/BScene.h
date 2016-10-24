/*
#ifndef _BSCENE_H_
#define _BSCENE_H_

#include "stdafx.h"

#include <fstream>
#include <iostream>

#include <vector>
#include <memory>

#include <string>

//KEEP TRACK OF DATA SIZE HERE
#define POSITION_FILE_VEC_SIZE 3
#define NORMAL_FILE_VEC_SIZE 3

/
Class to represent the junk we need to deal with...
ALL DATA INDEX START AT 0 !!!!

WE ASSUME OBJ ONLY CONTAINS TRIANGLE 

class BScene {

public :
	BScene(std::string _scene_path);
	~BScene();
	
	//getter
	float * getPositionArrayPtr();
	float * getNormalArrayPtr();
	int * getMeshTriangleIndexPtr();

	int getNPoints();

	
	//Write and read all binary files
	
	//Position array
	void writePositionArray(std::string file_out);
	void readPositionArray(std::string file_in);

	//Position array
	void writeNormalArray(std::string file_out);
	void readNormalArray(std::string file_in);


private :

	int _nb_points;
	float * _position;
	float * _normal;
	float * _mesh_index; 
	
};

#endif
*/