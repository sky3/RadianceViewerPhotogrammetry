#include "vizdistrib.h"
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>

VizDistrib::VizDistrib(QString title,const unsigned int & patch_res, QWidget *parent): QDockWidget(parent),_patch_res(patch_res)
{
	ui.setupUi(this);
	setWindowTitle(title);
	setAttribute(Qt::WA_DeleteOnClose);
	connect(this, SIGNAL(closed(VizDistrib*)), parent, SLOT(removeDistrib(VizDistrib*)));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
}

VizDistrib::~VizDistrib()
{

}

void VizDistrib::closeEvent(QCloseEvent *e)
{
	emit closed(this);
	e->accept();
}

void VizDistrib::setRadianceBox(const QImage & radiance_patch){
	//QPixmap test_image_read("test_radiance.png");
	QPixmap test_image_read;
	test_image_read = QPixmap::fromImage( radiance_patch);
	

	ui.RadianceBox->setPixmap(test_image_read);
}
void VizDistrib::setIrradianceBox(const QImage & irradiance_patch){

	//QPixmap test_image_read("test_radiance.png");
	QPixmap test_image_read;
	test_image_read = QPixmap::fromImage( irradiance_patch);
	ui.IrradianceBox->setPixmap(test_image_read);

}
void VizDistrib::setFittingBox(){

	//QPixmap pixmap = QPixmap::fromImage( qimage );
	QPixmap test_image_read("../previz/Resources/hemisphereNormal.png");
	ui.FittingBox->setPixmap(test_image_read);
}
void VizDistrib::setHemisphereBox(){

	QPixmap test_image_read("../previz/Resources/hemisphereNormal.png");
	ui.HemisphereBox->setPixmap(test_image_read);
}
void VizDistrib::showContextMenu(const QPoint& pos) 
{
    QPoint globalPos = mapToGlobal(pos);
    QMenu myMenu;
    myMenu.addAction("Print");
    QAction* selectedItem = myMenu.exec(globalPos);
    if(selectedItem)
    {
		printWidget();
    }
}
void VizDistrib::printWidget()
{
	QString file = QFileDialog::getSaveFileName(this, "Save file", QString(), "Images (*.png *.tif *.jpg *.jpeg)");
	if(file.size()>1)
	{
		QPixmap pixmap(rect().size()); 
		ui.widget->render(&pixmap, QPoint(), QRegion(rect()));

		pixmap.save(file,0,100);
	}
}