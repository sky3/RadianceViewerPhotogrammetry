#ifndef PREVIZ_H
#define PREVIZ_H

#include <QtWidgets/QMainWindow>
#include "ui_previz.h"
#include "glwidget.h"
//#include "glwidgetSample.h"
#include "importer.h"
#include "outputwidget.h"
#include "dockoutput.h"
#include "viewpointselector.h"
#include <QPropertyAnimation>
#include <QPushButton>
#include <QEasingCurve>
#include <QPoint>

/*#include <CImg.h>*/
#include <ConfigLib.h>
#include "PhotoFlyXmlManager.h"


#include "common.h"
#include "Camera.h"
#include "LoadBundleFile.h"
#include "LoadPMVSPoints.h"
#include "loadOBJMesh.h"
#include "dataRW.h"
#include "MVSPoint.h"



class Previz : public QMainWindow
{
	Q_OBJECT

public:
	Previz(QWidget *parent = 0);
	~Previz();

	Scene::DistribCollection getAllDistrib();
	ViewpointSelector*  getVPSelector;

public slots:
	void removeDistrib(VizDistrib *dist);

protected:
	virtual void mousePressEvent(QMouseEvent* e);

private:
	void createOpenglWidget();
	Ui::previzClass ui;
	bool displayButton;
	GLWidget *openglWidget;
	DockOutput *dockOutput;
	OutputWidget *outW;
	QPushButton *button;
	ViewpointSelector *vpSelector;
};

#endif // PREVIZ_H
