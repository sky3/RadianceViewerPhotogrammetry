#ifndef VIEWPOINTSELECTOR_H
#define VIEWPOINTSELECTOR_H

#include <QScrollArea>
#include <QVector>
#include <QLabel>
#include "ui_viewpointselector.h"
#include "clickablelabel.h"
#include <QGraphicsOpacityEffect> 

class ViewpointSelector : public QScrollArea
{
	Q_OBJECT

public:
	ViewpointSelector(QWidget *parent = 0);
	~ViewpointSelector();

	void updateThumbnails(void);
protected:
	virtual void mouseMoveEvent(QMouseEvent* e);
	virtual void wheelEvent(QWheelEvent* e);


public slots:
	void adaptSize();
	void selectLabel(ClickableLabel* sl);
	void update_sceneInfoAvailable(unsigned int id);


private:
	void move(float y);
	Ui::ViewpointSelector ui;
	QString thumbnailPath;
	QString prefix;
	QString suffix;
	//QVector<QLabel*> viewIconList;
	QPoint actualPos;
	QPoint previousPos;
	ClickableLabel *selectedLab;
};

#endif // VIEWPOINTSELECTOR_H
