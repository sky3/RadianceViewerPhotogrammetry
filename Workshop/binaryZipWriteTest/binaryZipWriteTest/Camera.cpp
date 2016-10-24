#include "stdafx.h"

Camera::Camera(unsigned int id, float m[15], unsigned int w, unsigned int h, bool a) {
	
	_id = id;
	_w  = w;
	_h  = h;
	_active = a;

	_f = 2.0 * atan( 0.5* _h / m[0]);

	for(int i=0; i<9; i++) r[i] = m[3+i];
	for(int i=0; i<3; i++) t[i] = m[12+i];

	_pos[0] = -(r[0]*t[0] + r[3]*t[1] + r[6]*t[2]);
	_pos[1] = -(r[1]*t[0] + r[4]*t[1] + r[7]*t[2]);
	_pos[2] = -(r[2]*t[0] + r[5]*t[1] + r[8]*t[2]);

	_dir  = glm::normalize(glm::vec3(-r[6],-r[7],-r[8]));
	_side = glm::normalize(glm::vec3( r[0], r[1], r[2]));
	_up   = glm::normalize(glm::cross(_side, _dir));

	_mvsPnts.clear();
	
	_focal_pixel = m[0];
	_ppx =0.5;//not a bad approximation
	_ppy =0.5;//not a bad approximation

}

Camera::Camera(unsigned int id, float m[15], unsigned int w, unsigned int h, bool a, float ppx, float ppy) {
	
	_id = id;
	_w  = w;
	_h  = h;
	_active = a;

	_f = 2.0 * atan( 0.5* _h / m[0]);

	for(int i=0; i<9; i++) r[i] = m[3+i];
	for(int i=0; i<3; i++) t[i] = m[12+i];

	_pos[0] = -(r[0]*t[0] + r[3]*t[1] + r[6]*t[2]);
	_pos[1] = -(r[1]*t[0] + r[4]*t[1] + r[7]*t[2]);
	_pos[2] = -(r[2]*t[0] + r[5]*t[1] + r[8]*t[2]);

	_dir  = glm::normalize(glm::vec3(-r[6],-r[7],-r[8]));
	_side = glm::normalize(glm::vec3( r[0], r[1], r[2]));
	_up   = glm::normalize(glm::cross(_side, _dir));

	_mvsPnts.clear();
	
	_focal_pixel = m[0];
	_ppx =ppx;
	_ppy =ppy;

	fixProjection();
}
Camera::Camera(unsigned int id, unsigned int w, unsigned int h, bool a) 
{
	_id = id;
	_w  = w;
	_h  = h;
	_active = a;

	_mvsPnts.clear();
}
void Camera::setPositionFromXml(float m[16], float f)
{
	_f = 2.0 * atan( 0.5* _h / m[0]);
	_pos[0] = -(m[0]*m[3] + m[4]*m[7] +m[8]*m[11]);
	_pos[1] = -(m[1]*m[3] + m[5]*m[7] +m[9]*m[11]);
	_pos[2] = -(m[2]*m[3] + m[6]*m[7] +m[10]*m[11]);

	_dir  = glm::normalize(glm::vec3(-m[8],-m[9],-m[10]));
	_side = glm::normalize(glm::vec3( m[0], m[1], m[2]));
	_up   = glm::normalize(glm::cross(_side, _dir));

	_mvsPnts.clear();
}

unsigned int   Camera::w          (void)          const { return _w;}
unsigned int   Camera::h          (void)          const { return _h;}
glm::vec3    Camera::pos        (void)          const { return _pos;}
glm::vec3    Camera::dir        (void)          const { return _dir;}
bool   Camera::active     (void)          const { return _active; }
void   Camera::setActive(bool active) {_active=active;}
float  Camera::aspect     (void)          const { return float(_w)/_h; }
unsigned int   Camera::numMVSPnts (void)          const { return _mvsPnts.size(); }

double Camera::mvsPntConfidence(unsigned int i)	const { return _mvsConfidence[i].first;}

glm::vec3 	Camera::mvsPosition(unsigned int i) const {return _mvsPnts[i].first; }
glm::vec3	Camera::mvsPntNormal(unsigned int i)			const { return _mvsNormal[i].first;}

glm::uvec2    Camera::mvs2DPos   (unsigned int i)        const { return _mvsPnts[i].second; }
void   Camera::addMVSPnt  (unsigned int id, glm::vec3 v,glm::uvec2 p)         {
	_mvsPatchCloudId.push_back(id); 
	_mvsPnts.push_back(std::make_pair(v,p));  }
void   Camera::addMVSPnt  (unsigned int id,glm::vec3 v,float confidence,glm::uvec2 p)         {
	_mvsPatchCloudId.push_back(id);
	_mvsPnts.push_back(std::make_pair(v,p)); 
	_mvsConfidence.push_back(std::make_pair(confidence,p));   
}

void   Camera::addMVSPnt  (unsigned int id,glm::vec3 v, glm::vec3 normal, float confidence,glm::uvec2 p)         { 
	_mvsPatchCloudId.push_back(id);
	_mvsPnts.push_back(std::make_pair(v,p)); 
	_mvsConfidence.push_back(std::make_pair(confidence,p));  
	_mvsNormal.push_back(std::make_pair(normal,p));
}


glm::mat4 Camera::lookMatrix(void) const {
	return glm::lookAt(_pos, _pos+_dir, glm::normalize(glm::cross(_side,_dir)));
}

glm::mat4 Camera::projMatrix(void) const {
	return glm::perspectiveFov((float)_f,(float) _w, (float)_h,(float) ZNEAR,(float) ZFAR);
}

void Camera::fixPrincipalPoint(float ppx, float ppy ) {

	 _ppx = ppx;
	 _ppy = ppy;

	fixProjection();
}
void Camera::fixProjection(void){

	float f =_focal_pixel;

	//--to update when _ppx and _ppy are loaded
	//matrice --K
	_K = glm::mat3  (
		_focal_pixel, 0.0f, 0.0f ,
		0.0f, _focal_pixel, 0.0f,
		_ppx*_w, _ppy*_h, 1.0f);

	 _R =glm::mat3(
		r[0], r[3], r[6],
		r[1], r[4], r[7],
		r[2], r[5], r[8]);

	 _T=glm::vec3(t[0], t[1], t[2]);

	//--move to bundle world and rotate
	_T = glm::vec3(1.0f, -1.0f,-1.0f) *_T ;
	_R = glm::mat3(1.0f,0.0f,0.0f,  0.0f,-1.0f,0.0f,  0.0f,0.0f,-1.0f) *_R ;

	_RT =glm::mat4x3(
		_R[0][0] , _R[0][1],_R[0][2], 
		_R[1][0] , _R[1][1],_R[1][2],
		_R[2][0] , _R[2][1],_R[2][2],
		_T[0], _T[1], _T[2]
	);

	glm::vec3				_translation(_RT[3][0],_RT[3][1],_RT[3][2]);
	//glm::mat3x3				_rotation(_RT[0][0],_RT[1][0],_RT[2][0],_RT[0][1],_RT[1][1],_RT[2][1],_RT[0][2],_RT[1][2],_RT[2][2]);

	glm::mat3x3				_invRotation=glm::inverse(_R);
	glm::vec3				_invTranslation=-(_invRotation*_translation);

	
	///*
	glm::vec3 camPos = _invTranslation;
	glm::vec3 camDir(0,0,1);
	glm::vec3 camUp(0,-1,0);

	camDir=_invRotation*camDir;
	camUp=_invRotation*camUp;

	_pos=camPos;
	_dir=camDir;

	/*
	_pos[0] = -(_R[0][0]*t[0] + _R[0][1]*t[1] + _R[0][2]*t[2]);
	_pos[1] = -(_R[1][0]*t[0] + _R[1][1]*t[1] + _R[1][2]*t[2]);
	_pos[2] = -(_R[2][0]*t[0] + _R[2][1]*t[1] + _R[2][2]*t[2]);

	_dir  = glm::normalize(glm::vec3(-_R[0][2],_R[1][2],_R[2][2]));
	_side = glm::normalize(glm::vec3(_R[0][0],_R[1][0],_R[2][0]));
	_up   = glm::normalize(glm::cross(_side, _dir));
	*/
	
}

glm::vec2 Camera::project(glm::vec3 p3d, glm::vec3 &tpos, glm::vec3 &tforw, glm::vec3 &tup , glm::mat4x3 &mat) const {
	glm::vec4 test_point( p3d[0],p3d[1], p3d[2] ,1.0);
	glm::vec4 valpos( 0,0,0,1.0);
	glm::vec4 valup( 0,1,0,0.0);
	glm::vec4 valfor( 0,0,1,0);

	glm::vec3 project_on_image = _K*_RT * test_point;
	tpos = _RT * valpos;
	tup = _RT * valup;
	tforw = _RT * valfor;
	mat=_RT;
	project_on_image = project_on_image/project_on_image[2];
    return glm::vec2(project_on_image[0], project_on_image[1]);
}

glm::vec2 Camera::project(glm::vec3 p3d) const {
	glm::vec4 test_point( p3d[0],p3d[1], p3d[2] ,1.0);
	glm::vec3 project_on_image = _K*_RT * test_point;
	project_on_image = project_on_image/project_on_image[2];
	return glm::vec2(project_on_image[0], project_on_image[1]);
}


void Camera::resize(float scale){

	_w  = std::floor( scale*_w );
	_h  = std::floor( scale*_h );

	_focal_pixel =_focal_pixel*scale ;

	fixProjection();
}

std::string Camera::getImgFilename(){
	return _img_filename;
}
void Camera::setImgFilename(std::string filename){
	_img_filename = filename;
}

std::vector<unsigned int> Camera::getPointCloudVizPointsId(){
	return _mvsPatchCloudId;
}