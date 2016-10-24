#ifndef IMPORTER_H
#define IMPORTER_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QFileInfo>
#include <QProcess>
#include <object3d.h>
#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
#include <fcntl.h>
#include <iomanip>
#include <sys/stat.h>
#include <io.h>
#include <thread>
#include "DataRW.h"
#include "Scene.h"

class ConvertToBin : public QObject
 {
     Q_OBJECT
     QThread workerThread;

private:
	 QProcess *convertProcess;

public slots:
	 void convertToBin(QFileInfo fInfo);
	 void newInput();

signals:
	 void fileConverted(QString);
 };

class Importer : public QObject
{
	Q_OBJECT
	QThread workerThread;

public:
	~Importer();
	static void setParentWidget(QWidget *parent);
	static Importer* getInstance();

signals:
	void operate(QFileInfo);
	void newObjectAvailable(std::shared_ptr<Object3D>);
	void newDistribAvailable(Scene::DistribCollection dist);
	void SceneInfoAvailable(unsigned int);

public slots:
	bool import();
	bool openScene();
	bool saveScene();
	bool converted(QString);

private:
	Importer();
	static Importer *importerInstance;
	QWidget *wParent;
	void LoadFromBinary(std::string aFileName);

	template<class taData>
	void LoadBinFile(std::string aFileName, std::vector<taData> &aData)
	{
		struct __stat64 stBuf;
		if(_stat64(aFileName.c_str(), &stBuf) != 0)
			throw std::exception("Error reading input file");
		aData.resize(stBuf.st_size / sizeof(taData));
		int fh = _open(aFileName.c_str(), _O_BINARY | O_RDONLY | O_SEQUENTIAL);
		if(fh < 0)
			throw std::exception("Error reading input file");
		int size = (int)aData.size() * sizeof(taData);
		bool errRead = _read(fh, &aData[0], size) != size;
		if(_close(fh) != 0 || errRead)
			throw std::exception("Error reading input file");
	}

	static bool loadAssImp(QString path);
	void loadAssImpTh(QString path);
	std::thread *threadLoad;

	std::vector<std::shared_ptr<Object3D>> objectCollection;

	static bool loadBinSy(QString path);
	void loadBinSyTh(QString path);
	std::thread *threadLoadBin;

	Scene *pScene;
};

#endif // IMPORTER_H
