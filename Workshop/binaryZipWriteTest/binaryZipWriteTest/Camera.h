#ifndef _CAMERA_H_
#define _CAMERA_H_


class Camera {

private:
	unsigned int  _id;
	unsigned int  _w;
	unsigned int  _h;
	bool  _active;

	//std::shared_ptr< cimg_library::CImg<float> > _img;

	glm::vec3   _pos;
	glm::vec3   _dir;
	glm::vec3   _side;
	glm::vec3   _up;
	float _f;

	std::vector< unsigned int> _mvsPatchCloudId;
	std::vector< std::pair<glm::vec3,glm::uvec2> >  _mvsPnts;
	std::vector< std::pair<float,glm::uvec2> > _mvsConfidence;
	std::vector< std::pair<glm::vec3,glm::uvec2> > _mvsNormal;

	float r[9], t[3];

	glm::mat4x3 _RT;
	glm::vec3 _T;
	glm::mat3 _R;
	glm::mat3 _K;
	float _focal_pixel;
	float _ppx;
	float _ppy;

	std::string _img_filename;

	void fixProjection(void);

public:
	Camera(unsigned int id, float m[15], unsigned int w, unsigned int h, bool a, float ppx, float ppy); 
	Camera(unsigned int id, float m[15], unsigned int w, unsigned int h, bool a);
	Camera(unsigned int id, unsigned int w, unsigned int h, bool a);

	void setPositionFromXml(float m[16], float f);

	float getF(){return _f;}
	float getFocalPixel(){return _focal_pixel;};

	unsigned int   w          (void)          const ;
	unsigned int   h          (void)          const ;
	glm::vec3   pos        (void)          const ;
	glm::vec3   dir        (void)          const ;
	glm::vec3 up(void) const{return _up;};
	glm::vec3 side(void) const{return _side;};
	glm::mat4x3 RT(void) const{return _RT;};
	bool   active     (void)          const ;
	void   setActive (bool active);
	float  aspect     (void)          const ;
	unsigned int   numMVSPnts (void)          const ;

	std::string getImgFilename();
	void setImgFilename(std::string filename);

	double mvsPntConfidence(unsigned int i)	const ;
	glm::vec3 mvsPosition(unsigned int i) const ;
	glm::vec3 mvsPntNormal(unsigned int i) const ;
	glm::uvec2 mvs2DPos(unsigned int i)   const ;

	void   addMVSPnt  (unsigned int id, glm::vec3 v,glm::uvec2 p);
	void   addMVSPnt  (unsigned int id, glm::vec3 v,float confidence,glm::uvec2 p)   ;
	void   addMVSPnt  (unsigned int id, glm::vec3 v, glm::vec3 normal, float confidence, glm::uvec2 p)  ;

	glm::mat4 lookMatrix(void) const ;
	glm::mat4 projMatrix(void) const ;
	
	glm::vec2 project(glm::vec3 p3d) const;
	glm::vec2 project(glm::vec3 p3d, glm::vec3 &tpos, glm::vec3 &tforw, glm::vec3 &tup, glm::mat4x3 &mat ) const;

	void fixPrincipalPoint(float ppx, float ppy ); 
	
	void resize(float scale);

	std::vector<unsigned int> getPointCloudVizPointsId();
};

#endif // _CAMERA_H_
