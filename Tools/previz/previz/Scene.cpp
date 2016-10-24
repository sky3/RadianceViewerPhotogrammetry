#include "Scene.h"

#include <iostream>

Scene::Distrib::Distrib(const quint32 gID, const quint32 pID, const qreal ucoord, const qreal vcoord)
{
	geomID = gID;
	primID = pID;
	u = ucoord;
	v = vcoord;
}

Scene::Scene(const QString name, const QString rootDir, const QString configLibFile, const quint16 activeShader, const QList<QString> objCollection, const DistribCollection distribCollection)
{
	this->name = name;
	this->rootDir = rootDir;
	this->configLibFile = configLibFile;
	this->activeShaderID = activeShader;
	this->objCollection = objCollection;
	this->distribCollection = distribCollection;
}

Scene::Scene(const Scene &scene)
{
	this->name = scene.name;
	this->rootDir = scene.rootDir;
	this->activeShaderID = scene.activeShaderID;
	this->objCollection = scene.objCollection;
	this->distribCollection = scene.distribCollection;
}


Scene::~Scene()
{
}

void Scene::initScene()
{
	qRegisterMetaTypeStreamOperators<Scene::Distrib>("Scene::Distrib");
	qRegisterMetaTypeStreamOperators<Scene>("Scene");

	qMetaTypeId<Scene::Distrib>();
	qMetaTypeId<Scene>();
}

QDataStream & operator << (QDataStream & out, const Scene::Distrib & dist)
{
	out << (quint32)dist.geomID
		<< dist.primID
		<< dist.u
		<< dist.v;

	return out;
}

QDataStream & operator >> (QDataStream & in, Scene::Distrib & dist)
{
	in >> dist.geomID;
	in >> dist.primID;
	in >> dist.u;
	in >> dist.v;

	return in;
}

QDataStream & operator << (QDataStream & out, const Scene & scene)
{
	out << scene.name
		<< scene.rootDir
		<< scene.configLibFile
		<< scene.activeShaderID
		<< scene.objCollection
		<< scene.distribCollection;

	return out;
}

QDataStream & operator >> (QDataStream & in, Scene & scene)
{
	in >> scene.name;
	in >> scene.rootDir;
	in >> scene.configLibFile;
	in >> scene.activeShaderID;
	in >> scene.objCollection;
	in >> scene.distribCollection;

	return in;
}
