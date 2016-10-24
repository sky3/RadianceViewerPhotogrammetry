#include "Envmap.h"
#include <FreeImage.h>

Envmap Envmap::m_instance=Envmap();

Envmap& Envmap::Instance()
{
    return m_instance;
}


Envmap::Envmap(){
	_width=0;
	_height=0;
	//envmap_array = new float[1];
	_loaded =false;
}

Envmap::Envmap(const Envmap  &e){
	_width=e._width;
	_height=e._height;

	envmap_array = new float[_height*_width*3];
	for (unsigned int y = 0; y < _height ; y++){
		for (unsigned int x = 0; x < _width ; x++){


			envmap_array[(y)*_width*3+x*3] = e.envmap_array[(y)*_width*3+x*3] ;
			envmap_array[(y)*_width*3+x*3+1] = e.envmap_array[(y)*_width*3+x*3+1];
			envmap_array[(y)*_width*3+x*3+2] = e.envmap_array[(y)*_width*3+x*3+2] ;
		}
	}
	_loaded =true;
}

Envmap& Envmap::operator= (const Envmap &e){

	m_instance._width=e._width;
	m_instance._height=e._height;

	m_instance.envmap_array = new float[_height*_width*3];
	for (unsigned int y = 0; y < _height ; y++){
		for (unsigned int x = 0; x < _width ; x++){


			m_instance.envmap_array[(y)*_width*3+x*3] = e.envmap_array[(y)*_width*3+x*3] ;
			m_instance.envmap_array[(y)*_width*3+x*3+1] = e.envmap_array[(y)*_width*3+x*3+1];
			m_instance.envmap_array[(y)*_width*3+x*3+2] = e.envmap_array[(y)*_width*3+x*3+2] ;
		}
	}
	return m_instance;
}

void Envmap::load(std::string filename)
{
	if(filename!="")
	{
		FIBITMAP *dib = FreeImage_Load(FIF_TIFF, filename.c_str(), 0);
		_width = FreeImage_GetWidth(dib);
		_height = FreeImage_GetHeight(dib);
		unsigned pitch = FreeImage_GetPitch(dib);
		FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(dib);

		//delete envmap_array;

		envmap_array = new float[_height*_width*3];

		float min = FLT_MAX;
		float max = FLT_MIN;

		if(image_type == FIT_RGBF) 
		{
			BYTE *bits = (BYTE*)FreeImage_GetBits(dib);
			for(int y = 0; y < _height; y++) 
			{
				FIRGBF *pixel = (FIRGBF*)bits;
				for(int x = 0; x < _width; x++) 
				{
					envmap_array[(_height-y-1)*_width*3+x*3] = pixel[x].red;
					envmap_array[(_height-y-1)*_width*3+x*3+1] = pixel[x].green;
					envmap_array[(_height-y-1)*_width*3+x*3+2] = pixel[x].blue;
				}
				bits += pitch;
			}
		}
		else if(image_type == FIT_RGB16)
		{
			BYTE *bits = (BYTE*)FreeImage_GetBits(dib);
			for(int y = 0; y < _height; y++) 
			{
				FIRGB16 *pixel = (FIRGB16*)bits;
				for(int x = 0; x < _width; x++) 
				{
					envmap_array[(_height-y-1)*_width*3+x*3] = pixel[x].red;
					envmap_array[(_height-y-1)*_width*3+x*3+1] = pixel[x].green;
					envmap_array[(_height-y-1)*_width*3+x*3+2] = pixel[x].blue;
				}
				bits += pitch;
			}
		}
	}
		_loaded =true;
}

Envmap::~Envmap(){
	_loaded =false;
	delete []envmap_array;

}

unsigned int Envmap::W(){
	return _width;
}

unsigned int Envmap::H(){
	return _height;
}
 bool Envmap::isLoaded(void){
	 return _loaded;
}

glm::vec3 Envmap::getEnvmapColor(glm::vec3 dir) const {
	//longitute latitude to x,y pixel coord
	/*
	float temp=dir.y;
	dir.y=dir.z;
	dir.z=temp;
	*/
	//Vec3f dir=hemisphereDir[i];
	dir = glm::normalize(dir);
	double dirLength = std::sqrt((double)dir.x*(double)dir.x+(double)dir.y*(double)dir.y+(double)dir.z*(double)dir.z);
	dir/=dirLength;

	if(dir.z>1.f)
		dir.z=1.f;
	if(dir.z<-1.f)
		dir.z=-1.f;

	float phi=embree::acos(dir.z);
	float theta;
	//*
	if((double)dir.y>=0){
		theta=embree::acos(dir.x/std::sqrt((double)dir.x*(double)dir.x+(double)dir.y*(double)dir.y));
	}
	else{
		theta=2*glm::pi<float>()-embree::acos(dir.x/std::sqrt((double)dir.x*(double)dir.x+(double)dir.y*(double)dir.y));
	}
	//*/
	/*
	theta = atan2f(dir.y, dir.x);
	if(theta<0.f)
	theta+=2.f*M_PI;
	*/
	int yEnv=(int)((phi)*(float)(_height-1)*glm::one_over_pi<float>());
	int xEnv=(int)((theta)*(float)(_width-1)*glm::one_over_pi<float>());

	if(yEnv<0)
		yEnv=0;
	if(yEnv>=_height)
		yEnv=_height-1;

	if(xEnv<0)
		xEnv=0;
	if(xEnv>=_width)
		xEnv=_width-1;


	float phi2 = 1/*std::max(0.f, phi)*/;
	//yEnv=loader.envmapHeight-1-yEnv;

	glm::vec3 color=glm::vec3(envmap_array[yEnv*_width*3+xEnv*3]*phi2,
		envmap_array[yEnv*_width*3+xEnv*3+1]*phi2,
		envmap_array[yEnv*_width*3+xEnv*3+2]*phi2);

	return color;
}