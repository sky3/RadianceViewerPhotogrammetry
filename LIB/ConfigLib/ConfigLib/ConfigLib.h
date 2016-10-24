#ifndef CONFIGLIB_H
#define CONFIGLIB_H

#include "CommonInclude.h"

#define SUNDIR_SIZE 3
#define SUNCOLOR_SIZE 3

#define	TRANSLATION_SIZE 3
#define	ROTATION_SIZE 9

class CONFIGLIB_API ObjXml
{
	public:
		static int globalID;
		ObjXml(std::string op, std::string st):objPath(op),sizeTag(st),objID(globalID){globalID++;};
		const int objID;
		const std::string objPath;
		const std::string sizeTag;
		const std::string getIdString();
		
};

// This class is exported from the ConfigLib.dll
class CONFIGLIB_API CConfigLib 
{
	public:

		~CConfigLib(void);

		static void loadConfigFile(std::string file);
		static void option(int argc, char **argv);
		static void _assert();
		static bool isLoaded(void);

		static std::string getXmlFile();

		static std::string getName();
		static std::string getRoot();
		static std::string getBundle();
		static std::string getPatch();
		static std::string getPhotoFlyXmlFile();

		static std::string getJpgPath(bool undist=true, bool createDir=true);
		static std::string getJpgExt(bool undist=true);
		static std::string getTifPath(bool undist=true, bool createDir=true);
		static std::string getTifExt(bool undist=true);
		static std::string getPngPath(bool undist=true, bool createDir=true);

		static std::string getRawPath();
		static std::string getRawExt();

		static std::string getEnvmap();
		static std::string getEnvmapSky();
		static std::vector<ObjXml> getObj();

		static std::string getPmvsPath(bool createDir=true);
		static std::string getSpmvsPath(bool createDir=true);
		static std::string getSobjPath(bool createDir=true);
						   
		static std::string getPmvsName();
		static std::string getSpmvsName();
		static std::string getSobjName();
						   
		static std::string getPmvsExt();
		static std::string getSpmvsExt();
		static std::string getSobjExt();

		static std::string getSobj(int id);
		static std::string getSpmvs(int id);
		static std::string getPmvs(int id);

		static std::string getShadowProxyPath();
		static std::string getShadowProxyBaseName();
		static std::string getShadowProxyExt();
		static std::string getShadowProxy(int id);

		static std::string getDepthPath();
		static std::string getDepthBaseName();
		static std::string getDepthExt();
		static std::string getDepth(int id);

		static std::string getNormalPath();
		static std::string getNormalBaseName();
		static std::string getNormalExt();
		static std::string getNormal(int id);

		static std::string getOutSuncalib();
		static std::string getOutIntrinsicFolder();
		static std::string getOutRadianceFolder();

		static int getDownScale();

		static std::vector<double> getWorldRotation();
		static std::vector<double> getWorldTranslation();

		static std::vector<double> getSunDirection();
		static std::vector<double> getSunColor();

		static std::vector<double> getDownscaleIntrinsic();

		static std::vector<unsigned int> getIntrinsicImageSet();
		static std::vector<unsigned int> getImagesSet();

		static int getSample();

		static int getRenderingOption();
		static int	getIdObjForRendering();


	private:

		static CConfigLib *configLib;

		CConfigLib(void);

		static void getSmpvs();
		static void getPmvs();
		static void getSobj();
		static void getShadow();
		static void getDepth();
		static void getNormal();
		static void completePath(std::string &path);
		static void checkPath(std::string &path);

		static XercesImpl* pImpl;

		static std::string xmlFile;

		static std::string name;
		static std::string root;
		static std::string bundle;
		static std::string patch;
		static std::string photoFly;

		static std::string pngPath;
		static std::string pngUndistPath;
		static std::string jpgPath;
		static std::string jpgExt;
		static std::string tifPath;
		static std::string tifExt;
		static std::string jpgUndistPath;
		static std::string jpgUndistExt;
		static std::string tifUndistPath;
		static std::string tifUndistExt;
		static std::string rawPath;
		static std::string rawExt;

		static std::string pathShadowProxy;
		static std::string shadowProxyBaseName;
		static std::string shadowProxyExt;

		static std::string pathDepth;
		static std::string depthBaseName;
		static std::string depthExt;

		static std::string pathNormal;
		static std::string normalBaseName;
		static std::string normalExt;

		static std::string envmap;
		static std::string envmapSky;

		static std::string pmvsPath;
		static std::string spmvsPath;
		static std::string sobjPath;

		static std::string pmvsName;
		static std::string spmvsName;
		static std::string sobjName;

		static std::string pmvsExt;
		static std::string spmvsExt;
		static std::string sobjExt;

		static std::vector<ObjXml> obj;
		static bool obj_isSet;
		

		static std::string outRadianceFolder;


		static std::string outSuncalib;
		static std::string outIntrinsicFolder;
		
		static int downscale;
		static bool   downscale_isSet;

		static std::vector<double> worldRotation;
		static bool   worldRotation_isSet;

		static std::vector<double> worldTranslation;
		static bool   worldTranslation_isSet;

		static std::vector<double> sunDirection;
		static bool   sunDirection_isSet;

		static std::vector<double> sunColor;
		static bool   sunColor_isSet;

		static std::vector<unsigned int> imagesSet;
		static bool   imagesSet_isSet;

		static std::vector<unsigned int> intrinsicImageSet;
		static bool   intrinsicImageSet_isSet;

		static std::vector<double> downscaleIntrinsic;
		static bool   downscaleIntrinsic_isSet;

		static int sample;
		static int renderingOption;

		static int idObjForRendering;

};

#endif;
