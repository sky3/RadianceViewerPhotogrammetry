#include "viewpointselector.h"
#include <QDir>
#include <QDebug>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QScrollBar>

ViewpointSelector::ViewpointSelector(QWidget *parent): QScrollArea(parent)
{
	QVBoxLayout *layout = new QVBoxLayout;
	selectedLab=nullptr;
	ui.setupUi(this);
	thumbnailPath="F:/Dev/BRDFStudyProject/Tools/previz/previz/JPEG";

	prefix="IMG_";
	suffix="";

	//viewIconList.reserve(infoList.size());
	setMaximumWidth(128);
	setMaximumHeight(parentWidget()->height());
	//layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

	updateThumbnails();
}

ViewpointSelector::~ViewpointSelector()
{

}

void ViewpointSelector::adaptSize()
{
	setFixedHeight(parentWidget()->height());
	setMaximumHeight(parentWidget()->height());

	//repaint();
}

void ViewpointSelector::mouseMoveEvent(QMouseEvent* e)
{
}

void ViewpointSelector::wheelEvent(QWheelEvent* e)
{
	QPoint numPixels = e->pixelDelta();
    QPoint numDegrees = e->angleDelta() / 8;

    if(!numPixels.isNull()) 
	{
		move(numPixels.y());
    } else if (!numDegrees.isNull()) {
        move(numDegrees.y());
    }

    e->accept();
}

void ViewpointSelector::move(float y)
{
	verticalScrollBar()->setValue(verticalScrollBar()->value()-y);
}

void  ViewpointSelector::selectLabel(ClickableLabel* sl)
{
	if(selectedLab)
	{
		selectedLab->setFrameStyle(QFrame::NoFrame);
		selectedLab->setStyleSheet("border: none;");
		qDebug()<<"Selected";
	}
	selectedLab=sl;
	
		qDebug()<<"Selected"<<sl->getCamId();

	selectedLab->setFrameStyle(QFrame::StyledPanel);
	selectedLab->setStyleSheet("border: 1px solid #C75670;");
}

void ViewpointSelector::update_sceneInfoAvailable(unsigned int id){
	unsigned int a=1;
	a+=id;
	qDebug()<<"Info de la scene disponible"<<id;
}

void ViewpointSelector::updateThumbnails(void){

	QDir d(thumbnailPath);
	QStringList filters;
	filters<<prefix+"*"+suffix+".jpg";
	QFileInfoList infoList = d.entryInfoList(filters,QDir::Files);

	for(int i=0;i<infoList.size();i++)
	{
		ClickableLabel *lab=new ClickableLabel(this,i);
		QPixmap map=QPixmap(infoList[i].absoluteFilePath());
		lab->setPixmap(map);
		lab->setAutoFillBackground(false);
		lab->setMinimumHeight(map.height());
		lab->setMinimumWidth(map.width());
		ui.verticalLayout->addWidget(lab);
		connect(lab, SIGNAL(click(ClickableLabel*)), this, SLOT(selectLabel(ClickableLabel*)));
		//viewIconList.push_back(lab);
	}

}