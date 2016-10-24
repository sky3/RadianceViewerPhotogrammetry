// ConfigLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ConfigLib.h"

#define cimg_display 0
#include "CImg.h"

#include <iostream>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <sstream>

using namespace std;

#include "XercesImpl.h"

CConfigLib *CConfigLib::configLib=_NULLPTR_;
XercesImpl *CConfigLib::pImpl=new XercesImpl;
/*
xercesc::XercesDOMParser *CConfigLib::parser=_NULLPTR_;

XMLCh* CConfigLib::ATTR_value=_NULLPTR_;
XMLCh* CConfigLib::ATTR_path=_NULLPTR_;
XMLCh* CConfigLib::ATTR_ext=_NULLPTR_;
*/

int ObjXml::globalID=0;

string CConfigLib::name;
string CConfigLib::root;
std::string CConfigLib::bundle;
std::string CConfigLib::patch;
std::string CConfigLib::photoFly;

std::string CConfigLib::pngPath;
std::string CConfigLib::jpgPath;
std::string CConfigLib::jpgExt;
std::string CConfigLib::tifPath;
std::string CConfigLib::tifExt;

std::string CConfigLib::pngUndistPath;
std::string CConfigLib::jpgUndistPath;
std::string CConfigLib::jpgUndistExt;
std::string CConfigLib::tifUndistPath;
std::string CConfigLib::tifUndistExt;

std::string CConfigLib::shadowProxyBaseName;
std::string CConfigLib::shadowProxyExt;
std::string CConfigLib::pathShadowProxy;

std::string CConfigLib::depthBaseName;
std::string CConfigLib::depthExt;
std::string CConfigLib::pathDepth;

std::string CConfigLib::normalBaseName;
std::string CConfigLib::normalExt;
std::string CConfigLib::pathNormal;

std::string CConfigLib::rawPath;
std::string CConfigLib::rawExt;
std::string CConfigLib::xmlFile;

std::string CConfigLib::envmap;
std::string CConfigLib::envmapSky;
std::vector<ObjXml> CConfigLib::obj;
bool CConfigLib::obj_isSet=false;

std::string CConfigLib::pmvsPath;
std::string CConfigLib::spmvsPath;
std::string CConfigLib::sobjPath;
			 
std::string CConfigLib::pmvsName;
std::string CConfigLib::spmvsName;
std::string CConfigLib::sobjName;
	
std::string CConfigLib::pmvsExt;
std::string CConfigLib::spmvsExt;
std::string CConfigLib::sobjExt;

std::string CConfigLib::outSuncalib;
std::string CConfigLib::outIntrinsicFolder;
std::string CConfigLib::outRadianceFolder;

int CConfigLib::sample=-1;

int CConfigLib::downscale=1;
bool   CConfigLib::downscale_isSet=false;

vector<double> CConfigLib::worldRotation=vector<double>(9);
bool   CConfigLib::worldRotation_isSet=false;
	  
vector<double> CConfigLib::worldTranslation=vector<double>(3);
bool   CConfigLib::worldTranslation_isSet=false;
	   
vector<double> CConfigLib::sunDirection=vector<double>(3);
bool   CConfigLib::sunDirection_isSet=false;

vector<double> CConfigLib::sunColor=vector<double>(3);
bool   CConfigLib::sunColor_isSet=false;

std::vector<unsigned int> CConfigLib::imagesSet=vector<unsigned int>();
bool   CConfigLib::imagesSet_isSet=false;

std::vector<unsigned int> CConfigLib::intrinsicImageSet=vector<unsigned int>();
bool   CConfigLib::intrinsicImageSet_isSet=false;

vector<double> CConfigLib::downscaleIntrinsic=vector<double>(1);
bool   CConfigLib::downscaleIntrinsic_isSet=false;

int CConfigLib::renderingOption=0;
int  CConfigLib::idObjForRendering=-1;

// This is the constructor of a class that has been exported.
// see ConfigLib.h for the class definition
CConfigLib::CConfigLib()
{
	downscale_isSet=false;
}

CConfigLib::~CConfigLib()
{
	if(configLib)
	{
	   delete pImpl;
	}
}

void CConfigLib::option(int argc, char **argv)
{
	cimg_usage("Retrieve command line arguments");
	/*
	const char* filename = cimg_option("-i","image.gif","Input image file");
	const char* output   = cimg_option("-o",(char*)0,"Output image file");
	const double sigma   = cimg_option("-s",1.0,"Standard variation of the gaussian smoothing");
	const  int nblevels  = cimg_option("-n",16,"Number of quantification levels");
	*/
	xmlFile = string(cimg_option("-xml","","Input xml config file"));
	renderingOption = int(cimg_option("-r",0,"Raytracing option,(0: spmvs, 1 :render images, 2: envmap)"));

	if(!xmlFile.empty())
		loadConfigFile(xmlFile);
}

void  CConfigLib::_assert()
{
	assert(xmlFile!="");
}


void CConfigLib::loadConfigFile(string file) throw(runtime_error)
{
	xmlFile = file;
	if(!configLib)
	{
		configLib = new CConfigLib;
	}

	try
	{
		pImpl->Init(file);
	}
	catch(const std::exception & e)
	{
		throw(e);
	}
}

std::string CConfigLib::getName() throw(runtime_error)
{
	if(pImpl->is_parsed())
	{
		if(name.empty())
		{
			try{ name = pImpl->getValue("Name");}
			catch(std::runtime_error &e){throw(e);}
		}
	}

	return name;
}

std::string CConfigLib::getPngPath(bool undist,bool createDir) throw(runtime_error)
{
	string ret;
	if(pImpl->is_parsed())
		if(!undist)
		{
			if(pngPath.empty())
			{
				try
				{
					vector<string> val = pImpl->findPath("Png");
					if(val.size()==1)
					{
						pngPath=val[0];
						completePath(pngPath);
						if(createDir)
							if(!CreateDirectoryA(pngPath.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
							{
								ostringstream oss;
								oss<<"Directory error "<<GetLastError();
								throw(runtime_error(oss.str()));
							}
					}
				}
				catch(std::runtime_error &e)
				{
					throw(e);
				}
			}
			ret=pngPath;
		}
		else
		{
			if(pngUndistPath.empty())
			{
				try
				{
					vector<string> val = pImpl->findPath("Png_Undist");
					if(val.size()==1)
					{
						pngUndistPath=val[0];
						completePath(pngUndistPath);
						if(createDir)
							if(!CreateDirectoryA(pngUndistPath.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
							{
								ostringstream oss;
								oss<<"Directory error "<<GetLastError();
								throw(runtime_error(oss.str()));
							}
					}
				}
				catch(std::runtime_error &e)
				{
					throw(e);
				}
			}
			ret=pngUndistPath;
		}

	return ret;
}

std::string CConfigLib::getJpgPath(bool undist, bool createDir) throw(runtime_error)
{
	string ret;
	if(pImpl->is_parsed())
		if(!undist)
		{
			if(jpgPath.empty())
			{
				try
				{
					vector<string> val = pImpl->findPath("Jpg",true);
					if(val.size()==2)
					{
						jpgPath=val[0];
						jpgExt=val[1];
						completePath(jpgPath);
						if(createDir)
							if(!CreateDirectoryA(jpgPath.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
							{
								ostringstream oss;
								oss<<"Directory error "<<GetLastError();
								throw(runtime_error(oss.str()));
							}
					}
				}
				catch(std::runtime_error &e)
				{
					throw(e);
				}
			}
			ret=jpgPath;
		}
		else
		{
			if(jpgUndistPath.empty())
			{
				try
				{
					vector<string> val = pImpl->findPath("Jpg_Undist",true);
					if(val.size()==2)
					{
						jpgUndistPath=val[0];
						jpgUndistExt=val[1];
						completePath(jpgUndistPath);
						if(createDir)
							if(!CreateDirectoryA(jpgUndistPath.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
							{
								ostringstream oss;
								oss<<"Directory error "<<GetLastError();
								throw(runtime_error(oss.str()));
							}
					}
				}
				catch(std::runtime_error &e)
				{
					throw(e);
				}
			}
			ret=jpgUndistPath;
		}

	return ret;
}

std::string CConfigLib::getJpgExt(bool undist) throw(runtime_error)
{
	string ret;
	if(pImpl->is_parsed())
		if(!undist)
		{
			if(jpgPath.empty())
			{
				try
				{
					vector<string> val = pImpl->findPath("Jpg",true);
					if(val.size()==2)
					{
						jpgPath=val[0];
						jpgExt=val[1];
					}
				}
				catch(std::runtime_error &e)
				{
					throw(e);
				}
			}
			ret=jpgExt;
		}
		else
		{
			if(jpgUndistPath.empty())
			{
				try
				{
					vector<string> val = pImpl->findPath("Jpg_Undist",true);
					if(val.size()==2)
					{
						jpgUndistPath=val[0];
						jpgUndistExt=val[1];
					}
				}
				catch(std::runtime_error &e)
				{
					throw(e);
				}
			}
			ret=jpgUndistExt;
		}

	return ret;
}

std::string CConfigLib::getTifPath(bool undist,bool createDir) throw(runtime_error)
{
	string ret;
	if(pImpl->is_parsed())
		if(!undist)
		{
			if(tifPath.empty())
			{
				try
				{
					vector<string> val = pImpl->findPath("Tif",true);
					if(val.size()==2)
					{
						tifPath=val[0];
						tifExt=val[1];
						completePath(tifPath);
						if(createDir)
							if(!CreateDirectoryA(tifPath.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
							{
								ostringstream oss;
								oss<<"Directory error "<<GetLastError();
								throw(runtime_error(oss.str()));
							}
					}
				}
				catch(std::runtime_error &e)
				{
					throw(e);
				}
			}
			ret=tifPath;
		}
		else
		{
			if(tifUndistPath.empty())
			{
				try
				{
					vector<string> val = pImpl->findPath("Tif_Undist",true);
					if(val.size()==2)
					{
						tifUndistPath=val[0];
						tifUndistExt=val[1];
						completePath(tifUndistPath);
						if(createDir)
							if(!CreateDirectoryA(tifUndistPath.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
							{
								ostringstream oss;
								oss<<"Directory error "<<GetLastError();
								throw(runtime_error(oss.str()));
							}
					}
				}
				catch(std::runtime_error &e)
				{
					throw(e);
				}
			}
			ret=tifUndistPath;
		}

	return ret;
}

std::string CConfigLib::getTifExt(bool undist) throw(runtime_error)
{
	string ret;
	if(pImpl->is_parsed())
		if(!undist)
		{
			if(tifPath.empty())
			{
				try
				{
					vector<string> val = pImpl->findPath("Tif",true);
					if(val.size()==2)
					{
						tifPath=val[0];
						tifExt=val[1];
					}
				}
				catch(std::runtime_error &e)
				{
					throw(e);
				}
			}
			ret=tifExt;
		}
		else
		{
			if(tifUndistPath.empty())
			{
				try
				{
					vector<string> val = pImpl->findPath("Tif_Undist",true);
					if(val.size()==2)
					{
						tifUndistPath=val[0];
						tifUndistExt=val[1];
					}
				}
				catch(std::runtime_error &e)
				{
					throw(e);
				}
			}
			ret=tifUndistExt;
		}

	return ret;
}

std::string CConfigLib::getRawPath() throw(runtime_error)
{
	if(pImpl->is_parsed())
		if(rawPath.empty())
		{
			try
			{
				vector<string> val = pImpl->findPath("Raw",true);
				if(val.size()==2)
				{
					rawPath=val[0];
					rawExt=val[1];
					completePath(rawPath);
				}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return rawPath;
}

std::string CConfigLib::getRawExt() throw(runtime_error)
{
	if(pImpl->is_parsed())
		if(rawExt.empty())
		{
			try
			{
				vector<string> val = pImpl->findPath("Raw",true);
				if(val.size()==2)
				{
					rawPath=val[0];
					rawExt=val[1];
				}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return rawExt;
}

std::string CConfigLib::getPatch() throw(runtime_error)
{
	if(pImpl->is_parsed())
		if(patch.empty())
		{
			try
			{
				string val = pImpl->getValue("Patch");
				if(val.size()>0) 
				{
					patch=val;
					completePath(patch);
				}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return patch;
}

std::string CConfigLib::getBundle() throw(runtime_error)
{
	if(pImpl->is_parsed())
		if(bundle.empty())
		{
			try
			{
				string val = pImpl->getValue("Bundle");
				if(val.size()>0) 
				{
					bundle=val;
					completePath(bundle);
				}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

		return bundle;
}

std::vector<ObjXml> CConfigLib::getObj() throw(runtime_error)
{
	if(pImpl->is_parsed())
		if(!obj_isSet)
		{
			try
			{
				vector<map<string,string> > map = pImpl->getAttributes("Obj");
				for(int i=0;i<map.size();i++)
				{
					string path=map[i]["value"];
					completePath(path);
					obj.push_back(ObjXml(path,map[i]["sizeTag"]));
					obj_isSet=true;
				}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return obj;

}

void CConfigLib::getSmpvs() throw(runtime_error)
{
	try
	{
		vector<map<string,string> > map = pImpl->getAttributes("Spmvs");
		if(map.size()>0)
		{
			spmvsPath=map[0]["path"];
			spmvsName=map[0]["name"];
			spmvsExt="."+map[0]["ext"];
		}
	}
	catch(std::runtime_error &e)
	{
		throw(e);
	}
}
void CConfigLib::getPmvs() throw(runtime_error)
{
	try
	{
		vector<map<string,string> > map = pImpl->getAttributes("Pmvs");
		if(map.size()>0)
		{
			pmvsPath=map[0]["path"];
			pmvsName=map[0]["name"];
			pmvsExt="."+map[0]["ext"];
		}
	}
	catch(std::runtime_error &e)
	{
		throw(e);
	}
}
void CConfigLib::getSobj() throw(runtime_error)
{
	try
	{
		vector<map<string,string> > map = pImpl->getAttributes("Sobj");
		if(map.size()>0)
		{
			sobjPath=map[0]["path"];
			sobjName=map[0]["name"];
			sobjExt="."+map[0]["ext"];
		}
	}
	catch(std::runtime_error &e)
	{
		throw(e);
	}
}

std::string CConfigLib::getPmvsPath(bool createDir) throw(runtime_error)
{
	if(pImpl->is_parsed())
		if(pmvsPath.empty())
		{
			try
			{
				getPmvs();
				completePath(pmvsPath);
				if(createDir)
					if(!CreateDirectoryA(pmvsPath.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
					{
						ostringstream oss;
						oss<<"Directory error "<<GetLastError();
						throw(runtime_error(oss.str()));
					}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return pmvsPath;
}			
std::string CConfigLib::getSpmvsPath(bool createDir) throw(runtime_error)
{
	if(pImpl->is_parsed())
		if(spmvsPath.empty())
		{
			try
			{
				getSmpvs();
				completePath(spmvsPath);
				if(createDir)
					if(!CreateDirectoryA(spmvsPath.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
					{
						ostringstream oss;
						oss<<"Directory error "<<GetLastError();
						throw(runtime_error(oss.str()));
					}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return spmvsPath;
}			
std::string CConfigLib::getSobjPath(bool createDir) throw(runtime_error)
{		
	if(pImpl->is_parsed())
		if(sobjPath.empty())
		{
			try
			{
				getSobj();
				completePath(sobjPath);
				if(createDir)
					if(!CreateDirectoryA(sobjPath.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
					{
						ostringstream oss;
						oss<<"Directory error "<<GetLastError();
						throw(runtime_error(oss.str()));
					}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return sobjPath;
}			
			   
std::string CConfigLib::getPmvsName() throw(runtime_error)
{	
	if(pImpl->is_parsed())
		if(pmvsName.empty())
		{
			try
			{
				getPmvs();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return pmvsName;
}			
std::string CConfigLib::getSpmvsName() throw(runtime_error)
{		
	if(pImpl->is_parsed())
		if(spmvsName.empty())
		{
			try
			{
				getSmpvs();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return spmvsName;
}			
std::string CConfigLib::getSobjName() throw(runtime_error)
{	
	if(pImpl->is_parsed())
		if(sobjName.empty())
		{
			try
			{
				getSobj();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return sobjName;
}	

std::string CConfigLib::getSobj(int id)
{
	std::ostringstream oss;
	oss<<getSobjPath()<<getSobjName()<<getObj()[id].getIdString()<<getSobjExt();
	return oss.str();
}
std::string CConfigLib::getPmvs(int id)
{
	std::ostringstream oss;
	oss<<getPmvsPath()<<getPmvsName()<<getObj()[id].getIdString()<<getPmvsExt();
	return oss.str();
}
std::string CConfigLib::getSpmvs(int id)
{
	std::ostringstream oss;
	oss<<getSpmvsPath()<<getSpmvsName()<<getObj()[id].getIdString()<<getSpmvsExt();
	return oss.str();
}
			   
std::string CConfigLib::getPmvsExt() throw(runtime_error)
{	
	if(pImpl->is_parsed())
		if(pmvsExt.empty())
		{
			try
			{
				getPmvs();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return pmvsExt;
}			
std::string CConfigLib::getSpmvsExt() throw(runtime_error)
{		
	if(pImpl->is_parsed())
		if(spmvsExt.empty())
		{
			try
			{
				getSmpvs();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return spmvsExt;
}			
std::string CConfigLib::getSobjExt() throw(runtime_error)
{
	if(pImpl->is_parsed())
		if(sobjExt.empty())
		{
			try
			{
				getSobj();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return sobjExt;
}

std::string CConfigLib::getEnvmap() throw(runtime_error)
{
	if(pImpl->is_parsed())
		if(envmap.empty())
		{
			try
			{
				string val = pImpl->getValue("Envmap");
				if(val.size()>0) 
				{
					envmap=val;
					completePath(envmap);
				}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

		return envmap;
}

std::string CConfigLib::getEnvmapSky() throw(runtime_error)
{
	if(pImpl->is_parsed())
		if(envmapSky.empty())
		{
			try
			{
				string val = pImpl->getValue("EnvmapSky");
				if(val.size()>0) 
				{
					envmapSky=val;
					completePath(envmapSky);
				}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}
	
		return envmapSky;
}

int CConfigLib::getDownScale()
{
	if(pImpl->is_parsed())
		if(!downscale_isSet)
		{
			try
			{
				vector<double> val = pImpl->getNumericalValue("Downscale");
				if(val.size()>0 && (int)val[0]>0)
					downscale=(int)val[0];
				else
					downscale=1;

				downscale_isSet=true;
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return downscale;
}

vector<double> CConfigLib::getWorldRotation()
{
	if(pImpl->is_parsed())
		if(!worldRotation_isSet)
		{
			try
			{
				vector<double> val = pImpl->getNumericalValue("WorldTransform_r");
				if(val.size()==ROTATION_SIZE)
					for(int i=0;i<ROTATION_SIZE;i++)
						worldRotation[i]=val[i];

				worldRotation_isSet=true;
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

		return worldRotation;
}

vector<double>  CConfigLib::getWorldTranslation()
{
	if(pImpl->is_parsed())
		if(!worldTranslation_isSet)
		{
			try
			{
				vector<double> val = pImpl->getNumericalValue("WorldTransform_t");
				if(val.size()==TRANSLATION_SIZE)
					for(int i=0;i<TRANSLATION_SIZE;i++)
						worldTranslation[i]=val[i];

				worldTranslation_isSet=true;
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

		return worldTranslation;
}

vector<double>  CConfigLib::getSunDirection()
{
	if(pImpl->is_parsed())
		if(!sunDirection_isSet)
		{
			try
			{
				vector<double> val = pImpl->getNumericalValue("SunDirection");
				if(val.size()==SUNDIR_SIZE)
					for(int i=0;i<SUNDIR_SIZE;i++)
						sunDirection[i]=val[i];

				sunDirection_isSet=true;
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

		return sunDirection;
}

vector<double>  CConfigLib::getDownscaleIntrinsic()
{
	if(pImpl->is_parsed())
		if(!downscaleIntrinsic_isSet)
		{
			try
			{
				vector<double> val = pImpl->getNumericalValue("downscaleIntrinsic");
				if(val.size()==1)
					for(int i=0;i<1;i++)
						downscaleIntrinsic[i]=val[i];

				downscaleIntrinsic_isSet=true;
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

		return downscaleIntrinsic;
}
vector<double>  CConfigLib::getSunColor()
{
	if(pImpl->is_parsed())
		if(!sunColor_isSet)
		{
			try
			{
				vector<double> val = pImpl->getNumericalValue("SunColor");
				if(val.size()==SUNCOLOR_SIZE)
					for(int i=0;i<SUNCOLOR_SIZE;i++)
						sunColor[i]=val[i];

				sunColor_isSet=true;
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

		return sunColor;
}

vector<unsigned int> CConfigLib::getImagesSet()
{
	if(pImpl->is_parsed())
		if(!imagesSet_isSet)
		{
			try
			{
				vector<string> val = pImpl->getValues("Set");
				imagesSet = pImpl->splitAndConvertToInt(val);
			}
			catch(std::runtime_error &e)
			{
				if(string(e.what())!=string("No Set"))
					throw(e);
			}
			imagesSet_isSet=true;
		}

		return imagesSet;
}

vector<unsigned int> CConfigLib::getIntrinsicImageSet()
{
	if(pImpl->is_parsed())
		if(!intrinsicImageSet_isSet)
		{
			try
			{
				vector<string> val = pImpl->getValues("IntrinsicImageSet");
				intrinsicImageSet = pImpl->splitAndConvertToInt(val);
			}
			catch(std::runtime_error &e)
			{
				if(string(e.what())!=string("No Set"))
					throw(e);
			}
			intrinsicImageSet_isSet=true;
		}

		return intrinsicImageSet;
}



std::string CConfigLib::getXmlFile()
{
	return xmlFile;
}

int CConfigLib::getSample()
{
	if(pImpl->is_parsed())
		if(sample==-1)
		{
			try
			{
				vector<double> val = pImpl->getNumericalValue("Sample");
				if(val.size()>0)
					sample=val[0];
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return sample;
}

std::string CConfigLib::getOutSuncalib()
{
	if(pImpl->is_parsed())
		if(outSuncalib.empty())
		{
			try
			{
				vector<string> val = pImpl->findPath("OutSunCalib");
				if(val.size()>0)
				{
					outSuncalib=val[0];
					completePath(outSuncalib);
					if(!CreateDirectoryA(outSuncalib.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
					{
						ostringstream oss;
						oss<<"Directory error "<<GetLastError();
						throw(runtime_error(oss.str()));
					}
				}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return outSuncalib;
}


std::string CConfigLib::getOutIntrinsicFolder()
{
	if(pImpl->is_parsed())
		if(outIntrinsicFolder.empty())
		{
			try
			{
				vector<string> val = pImpl->findPath("OutIntrinsicFolder");
				if(val.size()>0)
				{
					outIntrinsicFolder=val[0];
					completePath(outIntrinsicFolder);
					if(!CreateDirectoryA(outIntrinsicFolder.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
					{
						ostringstream oss;
						oss<<"Directory error "<<GetLastError();
						throw(runtime_error(oss.str()));
					}
				}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return outIntrinsicFolder;
}

std::string CConfigLib::getOutRadianceFolder()
{
	if(pImpl->is_parsed())
		if(outRadianceFolder.empty())
		{
			try
			{
				vector<string> val = pImpl->findPath("OutRadianceFolder");
				if(val.size()>0)
				{
					outRadianceFolder=val[0];
					completePath(outRadianceFolder);
					if(!CreateDirectoryA(outRadianceFolder.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
					{
						ostringstream oss;
						oss<<"Directory error "<<GetLastError();
						throw(runtime_error(oss.str()));
					}
				}
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return outRadianceFolder;
}

std::string CConfigLib::getPhotoFlyXmlFile()
{
	if(pImpl->is_parsed())
		if(photoFly.empty())
		{
			try
			{
				photoFly = pImpl->getValue("PhotoFly");
				completePath(photoFly);
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

		return photoFly;
}

void CConfigLib::getShadow()
{
	try
	{
		vector<map<string,string> > map = pImpl->getAttributes("ShadowProxy");
		if(map.size()>0)
		{
			pathShadowProxy=map[0]["path"];
			completePath(pathShadowProxy);
			checkPath(pathShadowProxy);
			if(!CreateDirectoryA(pathShadowProxy.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
			{
				ostringstream oss;
				oss<<"Directory error "<<GetLastError();
				throw(runtime_error(oss.str()));
			}
			shadowProxyBaseName=map[0]["name"];
			shadowProxyExt="."+map[0]["ext"];
		}
	}
	catch(std::runtime_error &e)
	{
		throw(e);
	}
}
void CConfigLib::getDepth()
{
	try
	{
		vector<map<string,string> > map = pImpl->getAttributes("Depth");
		if(map.size()>0)
		{
			pathDepth=map[0]["path"];
			completePath(pathDepth);
			checkPath(pathDepth);
			if(!CreateDirectoryA(pathDepth.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
			{
				ostringstream oss;
				oss<<"Directory error "<<GetLastError();
				throw(runtime_error(oss.str()));
			}
			depthBaseName=map[0]["name"];
			depthExt="."+map[0]["ext"];
		}
	}
	catch(std::runtime_error &e)
	{
		throw(e);
	}
}
void CConfigLib::getNormal()
{
	try
	{
		vector<map<string,string> > map = pImpl->getAttributes("Normal");
		if(map.size()>0)
		{
			pathNormal=map[0]["path"];
			completePath(pathNormal);
			checkPath(pathNormal);
			if(!CreateDirectoryA(pathNormal.c_str(), NULL) && GetLastError()!=ERROR_ALREADY_EXISTS)
			{
				ostringstream oss;
				oss<<"Directory error "<<GetLastError();
				throw(runtime_error(oss.str()));
			}
			normalBaseName=map[0]["name"];
			normalExt="."+map[0]["ext"];
		}
	}
	catch(std::runtime_error &e)
	{
		throw(e);
	}
}

std::string CConfigLib::getShadowProxyPath()
{
	if(pImpl->is_parsed())
		if(pathShadowProxy.empty())
		{
			try
			{
				getShadow();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return pathShadowProxy;
}
std::string CConfigLib::getShadowProxyBaseName()
{
	if(pImpl->is_parsed())
		if(shadowProxyBaseName.empty())
		{
			try
			{
				getShadow();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return shadowProxyBaseName;
}
std::string CConfigLib::getShadowProxyExt()
{
	if(pImpl->is_parsed())
		if(shadowProxyExt.empty())
		{
			try
			{
				getShadow();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return shadowProxyExt;
}

std::string CConfigLib::getDepthPath()
{
	if(pImpl->is_parsed())
		if(pathDepth.empty())
		{
			try
			{
				getDepth();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return pathDepth;
}
std::string CConfigLib::getDepthBaseName()
{
	if(pImpl->is_parsed())
		if(depthBaseName.empty())
		{
			try
			{
				getDepth();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return depthBaseName;
}
std::string CConfigLib::getDepthExt()
{
	if(pImpl->is_parsed())
		if(depthExt.empty())
		{
			try
			{
				getDepth();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return depthExt;
}

std::string CConfigLib::getNormalPath()
{
	if(pImpl->is_parsed())
		if(pathNormal.empty())
		{
			try
			{
				getNormal();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return pathNormal;
}
std::string CConfigLib::getNormalBaseName()
{
	if(pImpl->is_parsed())
		if(normalBaseName.empty())
		{
			try
			{
				getNormal();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

	return normalBaseName;
}
std::string CConfigLib::getNormalExt()
{
	if(pImpl->is_parsed())
		if(normalExt.empty())
		{
			try
			{
				getNormal();
			}
			catch(std::runtime_error &e)
			{
				throw(e);
			}
		}

		return normalExt;
}

std::string CConfigLib::getShadowProxy(int id)
{
	std::ostringstream oss;
	oss<<getShadowProxyPath()<<getShadowProxyBaseName()<<id<<getShadowProxyExt();
	return oss.str();
}

std::string CConfigLib::getDepth(int id)
{
	std::ostringstream oss;
	oss<<getDepthPath()<<getDepthBaseName()<<id<<getDepthExt();
	return oss.str();
}

std::string CConfigLib::getNormal(int id)
{
	std::ostringstream oss;
	oss<<getNormalPath()<<getNormalBaseName()<<id<<getNormalExt();
	return oss.str();
}

std::string CConfigLib::getRoot()
{
	if(pImpl->is_parsed())
		if(root.empty())
		{
			try
			{
				vector<string> vec = pImpl->findPath("Root");
				if(vec.size()>0)
					root=vec[0];
			}
			catch(std::runtime_error &e)
			{
				if(e.what()!=string("No Root"))
					throw(e);
			}
		}

	return root;
}

int	CConfigLib::getIdObjForRendering()
{
	if(pImpl->is_parsed())
		if(idObjForRendering==-1)
		{
			try
			{
				if(!obj_isSet)
					getObj();

				vector<map<string,string>> map = pImpl->getAttributes("ObjForRendering");
				if(map.size()>0)
				{
					string sizetag = map[0]["sizeTag"];
					idObjForRendering=0;

					if(!sizetag.empty())
					{
						for(int i=0;i<obj.size();i++)
							if(obj[i].sizeTag==sizetag)
							{
								idObjForRendering=obj[i].objID;
								i=obj.size()+12;
							}
					}

				}
			}
			catch(std::runtime_error &e)
			{
					throw(e);
			}
		}

		return idObjForRendering;
}

void CConfigLib::completePath(std::string &path)
{
	if(path.size()>=4 && path[1]!=':' && (path[3]!='/' || path[3]!='\\'))
		path=getRoot()+path;
}
void CConfigLib::checkPath(std::string &path)
{
	if(path[path.size()-1]!='/' && path[path.size()-1]!='\\')
		path+="/";
}

int CConfigLib::getRenderingOption()
{
	return renderingOption;
}

const std::string ObjXml::getIdString()
{
	std::ostringstream oss;
	oss<<"_"<<objID<<"_";
	return oss.str();
}
