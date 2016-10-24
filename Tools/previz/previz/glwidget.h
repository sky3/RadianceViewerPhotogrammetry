#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "vboobject.h"
#include "object3d.h"
 
#include <QGLBuffer>
#include <QGLShaderProgram>
#include <QKeyEvent>
#include <QCoreApplication>
#include <chrono>
#include <QPushButton>
#include <QList>

#include "vizdistrib.h"

//-shaders
#include "ShaderColor.h"
#include "ShaderNormal.h"
#include "ShaderLight.h"
#include "ShaderBRDF.h"

#include"EnvMap.h"

enum History
{
    PREVIOUS,
    ACTUAL
};

enum class Action
{
    NOACTION,
    SELECT,
    ROTATE,
    TRANSLATE,
    TRANSLATE_D
};
 

enum CurrentShader{
    SHNORMAL,
    SHCOLOR,
	SHLIGHT,
	SHBRDF
};

enum FittingViewerStatus{
	FVS_ENABLE,
	FVS_DISABLE,
};

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget( const QGLFormat& format, QWidget* parent = 0 );
    ~GLWidget();

    Scene::DistribCollection getAllDistrib();
 
protected:
    virtual void initializeGL();
    virtual void resizeGL( int w, int h );
    //virtual void paintGL();

    virtual void paintEvent(QPaintEvent *e);
    //virtual void showEvent(QShowEvent *e);
 
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void keyReleaseEvent(QKeyEvent * e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void wheelEvent(QWheelEvent* e);

signals: 
    void newAction();
    void sizeChange();

public slots:
    void updateAction();
    void updateNormalShader();
    void updateColorShader();
    void updateLightShader();
	void updateBRDFShader();
	void updateFittingEnable();
	void updateFittingDisable();

	void renderBRDFCamera();
	
	void addNewObject(std::shared_ptr<Object3D> obj);
    
    void removeDistrib(VizDistrib *dist);
    void addDistrib(Scene::DistribCollection dist);

 
private:
    bool prepareShaderProgram(const QString& vertexShaderPath,const QString& fragmentShaderPath );
    void initMousePos(QPointF p);
    void computeMatricesFromInputs();
    void emitNewAction(bool setTime=false);
    void resetScene();
    void keyButtonDebug();
    bool moveCamera(float xMove, float yMove, bool fromWheel=false);
    void setupViewport(int width, int height);
    void render();
    void keyseq(int k);
    void endSeq();

    void drawSelection(QPainter *painter);
    void selectPoint(const  std::vector<glm::ivec2>  _listPoint );

	
	
	//status application UI
	
    CurrentShader m_current_shader;
	FittingViewerStatus m_fitting_status;
		
	//
    Shader m_shader;


    ShaderColor m_shader_color;
    ShaderNormal m_shader_normal;
	ShaderLight m_shader_light;
	ShaderBRDF m_shader_brdf;

    std::vector<Shader> m_viz_shader;
    QGLBuffer m_vertexBuffer;
    GLuint vertexArrayID;
    GLuint matrixID;
    GLuint lightID;
    GLuint viewMatrixID; 
    GLuint modelMatrixID;
    GLuint objColorID;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    glm::mat4 MVP;

	glm::vec3 sundirection;
	glm::vec3 suncolor;

    int keySeqState;

    GLuint vertexbuffer;
    GLuint colorbuffer;

    glm::vec3 position;
    glm::vec3 lookatPosition;
    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 up;
    float vFov;
    

    float horizontalAngle;
    float verticalAngle;
    float initialFoV;
    float nearP;
    float farP;
    bool kona;
 
    float speed;
    float mouseSpeed;
    std::chrono::time_point<std::chrono::system_clock> lastCallSeq;

    QPoint mousePos[2];
    bool altIsPressed;
    bool leftIsPressed;
    bool rightIsPressed;
    bool wheelIsPressed;
    bool lockActionMove;
    //bool actionMoveRotate;
    Action currentAction;

    QCursor rotateCursor;
    QCursor moveCursor;
    QCursor moveDCursor;
    std::chrono::time_point<std::chrono::system_clock> currentTime, lastTime;
    std::vector<VboObject> objectsCollection;

    QPoint *selectionOrigine;
    QPoint selectionEnd;

    QBrush selectionBrush;
    QPen	selectionPen;

    QList<VizDistrib*> _windows_distrib_vec;

    unsigned imgTestCount;
    void computeProjection(GLdouble vFov, GLdouble nearPlane, GLdouble farPlane);
};
 
#endif // GLWIDGET_H