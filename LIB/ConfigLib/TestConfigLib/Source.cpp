#include <iostream>
#include <sstream>

#include <ConfigLib.h>
#include <PhotoFlyXmlManager.h>
#include <PipelineManager.h>
#include <iostream>
#include <windows.h>

#define LIB_TEST

using namespace std;
HANDLE  hConsole;

void boolTorString(bool val)
{
	if(val)
	{
		SetConsoleTextAttribute(hConsole, 10);
		cout<<"true";
	}
	else
	{
		SetConsoleTextAttribute(hConsole, 12);
		cout<<"false";
	}

	SetConsoleTextAttribute(hConsole, 7);
}
void testVector(vector<double> tab)
{
	bool ret=true;
	double val=-0.5;
	for(int i=0;i<tab.size();i++)
	{
		if(tab[i]!=val)
			ret=false;
		
		val+=0.5;
	}
	boolTorString(ret);
}
void testSet(vector<unsigned int> tab)
{
	bool ret;
	if(tab.size()>0)
	{

		for(int i=0;i<tab.size() && i<=5;i++)
			if(tab[i]!=i)
				ret=false;
	
		if(tab.size()>=6 && tab[6]!=8)
				ret=false;

		for(int i=7;i<tab.size();i++)
			if(tab[i]!=i+8)
				ret=false;

		ret=true;
	}
	else
		ret=false;

	boolTorString(ret);
}
void main(int arc, char **argv)
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	try
	{
		cout<<"Loading xml sample file"<<endl<<endl;
		CConfigLib::option(arc,argv);
		CConfigLib::_assert();
		cout<<"Test render: ";boolTorString(CConfigLib::getRenderingOption()==0);cout<<endl;
		cout<<"Test Name: ";boolTorString(CConfigLib::getName()=="testName");cout<<endl<<endl;

		cout<<"Test PngPath: ";boolTorString(CConfigLib::getPngPath(false,false)=="E:\\pathPng\\pathPng/pathPng\\");cout<<endl;
		cout<<"Test JpgPath: ";boolTorString(CConfigLib::getJpgPath(false,false)=="E:\\pathJpg/");cout<<"\tJpgExt: ";boolTorString(CConfigLib::getJpgExt(false)==".jpg");cout<<endl;
		cout<<"Test TifPath: ";boolTorString(CConfigLib::getTifPath(false,false)=="E:\\pathTif/");cout<<"\tTifExt: ";boolTorString(CConfigLib::getTifExt(false)==".tiff");cout<<endl;
		cout<<"Test RawPath: ";boolTorString(CConfigLib::getRawPath()=="E:\\pathRaw/");cout<<"\tRawExt: ";boolTorString(CConfigLib::getRawExt()==".CR2");cout<<endl<<endl;

		cout<<"Test PngUndistPath: ";boolTorString(CConfigLib::getPngPath(true,false)=="E:\\pathPng_Undist/");cout<<endl;
		cout<<"Test JpgUndistPath: ";boolTorString(CConfigLib::getJpgPath(true,false)=="E:\\pathJpg_Undist/");cout<<"\tJpgUndistExt: ";boolTorString(CConfigLib::getJpgExt()==".jpg");cout<<endl;
		cout<<"Test TifUndistPath: ";boolTorString(CConfigLib::getTifPath(true,false)=="E:\\pathTif_undist/");cout<<"\tTifUndistExt: ";boolTorString(CConfigLib::getTifExt()==".tiff");cout<<endl<<endl;

		cout<<"Test shadowProxyBaseName: ";boolTorString(CConfigLib::getShadowProxyBaseName()=="E:\\baseNameShadowProxy");cout<<"\tExtShadowProxy: ";boolTorString(CConfigLib::getShadowProxyExt()==".png");cout<<endl;
		cout<<"Test DepthBaseName: ";boolTorString(CConfigLib::getDepthBaseName()=="E:\\baseNameDepth");cout<<"\tExtdepth: ";boolTorString(CConfigLib::getDepthExt()==".tif");cout<<endl;
		cout<<"Test NormalBaseName: ";boolTorString(CConfigLib::getNormalBaseName()=="E:\\baseNameNormal");cout<<"\tExtNorm: ";boolTorString(CConfigLib::getDepthExt()==".tif");cout<<endl;

		cout<<"Test Patch: ";boolTorString(CConfigLib::getPatch()=="E:\\pathPatch");cout<<endl;
		cout<<"Test Bundle: ";boolTorString(CConfigLib::getBundle()=="E:\\pathBundle");cout<<endl;
		for(int i=0;i<CConfigLib::getObj().size();i++)
		{
			std::ostringstream ossPath;
			std::ostringstream ossSizeTag;
			ossPath<<"E:\\pathObj"<<i;
			ossSizeTag<<"sizeTag"<<i;
			cout<<"Test Obj path"<<i<<": ";boolTorString(CConfigLib::getObj()[i].objPath==ossPath.str());cout<<" sizeTag: ";boolTorString(CConfigLib::getObj()[i].sizeTag==ossSizeTag.str());cout<<endl;
		}
		cout<<"Test idObjForRendering: ";boolTorString(CConfigLib::getIdObjForRendering()==1);cout<<endl<<endl;
		cout<<"Test Photofly: ";boolTorString(CConfigLib::getPhotoFlyXmlFile()=="E:\\PhotoFlySample.xml");cout<<endl<<endl;

		cout<<"Test Pmvs: ";boolTorString(CConfigLib::getPmvsPath(false)=="E:\\pathPmvs/");cout<<" name: ";boolTorString(CConfigLib::getPmvsName()=="namePmvs");cout<<" ext: ";boolTorString(CConfigLib::getPmvsExt()==".txt");cout<<endl;
		cout<<"Test Spmvs: ";boolTorString(CConfigLib::getSpmvsPath(false)=="E:\\pathSpmvs/");cout<<" name: ";boolTorString(CConfigLib::getSpmvsName()=="nameSpmvs");cout<<" ext: ";boolTorString(CConfigLib::getSpmvsExt()==".txt");cout<<endl;
		cout<<"Test Sobj: ";boolTorString(CConfigLib::getSobjPath(false)=="E:\\pathSobj/");cout<<" name: ";boolTorString(CConfigLib::getSobjName()=="nameSobj");cout<<" ext: ";boolTorString(CConfigLib::getSobjExt()==".obj");cout<<endl<<endl;

		cout<<"Test Envmap: ";boolTorString(CConfigLib::getEnvmap()=="E:\\pathEnvmap");cout<<endl;
		cout<<"Test EnvmapSky: ";boolTorString(CConfigLib::getEnvmapSky()=="E:\\pathEnvmapSky");cout<<endl<<endl;

		cout<<"Test Downscale: ";boolTorString(CConfigLib::getDownScale()==1.0);cout<<endl;
		cout<<"Test WorldRotation: ";testVector(CConfigLib::getWorldRotation());cout<<endl;
		cout<<"Test WorldTranslation: ";testVector(CConfigLib::getWorldTranslation());cout<<endl;
		cout<<"Test SunDir: ";testVector(CConfigLib::getSunDirection());cout<<endl;

		cout<<"Test set: ";testSet(CConfigLib::getImagesSet());cout<<endl;
		cout<<"Test sample: ";boolTorString(CConfigLib::getSample()==1024);cout<<endl;
		cout<<"Test outsuncalib: ";boolTorString(CConfigLib::getOutSuncalib()=="E:\\pathOutSunCalib/");cout<<endl<<endl<<endl;

		cout<<"Intrinsic"<<endl;
	
		cout<<"Pipeline manager"<<endl;
		char *argu[2];
		argu[0]=new char[255];
		argu[1]=new char[255];
		argu[0]="-xml";
		argu[1]="Pipeline.xml";

		PipelineManager::option(2,argu);

		for(int i=0;i<PipelineManager::getNbDataset();i++)
		{
			ostringstream oss;
			oss<<"dataset"<<i+1<<".xml";
			cout<<"Test "<<oss.str()<<": ";boolTorString(PipelineManager::getDatasetConfigFile(i)==oss.str());cout<<"\tStep: ";boolTorString(PipelineManager::getDatasetSteps(i)[0]==i);cout<<endl;
		}



		PhotoFlyXmlManager::loadPhotoFlyFile("PhotoFlySample.xml");
		



	}
	catch(std::runtime_error &e)
	{
		std::cout<<e.what()<<std::endl;
	}
	getchar();
}