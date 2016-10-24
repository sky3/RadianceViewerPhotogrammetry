#include "stdafx.h"
#include "PhotoFlyXmlManager.h"

#include "XercesImpl.h"
#include "ConfigLib.h"

#include <iostream>

using namespace std;

PhotoFlyXmlManager	*PhotoFlyXmlManager::pfxManager	=	_NULLPTR_;
XercesImpl			*PhotoFlyXmlManager::pImpl		=	new XercesImpl;
std::map<int, int>	 PhotoFlyXmlManager::idBundleToCamera;

vector<Shot> PhotoFlyXmlManager::camInf=vector<Shot>();


PhotoFlyXmlManager::PhotoFlyXmlManager(void)
{
	_isload=false;
}


PhotoFlyXmlManager::~PhotoFlyXmlManager(void)
{
	if(pfxManager)
	{
	   delete pImpl;
	}
}

void PhotoFlyXmlManager::loadPhotoFlyFile(std::string file)
{
	
	if(!pfxManager)
	{
		pfxManager = new PhotoFlyXmlManager;
	}

	//CConfigLib::_assert();

	try
	{
		pImpl->Init(file);
		vector<void*> nodeList;
		vector<map<string, string> > shotData=pImpl->getAttributes("SHOT",nodeList);
		for(int i=0;i<shotData.size() && i<nodeList.size();i++)
		{
			vector<map<string, string> > cfrmData;
			vector<map<string, string> > translationData;
			vector<map<string, string> > rotationData;

			bool empty=false;

			try
			{
				cfrmData=pImpl->getAttributes("CFRM",nodeList[i]);
				translationData=pImpl->getAttributes("T",nodeList[i]);
				rotationData=pImpl->getAttributes("R",nodeList[i]);
			}
			catch(std::exception &e)
			{
				if(e.what()!=string("No CFRM"))
					throw e;
				else
					empty=true;
			}

			Shot cam;
			Cfrm cfrm;

			if(!empty)
			{
				cfrm.cf=stoi(cfrmData[0]["cf"]);
				cfrm.fovx=stod(cfrmData[0]["fovx"]);
				cfrm.rd1=stod(cfrmData[0]["rd"]);
				cfrm.rd2=stod(cfrmData[0]["rd2"]);
				cfrm.ppx=stod(cfrmData[0]["ppx"]);
				cfrm.ppy=stod(cfrmData[0]["ppy"]);
			}
			else
			{
				cfrm.cf=INT_MAX;
				cfrm.fovx=DBL_MAX;
				cfrm.rd1=DBL_MAX;
				cfrm.rd2=DBL_MAX;
				cfrm.ppx=DBL_MAX;
				cfrm.ppy=DBL_MAX;
			}

			if(!empty)
			{
				for(map<string, string>::iterator it=translationData[0].begin(); it!=translationData[0].end(); ++it)
						cfrm.t[it->first.c_str()[0]-'x']=stod(it->second);

				for(map<string, string>::iterator it=rotationData[0].begin(); it!=rotationData[0].end(); ++it)
						cfrm.r[it->first.c_str()[0]-'x']=stod(it->second);
			}
			else
				for(int j=0;j<3;j++)
				{
					cfrm.t[j]=DBL_MAX;
					cfrm.r[j]=DBL_MAX;
				}

			if(!empty)
			{
				cam.id=stoi(shotData[i]["i"]);
				cam.name=shotData[i]["n"];
			}
			else
			{
				cam.id=INT_MAX;
				cam.name="UNUSED";
			}

			string name;
			for(unsigned int j=0;j<cam.name.size();j++)
			{
				if(cam.name[j]=='.')
					j=cam.name.size();
				else
					name.push_back(cam.name[j]);}

			cam.name=name;

			cam.ci=stoi(shotData[i]["ci"]);
			cam.width=stoi(shotData[i]["w"]);
			cam.height=stoi(shotData[i]["h"]);
			cam.cfrm=cfrm;

			if(!empty)
				cam.used=true;

			camInf.push_back(cam);
		}
		pfxManager->_isload=true;
	}
	catch(const std::exception & e)
	{
		throw(e);
	}
}

void PhotoFlyXmlManager::loadPhotoFlyFile()
{
	if(!pfxManager)
	{
		pfxManager = new PhotoFlyXmlManager;
	}

	CConfigLib::_assert();

	try
	{
		pImpl->Init(CConfigLib::getPhotoFlyXmlFile());
		vector<void*> nodeList;
		vector<map<string, string> > shotData=pImpl->getAttributes("SHOT",nodeList);
		int bundleId=0;
		for(int i=0;i<shotData.size() && i<nodeList.size();i++)
		{
			vector<map<string, string> > cfrmData;
			vector<map<string, string> > translationData;
			vector<map<string, string> > rotationData;

			bool empty=false;

			try
			{
				cfrmData=pImpl->getAttributes("CFRM",nodeList[i]);
				translationData=pImpl->getAttributes("T",nodeList[i]);
				rotationData=pImpl->getAttributes("R",nodeList[i]);
			}
			catch(std::exception &e)
			{
				if(e.what()!=string("No CFRM"))
					throw e;
				else
					empty=true;
			}

			Shot cam;
			Cfrm cfrm;

			if(!empty)
			{
				cfrm.cf=stoi(cfrmData[0]["cf"]);
				cfrm.fovx=stod(cfrmData[0]["fovx"]);
				cfrm.rd1=stod(cfrmData[0]["rd"]);
				cfrm.rd2=stod(cfrmData[0]["rd2"]);
				cfrm.ppx=stod(cfrmData[0]["ppx"]);
				cfrm.ppy=stod(cfrmData[0]["ppy"]);
			}
			else
			{
				cfrm.cf=INT_MAX;
				cfrm.fovx=DBL_MAX;
				cfrm.rd1=DBL_MAX;
				cfrm.rd2=DBL_MAX;
				cfrm.ppx=DBL_MAX;
				cfrm.ppy=DBL_MAX;
			}

			if(!empty)
			{
				for(map<string, string>::iterator it=translationData[0].begin(); it!=translationData[0].end(); ++it)
						cfrm.t[it->first.c_str()[0]-'x']=stod(it->second);

				for(map<string, string>::iterator it=rotationData[0].begin(); it!=rotationData[0].end(); ++it)
						cfrm.r[it->first.c_str()[0]-'x']=stod(it->second);
			}
			else
				for(int j=0;j<3;j++)
				{
					cfrm.t[j]=DBL_MAX;
					cfrm.r[j]=DBL_MAX;
				}

			if(!empty)
			{
				cam.id=stoi(shotData[i]["i"]);
				idBundleToCamera[bundleId]=i;
				cam.name=shotData[i]["n"];
			}
			else
			{
				cam.id=INT_MAX;
				cam.name="UNUSED";
			}

			string name;
			for(unsigned int j=0;j<cam.name.size();j++)
			{
				if(cam.name[j]=='.')
					j=cam.name.size();
				else
					name.push_back(cam.name[j]);}

			cam.name=name;

			cam.ci=stoi(shotData[i]["ci"]);
			cam.width=stoi(shotData[i]["w"]);
			cam.height=stoi(shotData[i]["h"]);
			cam.cfrm=cfrm;

			if(!empty)
			{
				cam.used=true;
				bundleId++;
			}

			camInf.push_back(cam);
		}
		pfxManager->_isload=true;
	}
	catch(const std::exception & e)
	{
		throw(e);
	}
}

vector<Shot> PhotoFlyXmlManager::getCamInf()
{
	if(!pfxManager || !pImpl->is_parsed())
		loadPhotoFlyFile();
	
	return camInf;
}

int	PhotoFlyXmlManager::getIdBundleToCamera(int id)
{
	return idBundleToCamera[id];
}

Shot PhotoFlyXmlManager::getCamInf(unsigned int id)
{
	if(!pfxManager || !pImpl->is_parsed())
		loadPhotoFlyFile();

	if(id<camInf.size())
		return camInf[id];
}

int PhotoFlyXmlManager::getNbCam()
{
	if(!pfxManager || !pImpl->is_parsed())
		loadPhotoFlyFile();

	return camInf.size();
}

bool PhotoFlyXmlManager::isLoaded(){
	if (!pfxManager)
	return true;
	else return false;
}