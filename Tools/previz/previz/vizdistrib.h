#ifndef VIZDISTRIB_H
#define VIZDISTRIB_H


#include <QDockWidget>
#include "ui_vizdistrib.h"
#include <string>
#include <QMenu>
#include "importer.h"
#include <qimage.h>
class VizDistrib : public QDockWidget
{
	Q_OBJECT

public:
	VizDistrib(QString title, const unsigned int & patch_res, QWidget *parent = 0);
	~VizDistrib();

	void setRadianceBox(const QImage&  radiance_patch);
	void setIrradianceBox(const QImage & irradiance_patch);
	void setFittingBox();
	void setHemisphereBox();

	void setInfo(quint32 geomID, quint32 primID, qreal u, qreal v){	this->geomID = geomID;
																	this->primID = primID;
																	this->u = u;
																	this->v = v; };

	Scene::Distrib getInfo(){ return Scene::Distrib(geomID, primID, u, v); };

protected:
	virtual void closeEvent(QCloseEvent *e);

signals:
	void closed(VizDistrib*);

private slots:
	void printWidget();
	void showContextMenu(const QPoint&);

private:
	Ui::VizDistrib ui;
	unsigned int _patch_res;
	QMenu* contextMenu;

	quint32	geomID;
	quint32	primID;
	qreal	u;
	qreal	v;
};

#endif // VIZDISTRIB_H
