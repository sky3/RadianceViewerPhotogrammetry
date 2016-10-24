
#include "MVSPoint.h"


MVSPoint::MVSPoint(glm::vec3 p, glm::vec3 n, float c) : _position(p), _normal(n), _conf(c) {_active_status =true;}
glm::vec3   MVSPoint::pos3D     (void) const  { return _position;    }
glm::vec3   MVSPoint::normal     (void) const  { return _normal;    }
glm::vec3   MVSPoint::color     (void) const  { return _color;    }
void   MVSPoint::setcolor (glm::vec3 c)   { _color =c; }

float MVSPoint::confidence(void) const  { return _conf; }

bool MVSPoint::getStatus(void){ return _active_status;}

void MVSPoint::disable(void){ _active_status =false;}

void MVSPoint::addCamera(unsigned int i) {
	_campos2D.push_back(i);
}

 void MVSPoint::clearCamList(void){
	 _campos2D.clear();
 }
std::vector< unsigned int > MVSPoint::cameraList(void) {
	return _campos2D;
}

void MVSPoint::computeBinaryCamList()
{
	int nbByte=ceil(_camList.size()/8.0);
	binarySize=nbByte+1;
	camListBinary =  new char[binarySize];

	int camId = 0;
	for(int byteId=0;byteId<nbByte;byteId++)
	{
		camListBinary[byteId]=0x0;
		for(int bi=0;bi<8;bi++)
		{
			char mask = 0x0;
			if(_camList[camId])
			{
				mask=0x1;
				mask=mask<<(8-bi);
				camListBinary[byteId]=camListBinary[byteId]|mask;
			}
		}
	}

	camListBinary[nbByte]='\0';
}


std::string MVSPoint::serialize()
{
	std::ostringstream flux (std::ostringstream::ate); 


	flux<<pos3D()[0]<<" "<< pos3D()[1] <<" "<< pos3D()[2];
	flux<< " "<<normal()[0]<< " "<<normal()[1]<< " "<<normal()[2];
	flux<< " "<<color()[0]<< " "<<color()[1]<< " "<<color()[2];
	flux<< " "<<confidence()<<" "<<_campos2D.size();
	for(int i=0;i<_campos2D.size();i++)
		flux<<" "<<_campos2D[i];
	

	return flux.str() ;
}

