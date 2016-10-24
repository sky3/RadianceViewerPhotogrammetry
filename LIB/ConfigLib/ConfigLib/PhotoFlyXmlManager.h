#ifndef _PHOTOFLYXMLMANAGER_H_
#define _PHOTOFLYXMLMANAGER_H_

#include "CommonInclude.h"
#include <map>

class Cfrm
{
	public:
		Cfrm(){memset(&r,0,3*sizeof(double));memset(&t,0,3*sizeof(double));}

		int cf;
		double fovx;
		double rd1;
		double rd2;
		double ppx;
		double ppy;
		double r[3];
		double t[3];
		float  m[16];
};
class Shot
{
	public:
		Shot(){used=false;}

	unsigned int id;
	std::string name;
	int ci;
	unsigned int width;
	unsigned int height;
	bool used;

	Cfrm cfrm;
};

class CONFIGLIB_API PhotoFlyXmlManager 
{
	public:

		~PhotoFlyXmlManager(void);

		static void					loadPhotoFlyFile();
		static void					loadPhotoFlyFile(std::string file);
		static std::vector<Shot>	getCamInf();
		static Shot					getCamInf(unsigned int id);
		static int					getNbCam();

		static int					getIdBundleToCamera(int id);

		static bool                 isLoaded(void);
		

	private:

		PhotoFlyXmlManager(void);
		static void init();

		static PhotoFlyXmlManager *pfxManager;
		static XercesImpl* pImpl;

		static std::vector<Shot> camInf;

		static std::map<int, int> idBundleToCamera;

		bool _isload;
		

};

#endif;

