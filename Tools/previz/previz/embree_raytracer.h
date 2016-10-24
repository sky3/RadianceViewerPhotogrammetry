#ifndef EMB_H
#define EMB_H

#include <QObject>

#include <embree2\rtcore.h>
#include <embree2\rtcore_ray.h>
#include <embree2\rtcore_scene.h>

#include <object3d.h>
#include <vector>
#include <QDebug>

#include <map>
#include "Envmap.h"

class Emb: QObject
{
	Q_OBJECT

public:
	struct Vertex { float x,y,z,r; };
	struct Triangle { int v0, v1, v2; };
	struct Geometry { Vertex* vertices; Triangle *triangles;};

	~Emb(void);

	static unsigned int createGeometry(std::shared_ptr<Object3D> obj);
	static bool intersect(glm::vec3 origine,glm::vec3 direction,float aTMin, float aTMax,glm::vec2 &uv, float &aDist, int &aTriId, int &aGeomId);
	static bool isOccluded(glm::vec3 origine,glm::vec3 direction,float aTMin, float aTMax, int &aTriId, int &aGeomId);
	static void render(std::string img, glm::vec2 imgSize,glm::vec2 centerOffset,float vFov,int geomId,glm::vec3 origine,glm::vec3 direction,glm::vec3 up,float aTMin, float aTMax);
	static bool deleteGeometry(unsigned int id);
	static bool enableGeometry(unsigned int id);
	static bool disableGeometry(unsigned int id);
	static bool modifyGeometry(unsigned int id);
	static void error_handler(const RTCError code, const char* str);
	static std::string getLastError(){return instance->lastError;};
	static bool isReady(){return instance->errorOccured;};

	static glm::vec2 ComputeImagePlanePhysicalSize(float VerticalOpening, float aWidthToHeightRatio);
	static void ComputePixelDerivatives(glm::vec3 ForwardDirection, glm::vec3 UpDirection, float VerticalOpening, 
											glm::vec2 aImageResolution, glm::vec2 aImageCenterOffsetInPixels,
											glm::vec3 &aDerivX, glm::vec3 &aDerivY, glm::vec3 &aUpperLeftCorner);

	
	static std::shared_ptr<Object3D> getObject3D(unsigned int geomID);
	//static glm::uvec3 getVertexIndicesTriangle(unsigned int geomID, unsigned int idTriangle);

	static glm::vec3 getIrradianceAtPoint( embree::LinearSpace3<embree::Vec3<float>> local_frame,  unsigned int idPoint,unsigned int geomID, Envmap & _envmap);
	static  std::vector<std::pair<glm::vec3, glm::vec3>> getIrradiancePatch( embree::LinearSpace3<embree::Vec3<float>> local_frame,  unsigned int idPoint,unsigned int geomID, Envmap & _envmap);

	static  glm::vec3 Emb::cosSampleHemisphere(float u, float v);
	static float Halton(int aBase, int aSeed);

	static glm::vec3 Emb::getEnvmapColor(glm::vec3 dir,  Envmap &_envmap);

	static embree::LinearSpace3<embree::Vec3<float>> returnLocalFrame( embree::Vec3<float> _normal);

private:
	void init();
	static Emb *instance;
	RTCScene scene;
	bool errorOccured;
	std::string lastError;
	std::map<int,std::shared_ptr<Object3D>> embreeToObj;

	//std::vector<unsigned int> geomID;
	std::vector<Geometry*> geom;

	Emb(void);
};

#endif

