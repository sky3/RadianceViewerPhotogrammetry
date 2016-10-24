#ifndef SCENE_H
#define SCENE_H

#include <QVariant>

#include <QString>
#include <QList>
#include <QDataStream>

class Scene
{
	public:
		struct Distrib
		{
			Distrib(const quint32 gID = -1, const quint32 pID = -1, const qreal ucoord = 0, const qreal vcoord = 0);
			quint32 geomID;
			quint32 primID;
			qreal	u;
			qreal	v;
		};
		typedef QList<Distrib> DistribCollection;

		Scene(const QString name = "", const QString rootDir = "", const QString configLibFile="", const quint16 activeShader = 0, const QList<QString> objCollection = QList<QString>(), const DistribCollection distribCollection = DistribCollection());
		Scene(const Scene &scene);
		~Scene();

		static void initScene();

		const QString getName() { return name; };
		void setName(QString name) { this->name = name; };

		const QString getConfigLibFile() { return configLibFile; };
		void setConfigLibFile(QString configLibFile) { this->configLibFile = configLibFile; };

		const QString getRootDir() { return name; };
		void setRootDir(QString rootDir) { this->rootDir = rootDir; };

		const quint16 getActiveShader() { return activeShaderID; };
		void setActiveShader(quint16 activeShader) { this->activeShaderID = activeShader; };

		const QList<QString> getObjCollection() { return objCollection; };
		void addObj(QString obj) { objCollection << obj; };

		const DistribCollection getDistribCollection() { return distribCollection; };
		void addDistrib(Distrib dist) { distribCollection << dist; };
		void addDistribCollection(DistribCollection dist) { distribCollection << dist; };

	private:

		QString name;
		QString rootDir;
		QString configLibFile;
		QList<QString> objCollection;
		DistribCollection distribCollection;
		quint16 activeShaderID;

		friend QDataStream & operator<< (QDataStream &, const Scene::Distrib &);
		friend QDataStream & operator>> (QDataStream &, Scene::Distrib &);

		friend QDataStream & operator<< (QDataStream &, const Scene &);
		friend QDataStream & operator>> (QDataStream &, Scene &);

};

Q_DECLARE_METATYPE(Scene::Distrib)
QDataStream & operator<< (QDataStream & out, const Scene::Distrib & dist);
QDataStream & operator>> (QDataStream & in, Scene::Distrib & dist);

Q_DECLARE_METATYPE(Scene)
QDataStream & operator<< (QDataStream & out, const Scene & scene);
QDataStream & operator>> (QDataStream & in, Scene & scene);

#endif

