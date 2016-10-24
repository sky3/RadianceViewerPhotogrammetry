#include "importer.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QStringList>
#include <QDir>
#include <QDebug>
#include <iostream>
#include <QRegularExpression>
#include <regex>
#include <assimp/Importer.hpp>  
#include <assimp/scene.h>       
#include <assimp/postprocess.h> 
#include <stdio.h>
#include <algorithm>
#include <QFile>
#include <QSettings>

#include "previz.h"

//bool sortfunction (float i,float j) { return (i<j); }

Importer *Importer::importerInstance=new Importer();

Importer::Importer():wParent(nullptr)
{
	Scene::initScene();
	pScene = nullptr;
}

Importer::~Importer()
{

}

void Importer::setParentWidget(QWidget *parent)
{
	importerInstance->wParent=parent;
	importerInstance->setParent(parent);
}

Importer* Importer::getInstance()
{
	return importerInstance;
}

bool Importer::loadAssImp(QString path)
{
	QFileInfo inf(path);
	std::shared_ptr<Object3D> obj(new Object3D());
	obj->setReadyForVbo(true);
	std::vector<glm::vec2> uvs;
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path.toStdString().c_str(), aiProcess_JoinIdenticalVertices /*| aiProcess_SortByPType*/);
	if( !scene) {
		fprintf( stderr, importer.GetErrorString());
		getchar();
		return false;
	}
	const aiMesh* mesh = scene->mMeshes[0]; // In this simple example code we always use the 1rst mesh (in OBJ files there is often only one anyway)

	obj->m_vertices.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		aiVector3D pos = mesh->mVertices[i];
		obj->m_vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
	}
	/*
	uvs.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
		uvs.push_back(glm::vec2(UVW.x, UVW.y));
	}
	*/

	obj->m_normals.reserve(mesh->mNumVertices);
	obj->m_color.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		aiVector3D n = mesh->mNormals[i];
		obj->m_normals.push_back(glm::vec3(n.x, n.y, n.z));
		obj->m_color.push_back(0.5f*(glm::vec3(n.x, n.y, n.z) +glm::vec3(1.0))*65535.0f);
	}

	obj->nbTriangles=mesh->mNumFaces;
	obj->m_indices.reserve(3*mesh->mNumFaces);
	for (unsigned int i=0; i<mesh->mNumFaces; i++){
		// Assume the model has only triangles.
		obj->m_indices.push_back(mesh->mFaces[i].mIndices[0]);
		obj->m_indices.push_back(mesh->mFaces[i].mIndices[1]);
		obj->m_indices.push_back(mesh->mFaces[i].mIndices[2]);
	}
	obj->objName=inf.baseName();
	obj->filePath = path;
	Importer::getInstance()->objectCollection.push_back(obj);
	emit Importer::getInstance()->newObjectAvailable(obj);
	return true;
}
bool Importer::loadBinSy(QString path)
{
	QFileInfo inf(path);
	std::shared_ptr<Object3D> obj(new Object3D());
	obj->setReadyForVbo(true);
	
	std::vector<glm::uvec3> tmpIndex = dataRW::readIndexTriangleArray((inf.absolutePath()+"/mesh_transfer_index.bin").toStdString());
	obj->m_indices.resize(tmpIndex.size()*3);
	std::memcpy(&obj->m_indices[0],tmpIndex.data(),sizeof(int)*tmpIndex.size()*3);

	obj->m_normals=dataRW::readNormalArray((inf.absolutePath()+"/mesh_transfer_normal.bin").toStdString());
	obj->m_vertices=dataRW::readPositionArray((inf.absolutePath()+"/mesh_vertices.bin").toStdString());

	std::ifstream infile((inf.absolutePath()+"/have_fun_radiance.bin").toStdString());

	std::ifstream infile_mediane((inf.absolutePath()+"/mediane_radiance.bin").toStdString());
	if ( infile.good() && infile_mediane.good() ){

		//file exist.. so load data
		obj->_cache_radiance_per_points = dataRW::readRadiancePerPoints((inf.absolutePath()+"/have_fun_radiance.bin").toStdString());
		obj->m_color = dataRW::readGlmVec3Array((inf.absolutePath()+"/mediane_radiance.bin").toStdString());

	}
	else if(infile.good()) {
		//file exist.. so load data
		obj->_cache_radiance_per_points = dataRW::readRadiancePerPoints((inf.absolutePath()+"/have_fun_radiance.bin").toStdString());

		//compute the mediane for all points to attribute a color 
		obj->m_color.resize(obj->_cache_radiance_per_points.size());

			std::vector<float> r_channel;
			std::vector<float> g_channel;
			std::vector<float> b_channel;

		for ( unsigned int k=0; k< obj->m_color.size(); k++){

			r_channel.clear();
			g_channel.clear();
			b_channel.clear();

			for(unsigned int kc = 0; kc < obj->_cache_radiance_per_points[k].size();kc++){
				r_channel.push_back(obj->_cache_radiance_per_points[k][kc].second.r);
				g_channel.push_back(obj->_cache_radiance_per_points[k][kc].second.g);
				b_channel.push_back(obj->_cache_radiance_per_points[k][kc].second.b);
			}

			std::sort(r_channel.begin(), r_channel.end());
			std::sort(g_channel.begin(), g_channel.end());
			std::sort(b_channel.begin(), b_channel.end());

			if ( r_channel.size() >0){
			obj->m_color[k].r = r_channel[r_channel.size()/2];
			obj->m_color[k].g = g_channel[g_channel.size()/2];
			obj->m_color[k].b = b_channel[b_channel.size()/2];
			}
			
		}
		//write the file after computing mediane for fast load next time..
		dataRW::writeGlmVec3Array(obj->m_color,(inf.absolutePath()+"/mediane_radiance.bin").toStdString());

	}
	else{
		//put normal in color layers...

		for ( unsigned int k=0; k< obj->m_normals.size(); k++){
			obj->m_color[k] = (obj->m_normals[k] +glm::vec3(1.0))*0.5f*65535.0f;
		}
	}


	obj->nbTriangles=tmpIndex.size();//obj->m_vertices.size()/3.f;
	obj->objName=inf.baseName();
	obj->filePath = path;
	Importer::getInstance()->objectCollection.push_back(obj);
	emit Importer::getInstance()->newObjectAvailable(obj);

	return true;
}
void Importer::loadAssImpTh(QString path)
{
	threadLoad = new std::thread(loadAssImp,path);
}
void Importer::loadBinSyTh(QString path)
{
	threadLoadBin = new std::thread(loadBinSy,path);
}

void ConvertToBin::convertToBin(QFileInfo fInfo)
{
	QString prog = "obj2bin.exe";
	QDir dir=fInfo.absoluteDir();
	if(dir.mkdir(fInfo.baseName()+"_binaryObj"))
	{
		convertProcess = new QProcess(this);
		//QObject::connect(convertProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(newInput()));
		QStringList arguments;
		arguments<<fInfo.absoluteFilePath()<<fInfo.absolutePath()+"/"+fInfo.baseName()+"_binaryObj/"+fInfo.baseName();
		convertProcess->start(prog,arguments);
		convertProcess->waitForFinished();
		emit fileConverted(fInfo.absolutePath()+"/"+fInfo.baseName()+"_binaryObj/"+fInfo.baseName());
	}
	else
	{
		qWarning()<<"mkdir error: "<<fInfo.baseName()+"_binaryObj";
	}
}

void ConvertToBin::newInput()
{
	QByteArray byteArray = convertProcess->readAllStandardOutput();
	QString input=QString(byteArray);
	input.replace(QRegularExpression(QRegularExpression::escape("\r")),"\n");
	std::cout<<input.toStdString();;
}

bool Importer::import()
{
	QString file = QFileDialog::getOpenFileName(wParent, "Open file", QString(), "3D (*)");
	QFileInfo fInfo(file);
	if(fInfo.suffix()=="obj")
	{
		loadAssImpTh(file);
	}
	else if(fInfo.suffix()=="xml")
	{
		//LoadFromBinary(file.toStdString());
		try
		{
			CConfigLib::loadConfigFile(file.toStdString());
			if (!pScene)
				pScene = new Scene();

			pScene->setConfigLibFile(file);

			emit Importer::getInstance()->SceneInfoAvailable(1);
		}
		catch (const std::exception &e)
		{
			qWarning() << e.what();
		}
	}
	else if(fInfo.suffix()=="bin")
	{
		loadBinSyTh(file);
	}
	return false;
}

bool Importer::converted(QString file)
{
	std::cout<<"Convertion done"<<std::endl;
	LoadFromBinary((file+".xml").toStdString());
	return true;
}

void Importer::LoadFromBinary(std::string aFileName)
{
	std::cout<<"Importing "<<aFileName<<std::endl;
	std::string normfn = std::regex_replace(aFileName, std::regex("\\.xml$"), ".norm.bin");
	std::string vertfn = std::regex_replace(aFileName, std::regex("\\.xml$"), ".vert.bin");
	std::string idxfn = std::regex_replace(aFileName, std::regex("\\.xml$"), ".idxs.bin");

	std::vector<int> idxs;

	QFileInfo inf(aFileName.c_str());
	std::shared_ptr<Object3D> obj(new Object3D());
	obj->setReadyForVbo(false);
	obj->objName=inf.baseName();
	
	LoadBinFile(idxfn, idxs);
	LoadBinFile(vertfn, obj->m_vertices);
	LoadBinFile(normfn, obj->m_normals);

	for(int idxOffs = 0; idxOffs < idxs.size(); idxOffs += 10)
	{
		obj->m_indices.push_back(idxs[idxOffs + 0]);
		obj->m_indices.push_back(idxs[idxOffs + 1]);
		obj->m_indices.push_back(idxs[idxOffs + 2]);
	}
	std::cout<<"Import of "<<aFileName<<" done"<<std::endl;
	objectCollection.push_back(obj);
	emit newObjectAvailable(obj);
}

bool Importer::openScene()
{
	int ret = QMessageBox::question(wParent, "Replacement", "The active scene will be replace", QMessageBox::Yes | QMessageBox::No);
	if (ret == QMessageBox::Yes)
	{
		QString file = QFileDialog::getOpenFileName(wParent, "Open Scene", QString(), "Ini (*.ini)");
		delete pScene;
		pScene = new Scene();
		QSettings iniFile(file, QSettings::IniFormat);
		*pScene = iniFile.value("Scene", qVariantFromValue(Scene())).value<Scene>();

		if (pScene->getConfigLibFile().size()>1)
			try
			{
				CConfigLib::loadConfigFile(file.toStdString());
				if (!pScene)
					pScene = new Scene();

				pScene->setConfigLibFile(file);
			}
			catch (const std::exception &e)
			{
				qWarning() << e.what();
			}

			for (QString obj : pScene->getObjCollection())
			{
				loadAssImpTh(obj);
			}

			emit newDistribAvailable(pScene->getDistribCollection());


		return true;
	}

	return false;
}

bool Importer::saveScene()
{
	QString file = QFileDialog::getSaveFileName(wParent, "Save scene", QString(), "Ini (*.ini)");
	QFile sceneFile(file);
	bool saveFile = false;
	if (sceneFile.exists())
	{
		sceneFile.remove();
	}

	if (!pScene)
		pScene = new Scene();

	pScene->setName(sceneFile.fileName());

	if (wParent)
		pScene->addDistribCollection(((Previz*)wParent)->getAllDistrib());

	for(std::shared_ptr<Object3D> objIt : objectCollection)
	{
		pScene->addObj(objIt->filePath);
	}

	QSettings iniFile(file, QSettings::IniFormat);
	iniFile.setValue("Scene", qVariantFromValue(*pScene));
	iniFile.sync();

	return true;
}
