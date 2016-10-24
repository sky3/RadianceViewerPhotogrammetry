#ifndef _MVS_H_
#define _MVS_H_

#include "common.h"
#include "Camera.h"


#define MIN_MVS_PHOTOCONSISTENCY 0.0001


class MVSPoint {
private:
	glm::vec3 _position;
	glm::vec3 _normal;
	glm::vec3 _color;

	float _conf;

	std::vector< unsigned int >  _campos2D;
	bool _active_status;

	char* camListBinary;
	int binarySize;

	void computeBinaryCamList();

public:
	void clearCamList(void);
	std::vector<bool>  _camList;
	MVSPoint(glm::vec3 p, glm::vec3 n, float c);
	glm::vec3   pos3D     (void) const  ;
	glm::vec3   normal     (void) const  ;
	glm::vec3   color     (void) const;
	void   setcolor (glm::vec3 c) ;

	float confidence(void) const;

	bool getStatus(void);

	void disable(void);

	void addCamera(unsigned int i);

	std::vector< unsigned int > cameraList(void);

	std::string serialize();
};

#endif // _MVS_H_
