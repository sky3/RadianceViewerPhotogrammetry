#include "glwidget.h"
#include <iostream>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>

#include "embree_raytracer.h"
#include "Camera.h"
#include "LoadBundleFile.h"
#include "PatchRadiance.h"

#include"ConfigLib.h"
#include "PhotoFlyXmlManager.h"

#include <tuple>

#include "ImageBasedFittingMask.h"


#include "SABrdf.h"

float  ZNEAR =15000;
float  ZFAR  =15000;

GLWidget::GLWidget( const QGLFormat& format, QWidget* parent ): QGLWidget( format, parent ),m_vertexBuffer( QGLBuffer::VertexBuffer ),m_current_shader(CurrentShader::SHCOLOR),m_fitting_status(FittingViewerStatus::FVS_DISABLE)
{
	QObject::connect(this, SIGNAL(newAction(void)), this, SLOT(updateAction(void)));
	grabKeyboard();
	//grabMouse();
	setFocusPolicy(Qt::StrongFocus);
	rotateCursor = QCursor(QPixmap(":/previz/rotate.png"));
	moveCursor = QCursor(QPixmap(":/previz/move.png"));
	moveDCursor = QCursor(QPixmap(":/previz/moveD.png"));

	leftIsPressed=false;
	rightIsPressed=false;
	wheelIsPressed=false;
	altIsPressed=false;
	lockActionMove=false;
	position = glm::vec3( 0, 0, 5 );
	horizontalAngle = 3.14f;
	verticalAngle = 0.0f;
	initialFoV = 45.f;
	vFov = initialFoV;
	speed=0.01f;
	mouseSpeed=0.005f;

	selectionOrigine=nullptr;

	selectionBrush = QBrush(QColor("#608B4E"));
	selectionPen = QPen("white");
	selectionPen.setStyle(Qt::DashLine);
	setAutoFillBackground(false);

	imgTestCount=0;
	keySeqState = -1;
	currentAction=Action::NOACTION;

	kona = false;

	//load default color and sundirection
	suncolor = glm::vec3(1.0, 0.0, 0.0);
	sundirection = glm::vec3(0.0, 1.0, 0.0);

	/*
	Object3D obj;
	memcpy(&obj.m_vertices[0],g_vertex_buffer_data,sizeof(g_vertex_buffer_data));
	memcpy(&obj.m_normals[0],g_vertex_buffer_data,sizeof(g_vertex_buffer_data));
	*/
	//m_viz_shader.resize((unsigned int) ShaderViz::SHADER_SIZE);

}

GLWidget::~GLWidget()
{
	for(int i=0;i<objectsCollection.size();i++)
		objectsCollection[i].deleteVbo();

	glDeleteVertexArrays(1, &vertexArrayID);

	delete selectionOrigine;
}

void GLWidget::resetScene()
{
	leftIsPressed=false;
	rightIsPressed=false;
	wheelIsPressed=false;
	altIsPressed=false;
	position = glm::vec3(4,3,-3);
	lookatPosition = glm::vec3(0,0,0);
	horizontalAngle = 3.14f;
	verticalAngle = 0.0f;
	initialFoV = 45.f;
	speed=0.01f;
	mouseSpeed=0.005f;
}

void GLWidget::initializeGL()
{
	QGLFormat glFormat = QGLWidget::format();
	if ( !glFormat.sampleBuffers() )
		qWarning() << "Could not enable sample buffers";

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		qCritical()<<"Error: "<<glewGetErrorString(err);
	}
	std::cout<<"Status: Using GLEW "<<glewGetString(GLEW_VERSION)<<std::endl;

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	//if (!m_shader.LoadShaders("E:/BRDF/Tools/previz/previz/vertexShaderSample.vert", "E:/BRDF/Tools/previz/previz/fragmentShaderSample.frag"))
	//if (!m_shader.LoadShaders("../previz/vertexShaderSample.vert", "../previz/fragmentShaderSample.frag")) 
	//if (!m_shader.LoadShaders("../previz/shader_color.vert", "../previz/shader_color.frag")) {
	//	return;
	//}


	// Color Shader init.
	if (!m_shader_color.LoadShaders("../previz/shader_color.vert", "../previz/shader_color.frag")) {
		if (!m_shader_color.LoadShaders("previz/shader_color.vert", "previz/shader_color.frag")) {
			return;
		}
	}
	m_shader_color.p_matrixMVPID= m_shader_color.uniformLocation("MVP");
	if( m_shader_color.p_matrixMVPID ==-1){
		qWarning() << "Color Shader -> failed";
	}


	if (!m_shader_normal.LoadShaders("../previz/shader_normal.vert", "../previz/shader_normal.frag")) {
		if (!m_shader_normal.LoadShaders("previz/shader_normal.vert", "previz/shader_normal.frag")) {
			return;
		}
	}
	m_shader_normal.p_matrixMVPID= m_shader_normal.uniformLocation("MVP");
	if( m_shader_normal.p_matrixMVPID ==-1){
		qWarning() << "Normal Shader -> failed";
	}

	if (!m_shader_light.LoadShaders("../previz/shader_light.vert", "../previz/shader_light.frag")) {
		if (!m_shader_light.LoadShaders("previz/shader_light.vert", "previz/shader_light.frag")) {
			return;
		}
	}
	m_shader_light.p_matrixMVPID= m_shader_light.uniformLocation("MVP");
	m_shader_light.p_sundirection= m_shader_light.uniformLocation("Sundirection");

	if( m_shader_light.p_matrixMVPID ==-1 || m_shader_light.p_sundirection ==-1 ){
		qWarning() << "Light Shader -> failed";
	}

	if (!m_shader_brdf.LoadShaders("../previz/shader_brdf.vert", "../previz/shader_brdf.frag")) {
		return;
	}
	m_shader_brdf.p_matrixMVPID= m_shader_brdf.uniformLocation("MVP");
	m_shader_brdf.p_sundirection= m_shader_brdf.uniformLocation("Sundirection");
	m_shader_brdf.p_position =  m_shader_brdf.uniformLocation("Campos");

	if( m_shader_brdf.p_matrixMVPID ==-1 || m_shader_brdf.p_sundirection ==-1 ||m_shader_brdf.p_position ){
		qWarning() << "Light Shader -> failed";
	}

	float ratio = (float)width()/(float)height();

	//projection = glm::perspective(vFov,ratio, 0.1f, 100.0f);
	nearP=0.1f;
	farP=1000.f;

	computeProjection(vFov,nearP,farP);
	/*
	view       = glm::lookAt(glm::vec3(4,3,-3),glm::vec3(0,0,0),glm::vec3(0,1,0));
	model      = glm::mat4(1.0f);
	MVP        = projection*view*model;
	*/
	computeMatricesFromInputs();

	/*
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
	*/

	//m_shader.release();

}

bool GLWidget::prepareShaderProgram( const QString& vertexShaderPath,
									const QString& fragmentShaderPath )
{
	/*
	bool result = m_shader.addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
	if ( !result )
	qWarning() << m_shader.log();

	result = m_shader.addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
	if ( !result )
	qWarning() << m_shader.log();

	result = m_shader.link();
	if ( !result )
	qWarning() << "Could not link shader program:" << m_shader.log();

	return result;
	*/
	return true;
}

void GLWidget::resizeGL( int w, int h )
{
	// qreal pixelRatio = devicePixelRatio();
	computeProjection(vFov,nearP,farP);
	emit sizeChange();	
}


void GLWidget::addNewObject(std::shared_ptr<Object3D> obj)
{
	qDebug()<<"New object";
	VboObject vboObj;
	vboObj.initVBO(obj);
	Emb::createGeometry(obj);
	objectsCollection.push_back(vboObj);
	update();
}

void GLWidget::updateColorShader(){
	m_current_shader = CurrentShader::SHCOLOR;
	update();
	qDebug()<<"Update color shader";
}
void GLWidget::updateNormalShader(){
	m_current_shader = CurrentShader::SHNORMAL;
	update();
	qDebug()<<"Update normal shader";
}
void GLWidget::updateLightShader(){
	m_current_shader = CurrentShader::SHLIGHT;
	update();
	qDebug()<<"New light shader";
}
void GLWidget::updateBRDFShader(){
	m_current_shader = CurrentShader::SHBRDF;
	update();
	qDebug()<<"New viz BRDF shader";
}


void GLWidget::updateFittingEnable(){
	m_fitting_status = FittingViewerStatus::FVS_ENABLE;
	update();
	qDebug()<<"Enable Fitting BRDF";
}
void GLWidget::updateFittingDisable(){
	m_fitting_status = FittingViewerStatus::FVS_DISABLE;
	update();
	qDebug()<<"Disable Fitting BRDF";

}

void GLWidget::renderBRDFCamera(void){
	ImageBasedFittingMask test;
	test.runComputation();
	qDebug()<<"Rendering camera 32 Fitting BRDF";
}

/*
void GLWidget::paintGL()
{
/*
makeCurrent();
glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LESS); 
glEnable(GL_CULL_FACE);
glEnable(GL_MULTISAMPLE);
glBindVertexArray(vertexArrayID);
m_shader.bind();
MVP = projection*view*model;

glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &view[0][0]);
glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &model[0][0]);
glUniform3f(lightID, position.x, position.y, position.z);
glUniform3f(objColorID,0.65f,0.65f,0.65f);
for(int i=0;i<objectsCollection.size();i++)
{
objectsCollection[i].draw();
}

m_shader.unBind();
glBindVertexArray(0);
*/
//}

//rendering loop
void GLWidget::paintEvent(QPaintEvent *e)
{
	//Q_UNUSED(e)
	//updateGL();
	//makeCurrent();
	//*

	computeProjection(vFov,nearP,farP);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glEnable(GL_CULL_FACE);
	glBindVertexArray(vertexArrayID);

	MVP = projection*view*model;


	/*
	//	m_shader.bind();
	glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &model[0][0]);

	glUniform3f(lightID, position.x, position.y, position.z);
	glUniform3f(objColorID,0.65f,0.65f,0.65f);
	*/


	//select shader here..

	//bind shader + uniform

	//m_shader_color.bind();
	//glUniformMatrix4fv(m_shader_color.p_matrixMVPID, 1, GL_FALSE, &MVP[0][0]);

	//unsigned int scolor = CurrentShader::SHCOLOR;
	switch(m_current_shader)
	{
	case CurrentShader::SHNORMAL:
		m_shader_normal.bind();
		glUniformMatrix4fv(m_shader_normal.p_matrixMVPID, 1, GL_FALSE, &MVP[0][0]);
		break;

	case CurrentShader::SHCOLOR :
		m_shader_color.bind();
		glUniformMatrix4fv(m_shader_color.p_matrixMVPID, 1, GL_FALSE, &MVP[0][0]);
		break;

	case CurrentShader::SHLIGHT :
		m_shader_light.bind();
		glUniformMatrix4fv(m_shader_light.p_matrixMVPID, 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(m_shader_light.p_sundirection, sundirection[0],sundirection[1],sundirection[2]);

		break;

	case CurrentShader::SHBRDF :
		m_shader_brdf.bind();
		glUniformMatrix4fv(m_shader_brdf.p_matrixMVPID, 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(m_shader_brdf.p_sundirection, sundirection[0],sundirection[1],sundirection[2]);
		glUniform3f(m_shader_brdf.p_position, position[0],position[1],position[2]);

		break;
	default:;
	}

	for(int i=0;i<objectsCollection.size();i++)
	{
		objectsCollection[i].draw();
	}

	//unbind shader
	switch(m_current_shader)
	{
	case CurrentShader::SHNORMAL:
		m_shader_normal.unBind();
	case CurrentShader::SHCOLOR :
		m_shader_color.unBind();

	case CurrentShader::SHLIGHT :
		m_shader_light.unBind();

	case CurrentShader::SHBRDF :
		m_shader_light.unBind();

	default:;
	}


	glBindVertexArray(0);

	//glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_LIGHTING);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	drawSelection(&painter);
	painter.end();
	//*/
	//swapBuffers();
}

void GLWidget::setupViewport(int width, int height)
{
	glViewport(0, 0, width, height);
	float ratio = width/height;
	projection = glm::perspective(vFov,ratio, 0.1f, 100.0f);
}

void GLWidget::drawSelection(QPainter *painter)
{
	if(selectionOrigine && currentAction==Action::SELECT && *selectionOrigine!=mousePos[ACTUAL]){

		painter->setPen(selectionPen);
		painter->drawRect(QRect(*selectionOrigine,mousePos[ACTUAL]));

	}
}

void GLWidget::updateAction()
{
	if(altIsPressed)
	{
		if(leftIsPressed)
		{
			currentAction=Action::ROTATE;
			setCursor(rotateCursor);
			lockActionMove=true;
		}
		else if(wheelIsPressed)
		{
			currentAction=Action::TRANSLATE;
			setCursor(moveCursor);
			lockActionMove=true;
		}
		else if(rightIsPressed)
		{
			currentAction=Action::TRANSLATE_D;
			setCursor(moveDCursor);
			lockActionMove=true;
		}
		else
		{
			setCursor(Qt::ArrowCursor);
			currentAction=Action::NOACTION;
			lockActionMove=false;
		}
	}
	else
	{
		if(currentAction!=Action::NOACTION)
		{
			if(!leftIsPressed)
			{
				if(currentAction==Action::SELECT)
				{

					if(selectionOrigine && *selectionOrigine!=mousePos[ACTUAL]){

						//check bbox

						std::cout<<"Plot radiance BBox for region from :"<<selectionOrigine->x()<<" "<<selectionOrigine->y();
						std::cout<<"to :"<<mousePos[ACTUAL].x() <<" "<<mousePos[ACTUAL].y();
						std::cout<<std::endl;
						std::vector<glm::ivec2>  _listPoint;

						int min_x = std::min( mousePos[ACTUAL].x(),selectionOrigine->x());
						int max_x = std::max( mousePos[ACTUAL].x(),selectionOrigine->x());
						int min_y = std::min( mousePos[ACTUAL].y(),selectionOrigine->y());
						int max_y = std::max( mousePos[ACTUAL].y(),selectionOrigine->y());


						for ( unsigned int kx = min_x; kx <= max_x; kx++){
							for ( unsigned int ky = min_y; ky <= max_y; ky++){

								_listPoint.push_back(glm::ivec2(kx,ky));
							}
						}
						//std::thread _tutu(&GLWidget::selectPoint, this,_listPoint);

						selectPoint(_listPoint);

					}
					else{

						//for one point
						std::vector<glm::ivec2>  _listPoint;

						_listPoint.push_back(glm::ivec2(mousePos[ACTUAL].x(),mousePos[ACTUAL].y()));
						selectPoint(_listPoint);
					}


				}
				setCursor(Qt::ArrowCursor);
				currentAction=Action::NOACTION;
				lockActionMove=false;
			}
		}
		else if(leftIsPressed)
		{
			currentAction=Action::SELECT;

			selectionOrigine = new QPoint(mousePos[ACTUAL]);
		}
	}
	update();
}
void GLWidget::keyButtonDebug()
{
	qDebug()<<"Lock: "<<lockActionMove<<" Key_Alt: "<<altIsPressed<<"  RB: "<<rightIsPressed<<"  LB: "<<leftIsPressed<<"  MB: "<<wheelIsPressed;
	qDebug()<<"Nb obj: "<<objectsCollection.size();
	for(int i=0;i<objectsCollection.size();i++)
	{
		qDebug()<<objectsCollection[i].getObjectRef()->objName<<" with "<<objectsCollection[i].getnbIndices()<<" vertices";
	}
	/*
	qDebug()<<"Short "<<sizeof(short);
	qDebug()<<"Uint "<<sizeof(unsigned int);
	qDebug()<<"Float "<<sizeof(float);

	qDebug()<<"3Float "<<3*sizeof(float);
	qDebug()<<"glm::vec3 "<<sizeof(glm::vec3);
	qDebug()<<"glm::uvec3 "<<sizeof(glm::uvec3);
	qDebug()<<"glm::lowp_f32vec3 "<<sizeof(glm::lowp_f32vec3);
	*/
}


void GLWidget::computeMatricesFromInputs()
{
	direction = -glm::vec3(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle),cos(verticalAngle) * cos(horizontalAngle));
	right = glm::vec3(sin(horizontalAngle - glm::pi<float>()/2.0f), 0,cos(horizontalAngle - glm::pi<float>()/2.0f));
	up = glm::cross( right, direction );
	float dist=glm::length(lookatPosition-position);
	position=lookatPosition+dist*direction;
	view = glm::lookAt(position,lookatPosition,up);
}
bool GLWidget::moveCamera(float xMove, float yMove, bool fromWheel)
{
	if(fromWheel)
	{
		position-=direction*(xMove+yMove)*2.f*mouseSpeed;
		computeMatricesFromInputs();
		return true;
	}
	else if(lockActionMove && (leftIsPressed || rightIsPressed || wheelIsPressed))
	{
		if(leftIsPressed)
		{
			horizontalAngle += xMove*mouseSpeed;
			verticalAngle   += yMove*mouseSpeed;
		}
		else if(rightIsPressed)
		{
			position-=direction*(xMove+yMove)*2.f*mouseSpeed;
		}
		else if(wheelIsPressed)
		{
			position+=right*xMove*mouseSpeed;
			position+=up*yMove*mouseSpeed;
			lookatPosition+=right*xMove*mouseSpeed;
			lookatPosition+=up*yMove*mouseSpeed;
		}
		computeMatricesFromInputs();
		return true;
	}
	return false;
}
void GLWidget::initMousePos(QPointF p)
{
	mousePos[ACTUAL]=p.toPoint();
}

void GLWidget::emitNewAction(bool setTime)
{
	emit newAction();
}
void GLWidget::keyseq(int k)
{
	if (keySeqState != -1)
	{
		std::chrono::time_point<std::chrono::system_clock> at = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = at - lastCallSeq;
		if (elapsed_seconds.count() > 0.3)
			keySeqState = -1;
	}
	switch (k)
	{
	case Qt::Key_Up:
		if (keySeqState == -1 || keySeqState == 0)
			keySeqState++;
		else
			keySeqState = -1;
		break;
	case Qt::Key_Down:
		if (keySeqState == 1 || keySeqState == 2)
			keySeqState++;
		else
			keySeqState = -1;
		break;
	case Qt::Key_Left:
		if (keySeqState == 3 || keySeqState == 5)
			keySeqState++;
		else
			keySeqState = -1;
		break;
	case Qt::Key_Right:
		if (keySeqState == 4 || keySeqState == 6)
			keySeqState++;
		else
			keySeqState = -1;
		break;
	case Qt::Key_A:
		if (keySeqState == 8)
			endSeq();
		else
			keySeqState = -1;
		break;
	case Qt::Key_B:
		if (keySeqState == 7)
			keySeqState++;
		else
			keySeqState = -1;
		break;
	}

	lastCallSeq = std::chrono::system_clock::now();
}

void GLWidget::mouseMoveEvent(QMouseEvent* e)
{
	mousePos[PREVIOUS]=mousePos[ACTUAL];
	mousePos[ACTUAL]=e->localPos().toPoint();
	float xMove=float(mousePos[ACTUAL].x() - mousePos[PREVIOUS].x());
	float yMove=float(mousePos[ACTUAL].y() - mousePos[PREVIOUS].y());

	if(moveCamera(xMove,yMove))
		update();
	else if(currentAction==Action::SELECT)
	{
		update();
	}
}
void GLWidget::wheelEvent(QWheelEvent* e)
{
	QPoint numPixels = e->pixelDelta();
	QPoint numDegrees = e->angleDelta() / 8;

	if(!numPixels.isNull()) 
	{
		if(moveCamera(numPixels.x(),numPixels.y(),true))
			update();
	} else if (!numDegrees.isNull()) {
		if(moveCamera(numDegrees.x(),numDegrees.y(),true))
			update();
	}

	e->accept();
}
void GLWidget::mousePressEvent(QMouseEvent* e)
{
	initMousePos(e->localPos());
	switch(e->button())
	{
	case Qt::LeftButton:
		leftIsPressed=true;
		emitNewAction();
		break;
	case Qt::RightButton:
		rightIsPressed=true;
		emitNewAction();
		break;
	case Qt::MiddleButton:
		wheelIsPressed=true;
		emitNewAction();
		break;

	default:
		QGLWidget::mousePressEvent(e);
	}

}
void GLWidget::mouseReleaseEvent(QMouseEvent* e)
{
	switch (e->button())
	{
	case Qt::LeftButton:
		leftIsPressed=false;
		emitNewAction();
		break;
	case Qt::RightButton:
		rightIsPressed=false;
		emitNewAction();
		break;
	case Qt::MiddleButton:
		wheelIsPressed=false;
		emitNewAction();
		break;

	default:
		QGLWidget::mousePressEvent(e);
	}
}

void GLWidget::endSeq()
{
	qDebug() << "Konami seq";
}

void GLWidget::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key_Escape:
		QCoreApplication::instance()->quit();
		break;
	case Qt::Key_Alt:
		altIsPressed=true;
		break;
	case Qt::Key_F12:
		resetScene();
	case Qt::Key_F11:
		keyButtonDebug();
		break;
	case Qt::Key_F10:
		render();
		break;

	default:
		QGLWidget::keyPressEvent(e);
	}
}
void GLWidget::keyReleaseEvent(QKeyEvent* e)
{
	keyseq(e->key());
	switch (e->key())
	{
	case Qt::Key_Alt:
		altIsPressed=false;
		break; 

	default:
		QGLWidget::keyPressEvent(e);
	}
}

void GLWidget::selectPoint( const  std::vector<glm::ivec2>  _listPoint )
{
	glm::vec3 pixDerivX, pixDerivY, upperLeft;
	glm::vec2 coff=glm::vec2(0.0f,0.0f)*glm::vec2(width(),height());
	Emb::ComputePixelDerivatives(-direction,up,vFov*glm::pi<float>()/180.f,glm::vec2(width(),height()), coff,pixDerivX,pixDerivY,upperLeft);

	PatchRadiance _patch_test(256);
	PatchRadiance _pa_irradiancetch_test(256);
	PatchRadiance _patch_jet_map(256);
	std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>> _stack_value;

	try {


		QString window_name;
		std::vector< std::shared_ptr<Camera> >	_cam_vec =  loadBundleFileHDR(false,false);


		std::vector<double> sundir_g = CConfigLib::getSunDirection();
		glm::vec3  sundir= glm::normalize( glm::vec3(sundir_g[0],sundir_g[1], sundir_g[2]));

		std::vector<double> suncolor_g = CConfigLib::getSunColor();

		suncolor= glm::vec3(suncolor_g[0],suncolor_g[1], suncolor_g[2]);

		sundirection = sundir;

		SABrdf experiment_one_point;

		experiment_one_point.setSunDirection(sundirection);
		experiment_one_point.setSunColor(suncolor);

		//_my_envmap_adr = Envmap(CConfigLib::getEnvmap());

		
		if(!Envmap::Instance().isLoaded()){
			//load only once..
			Envmap::Instance().load(CConfigLib::getEnvmap());
		}
		

		if( _listPoint.size()  == 1){


			//buildRadianceBoxInfo(int x, y )

			//iterate over the list 
			for( unsigned int kl =0; kl < _listPoint.size() ; kl++){

				float pix_x = (float)_listPoint[kl].x;
				float pix_y = (float)_listPoint[kl].y;
				qDebug()<<pix_x<<"  "<<pix_y;
				//get acces to bundle file

				glm::vec3 directionImagePlane = pix_x * pixDerivX + pix_y * pixDerivY + upperLeft;
				directionImagePlane = glm::normalize(directionImagePlane);

				glm::vec2 uv;
				int idGeom;
				int idTriangle;
				float dist;


				if(Emb::intersect(position,directionImagePlane,1e-5f,FLT_MAX,uv,dist,idTriangle,idGeom))
				{


					//get intersect geometry
					std::shared_ptr<Object3D> obj_3 = Emb::getObject3D(idGeom);

					//just go for the 1st one... might be smarter to get the closest one or interpolate...whatever
					unsigned int id_vertex_0 = obj_3->m_indices[idTriangle*3];
					unsigned int id_vertex_1 = obj_3->m_indices[idTriangle*3+1];
					unsigned int id_vertex_2 = obj_3->m_indices[idTriangle*3+2];


					glm::vec3 color_0 = obj_3->m_color[id_vertex_0];
					glm::vec3 color_1 = obj_3->m_color[id_vertex_1];
					glm::vec3 color_2 = obj_3->m_color[id_vertex_2];

					glm::vec3 interpol_color =	(1.0f-uv.x-uv.y)*color_0 +uv.x*color_1+ uv.y*color_2;

					glm::vec3 normal_0 = glm::normalize(obj_3->m_normals[id_vertex_0]);
					glm::vec3 normal_1 = glm::normalize(obj_3->m_normals[id_vertex_1]);
					glm::vec3 normal_2 = glm::normalize(obj_3->m_normals[id_vertex_2]);

					//glm::vec3 color =	(1.0f-uv.x-uv.y)*color_0 +uv.x*color_1+ uv.y*color_2;

					glm::vec3 interpol_normal =	glm::normalize((1.0f-uv.x-uv.y)*glm::normalize(normal_0) +uv.x*glm::normalize(normal_1)+ uv.y*glm::normalize(normal_2));

					glm::mat3x3 local_frame_radiance = glm::mat3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f) ;

					embree::LinearSpace3<embree::Vec3<float>>  local_frame_embree = Emb::returnLocalFrame( embree::Vec3<float>(normal_0.x,normal_0.y,normal_0.z) );

					//float alpha = glm::dot( 


					glm::mat3x3 local_frame_irradiance =glm::inverse( 
						glm::transpose(
						glm::mat3x3(local_frame_embree.row0().x, local_frame_embree.row0().y, local_frame_embree.row0().z, 
						local_frame_embree.row1().x, local_frame_embree.row1().y, local_frame_embree.row1().z, 
						local_frame_embree.row2().x, local_frame_embree.row2().y, local_frame_embree.row2().z)));// we do not vizualize irradiance in this case

					if(obj_3->_cache_radiance_per_points.size() >0 ) { //check radiance is load... with just size of vector..

						//project the sun
						//	std::vector<double> sundir_g = CConfigLib::getSunDirection();
						//	glm::vec3  sundir= glm::normalize( glm::vec3(sundir_g[0],sundir_g[1], sundir_g[2]));

					//	_patch_test.projectOnPatch(local_frame_irradiance, normal_0, sundir, glm::vec3(65000,12000,18000),10);

						//create the radiance patch
						for ( unsigned int k=0; k < obj_3->_cache_radiance_per_points[id_vertex_0].size();k++){

							unsigned int cam_id =  obj_3->_cache_radiance_per_points[id_vertex_0][k].first;
							glm::vec3 radiance = obj_3->_cache_radiance_per_points[id_vertex_0][k].second;

							glm::vec3 normal_cam_dir =_cam_vec[PhotoFlyXmlManager::getIdBundleToCamera(cam_id)]->pos() - obj_3->m_vertices[id_vertex_0] ;
							normal_cam_dir = glm::normalize(normal_cam_dir);

							//---
							_patch_test.projectOnPatch(local_frame_irradiance,normal_0, normal_cam_dir, radiance,3);

							_stack_value.push_back(std::make_tuple(normal_0, normal_cam_dir, radiance));

							experiment_one_point.addSamples(normal_0, normal_cam_dir,radiance/65535.0f, glm::vec3(0.0,0.0,0.0));

						}

						//have fun with irradiance
						std::vector<std::pair<glm::vec3, glm::vec3>> _irradiance_dirtoColor= Emb::getIrradiancePatch(local_frame_embree, id_vertex_0, idGeom, Envmap::Instance() );

						for ( unsigned int k=0; k < _irradiance_dirtoColor.size();k++){
							_pa_irradiancetch_test.projectOnPatch(local_frame_irradiance,normal_0, glm::normalize(_irradiance_dirtoColor[k].first), _irradiance_dirtoColor[k].second,1);
						}

						//_pa_irradiancetch_test.projectOnPatch(local_frame_irradiance,normal_0, (sundir), glm::vec3(65000,12000,18000),10);

						window_name = QString("Triangle ")+QString::number(idTriangle);

						if ( m_fitting_status == FittingViewerStatus::FVS_ENABLE){
							//solve local fitting 
							//experiment_one_point.solve();

							experiment_one_point.CERES_Solve();
						}

					}
				}
				else{
					//qDebug()<<"No intersection";
				}
			}						

		}
		else if ( _listPoint.size()  > 1){

			window_name = QString("BBox ")+QString::number(_listPoint[0].x) + QString(".") +QString::number(_listPoint[0].y);
			window_name += QString(" to ")+QString::number(_listPoint[_listPoint.size()-1].x) + QString(".") +QString::number(_listPoint[_listPoint.size()-1].y);

			//iterate over the list 
			int NBTHREADS = 8;

			std::vector<SABrdf> parallel_cache;
			parallel_cache.resize(_listPoint.size());

#pragma omp parallel for num_threads(NBTHREADS) shared(_patch_test,_stack_value)
			for(  int kl =0; kl < _listPoint.size() ; kl++){

				float pix_x = (float)_listPoint[kl].x;
				float pix_y = (float)_listPoint[kl].y;
				//qDebug()<<pix_x<<"  "<<pix_y;
				//get acces to bundle files

				glm::vec3 directionImagePlane = pix_x * pixDerivX + pix_y * pixDerivY + upperLeft;
				directionImagePlane = glm::normalize(directionImagePlane);

				glm::vec2 uv;
				int idGeom;
				int idTriangle;
				float dist;


				if(Emb::intersect(position,directionImagePlane,1e-5f,FLT_MAX,uv,dist,idTriangle,idGeom))
				{

					//get intersect geometry
					std::shared_ptr<Object3D> obj_3 = Emb::getObject3D(idGeom);

					//just go for the 1st one... might be smarter to get the closest one or interpolate...whatever
					unsigned int id_vertex_0 = obj_3->m_indices[idTriangle*3];
					unsigned int id_vertex_1 = obj_3->m_indices[idTriangle*3+1];
					unsigned int id_vertex_2 = obj_3->m_indices[idTriangle*3+2];

					glm::vec3 color_0 = obj_3->m_color[id_vertex_0];
					glm::vec3 color_1 = obj_3->m_color[id_vertex_1];
					glm::vec3 color_2 = obj_3->m_color[id_vertex_2];

					glm::vec3 interpol_color =	(1.0f-uv.x-uv.y)*color_0 +uv.x*color_1+ uv.y*color_2;

					glm::vec3 normal_0 = glm::normalize(obj_3->m_normals[id_vertex_0]);
					glm::vec3 normal_1 = glm::normalize(obj_3->m_normals[id_vertex_1]);
					glm::vec3 normal_2 = glm::normalize(obj_3->m_normals[id_vertex_2]);

					//glm::vec3 color =	(1.0f-uv.x-uv.y)*color_0 +uv.x*color_1+ uv.y*color_2;

					glm::vec3 interpol_normal =	glm::normalize((1.0f-uv.x-uv.y)*glm::normalize(normal_0) +uv.x*glm::normalize(normal_1)+ uv.y*glm::normalize(normal_2));

					glm::mat3x3 local_frame_radiance = glm::mat3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f) ;

					embree::LinearSpace3<embree::Vec3<float>>  local_frame_embree = Emb::returnLocalFrame( embree::Vec3<float>(normal_0.x,normal_0.y,normal_0.z) );

					glm::mat3x3 local_frame_irradiance =glm::inverse( 
						glm::transpose(
						glm::mat3x3(local_frame_embree.row0().x, local_frame_embree.row0().y, local_frame_embree.row0().z, 
						local_frame_embree.row1().x, local_frame_embree.row1().y, local_frame_embree.row1().z, 
						local_frame_embree.row2().x, local_frame_embree.row2().y, local_frame_embree.row2().z)));// we do not vizualize irradiance in this case


					if(obj_3->_cache_radiance_per_points.size() >0 ) { //check radiance is load... with just size of vector..

						//create the radiance patch
						for ( unsigned int k=0; k < obj_3->_cache_radiance_per_points[id_vertex_0].size();k++){

							unsigned int cam_id =  obj_3->_cache_radiance_per_points[id_vertex_0][k].first;
							glm::vec3 radiance = obj_3->_cache_radiance_per_points[id_vertex_0][k].second;

							glm::vec3 normal_cam_dir =_cam_vec[PhotoFlyXmlManager::getIdBundleToCamera(cam_id)]->pos() - obj_3->m_vertices[id_vertex_0] ;
							normal_cam_dir = glm::normalize(normal_cam_dir);

							glm::vec3 _irradiance_at_P= Emb::getIrradianceAtPoint(local_frame_embree, id_vertex_0, idGeom, Envmap::Instance() );

							//---
							_patch_test.projectOnPatch(local_frame_radiance,normal_0, normal_cam_dir, radiance,3);
							//_stack_value.push_back(std::make_tuple(obj_3->m_normals[id_vertex_0], normal, radiance));

							//experiment_one_point.addSamples(interpol_normal, normal_cam_dir, radiance, _irradiance_at_P);

							
							parallel_cache[kl].addSamples(interpol_normal, normal_cam_dir, radiance/65535.0f, _irradiance_at_P/65535.0f);

							//for the 1st point only
							if( kl <1) {

								///	std::vector<double> sundir_g = CConfigLib::getSunDirection();
								///	glm::vec3  sundir= glm::normalize( glm::vec3(sundir_g[0],sundir_g[1], sundir_g[2]));

								_patch_test.projectOnPatch(local_frame_radiance,normal_0, sundir, glm::vec3(65000,12000,18000),10);

							}

						}
					}


				}
				else{
					//return;
					//qDebug()<<"No intersection";
				}


			}//end of parallel loop

			if ( m_fitting_status == FittingViewerStatus::FVS_ENABLE){
				std::cout<<"parallell pack"<<std::endl;
				for(unsigned int k=0; k< _listPoint.size(); k++){
					
					experiment_one_point = experiment_one_point+ parallel_cache[k];
				}
				std::cout<<"Ready pack"<<std::endl;
				//solve local fitting 
				//experiment_one_point.solve();

				experiment_one_point.CERES_Solve();
			}
		}

		//compute the jetmap value of point cloud
		if( _stack_value.size()> 0){

			float lumi_m= 1000000.0;
			float lumi_M= 0.0;
			std::vector<float>  _cachelum;
			_cachelum.resize(_stack_value.size() );
			//find max r,g,b and min rgb
			for ( int i=0; i < _stack_value.size(); i++){

				_cachelum[i] =std::get<2>(_stack_value[i]).r*0.33  + std::get<2>(_stack_value[i]).g*0.33  +std::get<2>(_stack_value[i]).b*0.33  ;

				if( _cachelum[i]  <lumi_m) 
					lumi_m = _cachelum[i];
				if( _cachelum[i]  > lumi_M) 
					lumi_M = _cachelum[i];

			}

			//scale value between lumi_m and lumi_M
			for ( int i=0; i < _stack_value.size(); i++){

				glm::vec3 ncolor = 65535.0f* glm::vec3(
					_cachelum[i]/(lumi_M-lumi_m ) -lumi_m/(lumi_M-lumi_m ),
					_cachelum[i]/(lumi_M-lumi_m ) -lumi_m/(lumi_M-lumi_m ),
					_cachelum[i]/(lumi_M-lumi_m ) -lumi_m/(lumi_M-lumi_m )
					);
				//_patch_jet_map.projectOnPatch(std::get<0>(_stack_value[i]),std::get<1>(_stack_value[i]),ncolor,5 ) ;
			}

		}


		//_patch_test.getGridToImage().save("test_radiance.png");
		//_pa_irradiancetch_test.getGridToImage().save("test_irradiance.png");

		_windows_distrib_vec<<new VizDistrib(window_name,512,parentWidget());
		_windows_distrib_vec[_windows_distrib_vec.size()-1]->setRadianceBox(_patch_test.getImg());
		//if (_listPoint.size()<2){
		_windows_distrib_vec[_windows_distrib_vec.size()-1]->setIrradianceBox(_pa_irradiancetch_test.getImg() /*_patch_jet_map.getImg()*/);
		_windows_distrib_vec[_windows_distrib_vec.size()-1]->setFittingBox();
		_windows_distrib_vec[_windows_distrib_vec.size()-1]->setHemisphereBox();
		//}
		_windows_distrib_vec[_windows_distrib_vec.size()-1]->show();

	}

	catch(const std::exception & e) {
		std::cout<<e.what()<<std::endl;
		return;
	}
}

void GLWidget::render()
{
	QString img("imgTest");
	img+=QString::number(imgTestCount)+".tif";
	Emb::render(img.toStdString(),glm::vec2(width(),height()),glm::vec2(0.0f,0.0f)*glm::vec2(width(),height()),vFov,objectsCollection[0].getObjectRef()->embreeID,position,-direction,up,1e-5f,FLT_MAX);
	imgTestCount++;
}

void GLWidget::computeProjection(GLdouble vFov, GLdouble nearPlane, GLdouble farPlane)
{
	GLsizei w= width();
	GLsizei h= height();

	GLdouble ratio;
	GLdouble tanHalfVAngleNear = tan(vFov*glm::pi<float>()/180.0/2.0)*nearPlane;

	if ( h>0 && w >0 ){
		ratio = w/(GLdouble)h;

		glViewport(0,0,w,h);
		//glFrustum(-ratio*tanHalfVAngleNear,ratio*tanHalfVAngleNear, -tanHalfVAngleNear, tanHalfVAngleNear ,nearPlane, farPlane);
		projection = glm::frustum(-ratio*tanHalfVAngleNear,ratio*tanHalfVAngleNear, -tanHalfVAngleNear, tanHalfVAngleNear ,nearPlane, farPlane);
	}
}

void GLWidget::removeDistrib(VizDistrib *dist)
{
	_windows_distrib_vec.removeOne(dist);
}

Scene::DistribCollection GLWidget::getAllDistrib()
{
	Scene::DistribCollection dc;

	for (VizDistrib *dist : _windows_distrib_vec)
	{
		dc << dist->getInfo();
	}

	return dc;
}

void GLWidget::addDistrib(Scene::DistribCollection dist)
{
	/*
	for (Scene::Distrib d : dist)
	{
	_windows_distrib_vec << new VizDistrib(QString("Triangle ") + QString::number(d.primID), 512, parentWidget());
	_windows_distrib_vec[_windows_distrib_vec.size() - 1]->setInfo(d.geomID, d.primID,d.u,d.v);
	_windows_distrib_vec[_windows_distrib_vec.size() - 1]->setRadianceBox();
	_windows_distrib_vec[_windows_distrib_vec.size() - 1]->setIrradianceBox();
	_windows_distrib_vec[_windows_distrib_vec.size() - 1]->setFittingBox();
	_windows_distrib_vec[_windows_distrib_vec.size() - 1]->setHemisphereBox();
	_windows_distrib_vec[_windows_distrib_vec.size() - 1]->show();
	}
	*/
}