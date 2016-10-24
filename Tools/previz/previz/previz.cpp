#include "previz.h"
#include <QPushButton>


Previz::Previz(QWidget *parent): QMainWindow(parent)
{
	ui.setupUi(this);
	//Message setup 
	dockOutput = new DockOutput(tr("Output"),parent);
	addDockWidget(Qt::BottomDockWidgetArea, dockOutput);
	qInstallMessageHandler(OutputWidget::logHandler);
	qRegisterMetaType<std::shared_ptr<Object3D>>();

	showMaximized();
	//Init Importer
	Importer::getInstance()->setParentWidget(this);

	createOpenglWidget();
	vpSelector = new ViewpointSelector(openglWidget);

	connect(openglWidget, SIGNAL(sizeChange()), vpSelector, SLOT(adaptSize()));

	
	//update scene info when the file is loaded
	connect(Importer::getInstance(), SIGNAL(SceneInfoAvailable(unsigned int)), vpSelector, SLOT(update_sceneInfoAvailable(unsigned int)));
		
}

void Previz::createOpenglWidget()
{
	/*
	QGLFormat glFormat;
    glFormat.setVersion( 4, 2 );
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);
	glFormat.setDoubleBuffer(true);
	*/

	openglWidget=new GLWidget(QGLFormat(QGL::SampleBuffers),this);
	//openglWidget=new GLWidget(this);
	setCentralWidget(openglWidget);
	connect(ui.actionImport, SIGNAL(triggered()), Importer::getInstance(), SLOT(import()));
	connect(ui.actionOpen_scene, SIGNAL(triggered()), Importer::getInstance(), SLOT(openScene()));
	connect(ui.actionSave_Scene, SIGNAL(triggered()), Importer::getInstance(), SLOT(saveScene()));
	connect(Importer::getInstance(), SIGNAL(newObjectAvailable(std::shared_ptr<Object3D>)), openglWidget, SLOT(addNewObject(std::shared_ptr<Object3D>)));
	connect(Importer::getInstance(), SIGNAL(newDistribAvailable(Scene::DistribCollection)), openglWidget, SLOT(addDistrib(Scene::DistribCollection)));

	//sylvain
	connect(ui.actionLight, SIGNAL(triggered()), openglWidget, SLOT(updateLightShader(void)));
	connect(ui.actionNormal, SIGNAL(triggered()), openglWidget, SLOT(updateNormalShader(void)));
	connect(ui.actionColor, SIGNAL(triggered()), openglWidget, SLOT(updateColorShader(void)));
	connect(ui.actionBRDF, SIGNAL(triggered()), openglWidget, SLOT(updateBRDFShader(void)));
	connect(ui.actionFitting_Enable, SIGNAL(triggered()), openglWidget, SLOT(updateFittingEnable(void)));
	connect(ui.actionFitting_Disable, SIGNAL(triggered()), openglWidget, SLOT(updateFittingDisable(void)));

	connect(ui.actionRenderBRDFCamera,SIGNAL(triggered()), openglWidget, SLOT(renderBRDFCamera(void)));

}

void Previz::removeDistrib(VizDistrib *dist)
{
	if (openglWidget)
	{
		openglWidget->removeDistrib(dist);
	}
}

void Previz::mousePressEvent(QMouseEvent* e)
{
}

Previz::~Previz()
{
}

Scene::DistribCollection Previz::getAllDistrib()
{
	if (openglWidget)
	{
		return openglWidget->getAllDistrib();
	}

	return Scene::DistribCollection();
}
