#include "embree_raytracer.h"
#include <iostream>
#include <FreeImage.h>
#include <chrono>
#include <math.h>
#include <Windows.h>
#include <QVector3D>
#include <QVector2D>
#include <math/vec3.h>
#include <math/linearspace3.h> 
#include <glm/gtc/constants.hpp>
#include "EnvMap.h"

Emb *Emb::instance=new Emb();

Emb::Emb(void)
{
	errorOccured=false;
	rtcInit();
	rtcSetErrorFunction(error_handler);
	scene=nullptr;
}

void  Emb::init()
{
	scene=rtcNewScene(RTC_SCENE_DYNAMIC,RTC_INTERSECT1);
}

void Emb::error_handler(const RTCError code, const char* str)
{
	instance->lastError="Embree: ";
	switch (code) 
	{
	case RTC_UNKNOWN_ERROR    : instance->lastError+="RTC_UNKNOWN_ERROR"; break;
	case RTC_INVALID_ARGUMENT : instance->lastError+="RTC_INVALID_ARGUMENT"; break;
	case RTC_INVALID_OPERATION: instance->lastError+="RTC_INVALID_OPERATION"; break;
	case RTC_OUT_OF_MEMORY    : instance->lastError+="RTC_OUT_OF_MEMORY"; break;
	case RTC_UNSUPPORTED_CPU  : instance->lastError+="RTC_UNSUPPORTED_CPU"; break;
	default                   : instance->lastError+="invalid error code"; break;
	}
	if(str) 
	{ 
		instance->lastError+="\n";
		instance->lastError+=std::string(str);
	}
	qWarning()<<instance->lastError.c_str();
	instance->errorOccured=true;
}


Emb::~Emb(void)
{
	rtcDeleteScene(scene);
	rtcExit();
}

unsigned int Emb::createGeometry(std::shared_ptr<Object3D> obj)
{
	if(instance)
	{
		if(!instance->scene)
			instance->init();

		unsigned int id=rtcNewTriangleMesh(instance->scene,RTC_GEOMETRY_STATIC,obj->nbTriangles,obj->m_vertices.size(),1);
		//instance->geomID.push_back(id);
		Geometry *g=new Geometry;
		g->vertices=nullptr;
		g->triangles=nullptr;

		g->vertices = (Vertex*) rtcMapBuffer(instance->scene,id,RTC_VERTEX_BUFFER);
		if(obj->vertexSize==3)
		{
			for(int i=0;i<obj->m_vertices.size();i++)
			{
				g->vertices[i].x=obj->m_vertices[i].x;
				g->vertices[i].y=obj->m_vertices[i].y;
				g->vertices[i].z=obj->m_vertices[i].z;
				g->vertices[i].r=1.f;
			}
		}
		else
		{
			std::memcpy(g->vertices,obj->m_vertices.data(),sizeof(glm::vec3)*obj->m_vertices.size());
		}
		rtcUnmapBuffer(instance->scene,id,RTC_VERTEX_BUFFER);

		g->triangles = (Triangle*) rtcMapBuffer(instance->scene,id,RTC_INDEX_BUFFER);
		std::memcpy(g->triangles,obj->m_indices.data(),sizeof(int)*obj->m_indices.size());
		rtcUnmapBuffer(instance->scene,id,RTC_INDEX_BUFFER);
		rtcCommit(instance->scene);
		obj->embreeID=id;
		instance->embreeToObj[id]=obj;
		instance->geom.push_back(g);

		return id;
	}
	obj->embreeID=-1;
	return -1;
}

bool Emb::deleteGeometry(unsigned int id)
{
	if(instance->scene)
	{
		rtcDeleteGeometry(instance->scene,id);
		rtcCommit(instance->scene);
		return !instance->errorOccured;
	}

	return false;
}
bool Emb::enableGeometry(unsigned int id)
{
	if(instance->scene)
	{
		rtcEnable(instance->scene,id);
		rtcCommit(instance->scene);
		return !instance->errorOccured;
	}

	return false;
}
bool Emb::disableGeometry(unsigned int id)
{
	if(instance->scene)
	{
		rtcDisable(instance->scene,id);
		rtcCommit(instance->scene);
		return !instance->errorOccured;
	}

	return false;
}
bool Emb::modifyGeometry(unsigned int id)
{
	return !instance->errorOccured;
}

bool Emb::intersect(glm::vec3 origine,glm::vec3 direction,float aTMin, float aTMax,glm::vec2 &uv, float &aDist, int &aTriId, int &aGeomId)
{
	if(instance->scene)
	{
		bool intersect = false;
		
		RTCRay ray;
		ray.org[0] = origine.x;
		ray.org[1] = origine.y;
		ray.org[2] = origine.z;
		ray.dir[0] = direction.x;
		ray.dir[1] = direction.y;
		ray.dir[2] = direction.z;
		ray.tnear = aTMin;
		ray.tfar = aTMax;
		ray.geomID = RTC_INVALID_GEOMETRY_ID;
		ray.primID = RTC_INVALID_GEOMETRY_ID;
		ray.mask = -1;
		ray.time = 0;

		// intersect ray with scene 
		rtcIntersect(instance->scene,ray);
		aGeomId = ray.geomID;
		aTriId = ray.primID;
		uv.x=ray.u;
		uv.y=ray.v;
		
		if(aGeomId!=RTC_INVALID_GEOMETRY_ID && aTriId!=RTC_INVALID_GEOMETRY_ID)
			intersect=true;
		
		return !instance->errorOccured & intersect;
	}

	return false;
}
bool Emb::isOccluded(glm::vec3 origine,glm::vec3 direction,float aTMin, float aTMax, int &aTriId, int &aGeomId)
{
	if(instance->scene)
	{
		bool intersect = false;
		RTCRay ray;
		ray.org[0] = origine.x;
		ray.org[1] = origine.y;
		ray.org[2] = origine.z;
		ray.dir[0] = direction.x;
		ray.dir[1] = direction.y;
		ray.dir[2] = direction.z;
		ray.tnear = aTMin;
		ray.tfar = aTMax;
		ray.geomID = RTC_INVALID_GEOMETRY_ID;
		ray.primID = RTC_INVALID_GEOMETRY_ID;
		ray.mask = -1;
		ray.time = 0;

		/* intersect ray with scene */
		rtcIntersect(instance->scene,ray);
		aGeomId = ray.primID;
		aTriId = ray.primID;


		if(aGeomId!=RTC_INVALID_GEOMETRY_ID && aTriId!=RTC_INVALID_GEOMETRY_ID)
			intersect=true;

		return !instance->errorOccured & intersect;
	}

	return false;
}

glm::vec2  Emb::ComputeImagePlanePhysicalSize(float VerticalOpening, float aWidthToHeightRatio)
{
	float angleTan = tanf(VerticalOpening / 2.0f);
	return glm::vec2 (2 * angleTan * aWidthToHeightRatio, 2 * angleTan);
}

void  Emb::ComputePixelDerivatives(
	glm::vec3 ForwardDirection, glm::vec3 UpDirection, float VerticalOpening, 
	glm::vec2 aImageResolution, glm::vec2 aImageCenterOffsetInPixels,glm::vec3 &aDerivX, glm::vec3 &aDerivY, glm::vec3 &aUpperLeftCorner)
{
	ForwardDirection = glm::normalize(ForwardDirection);
	UpDirection =glm::normalize(UpDirection);

	float aspectRatio = aImageResolution.x / (float)aImageResolution.y;
	glm::vec2 imageSize = ComputeImagePlanePhysicalSize(VerticalOpening, aspectRatio);
	glm::vec3 forwardAxis = ForwardDirection;
	glm::vec3 rightAxis = -glm::cross(UpDirection, ForwardDirection);
	glm::vec3 upAxis = -glm::cross(rightAxis, forwardAxis);
	glm::vec3 rowVector = rightAxis * imageSize.x;
	glm::vec3 colVector = upAxis * imageSize.y;
	aDerivX = rowVector / (float)aImageResolution.x;
	aDerivY = colVector / (float)aImageResolution.y;
	aUpperLeftCorner = forwardAxis - rowVector / 2.0f - colVector / 2.0f;
	aUpperLeftCorner += aDerivX * (float)aImageCenterOffsetInPixels.x;
	aUpperLeftCorner += aDerivY * (float)aImageCenterOffsetInPixels.y;
}

void Emb::render(std::string img, glm::vec2 imgSize,glm::vec2 centerOffset,float vFov,int geomId,glm::vec3 origine,glm::vec3 direction,glm::vec3 up,float aTMin, float aTMax)
{
	glm::vec3 dirFormSunToPi(-1.f,-1.f,1.f);
	dirFormSunToPi=glm::normalize(dirFormSunToPi);
	glm::vec3 pixDerivX, pixDerivY, upperLeft;
	Emb::ComputePixelDerivatives(direction,up,vFov*glm::pi<float>()/180.f,imgSize,centerOffset,pixDerivX,pixDerivY,upperLeft);

	FIBITMAP *bitmap= FreeImage_AllocateT(FIT_RGBF, imgSize.x, imgSize.y);
	unsigned pitch = FreeImage_GetPitch(bitmap);
	BYTE *bits = (BYTE*)FreeImage_GetBits(bitmap);

	std::cout<<"Rendering..."<<std::endl;

	std::chrono::time_point<std::chrono::system_clock> time_all;
	time_all=std::chrono::system_clock::now();

	//#pragma omp parallel for num_threads(16) shared(minD,maxD)
	for(int y = 0; y < imgSize.y; y++) 
	{
		FIRGBF *pixel = (FIRGBF*)FreeImage_GetScanLine(bitmap, imgSize.y-1-y);

		for(int x = 0; x < imgSize.x; x++)
		{
			glm::vec2 uv;
			float dist=0;
			int geomID=-1;
			int primID=-1;

			glm::vec3 directionImagePlane = (float)(x) * pixDerivX + (float)(y) * pixDerivY + upperLeft;
			directionImagePlane = glm::normalize(directionImagePlane);

			if(intersect(origine,directionImagePlane,aTMin,aTMax,uv,dist,primID,geomID))
			{
				std::shared_ptr<Object3D> obj=instance->embreeToObj[geomID];
				glm::vec3 pi=(1-uv.x-uv.y)*obj->m_vertices[obj->m_indices[primID*3]]+uv.x*obj->m_vertices[obj->m_indices[primID*3+1]]+uv.y*obj->m_vertices[obj->m_indices[primID*3+2]];
				glm::vec3 ni=(1-uv.x-uv.y)*obj->m_normals[obj->m_indices[primID*3]]+uv.x*obj->m_normals[obj->m_indices[primID*3+1]]+uv.y*obj->m_normals[obj->m_indices[primID*3+2]];


				pixel[x].red=1.f;
				pixel[x].green=1.f;
				pixel[x].blue=1.f;
				/*
				float dotp=std::max(0.f, glm::dot(-dirFormSunToPi,ni));

				if(dotp>0)
				{
				int geomID_oc=-1;
				int primID_oc=-1;
				bool isOcculted = isOccluded(pi,-dirFormSunToPi,1e-4f,FLT_MAX,primID_oc,geomID_oc);
				if(isOcculted)
				{
				pixel[x].red=0.f;
				pixel[x].green=0.f;
				pixel[x].blue=0.f;
				}
				else
				{
				pixel[x].red=1.f;
				pixel[x].green=1.f;
				pixel[x].blue=1.f;
				}
				}
				else
				{
				pixel[x].red=0.f;
				pixel[x].green=0.f;
				pixel[x].blue=0.f;
				}
				*/


			}
			else
			{
				pixel[x].red=0.f;
				pixel[x].green=0.f;
				pixel[x].blue=0.f;
			}
		}
	}

	std::chrono::time_point<std::chrono::system_clock> time_rendering;
	time_rendering=std::chrono::system_clock::now();

	std::chrono::duration<double, std::ratio<1>> nbrSecondes = (time_rendering - time_all);
	std::cout<<"Rendering time: "<<nbrSecondes.count()<<" seconds."<<std::endl;

	FreeImage_Save(FIF_TIFF,bitmap,img.c_str(),TIFF_DEFAULT);
	std::cout<<"Done"<<std::endl;

	FreeImage_Unload(bitmap);
}

std::shared_ptr<Object3D> Emb::getObject3D(unsigned int geomID){
	return  instance->embreeToObj[geomID];
}
/*
glm::uvec3 Emb::getVertexIndicesTriangle(unsigned int geomID, unsigned int idTriangle){
		std::shared_ptr<Object3D> obj=instance->embreeToObj[geomID];
	return glm::uvec3(obj->m_vertices[obj->m_indices[idTriangle*3]], obj->m_vertices[obj->m_indices[idTriangle*3]],obj->m_vertices[obj->m_indices[idTriangle*3]]);
}*/
 float Emb::Halton(int aBase, int aSeed){
    long i = aSeed;
    float h = 0.0f;
    float step = 1.0f / aBase;
    while (i > 0)
    {
            long digit = i % aBase;
            h += digit * step;
            i = (i - digit) / aBase;
            step /= aBase;
    }

    return h;
}

 glm::vec3 Emb::cosSampleHemisphere(float u, float v)
{
	float x = cosf((float) glm::pi<float>()*2.0f * u) * sqrtf(1-v);
	float y = sinf((float) glm::pi<float>()*2.0f * u) * sqrtf(1-v);
	float z = sqrtf(v);
	
	return glm::vec3(x, y, z);
}


 embree::LinearSpace3<embree::Vec3<float>> Emb::returnLocalFrame( embree::Vec3<float> _normal){
	  return embree::frame(_normal);
  }


glm::vec3 Emb::getIrradianceAtPoint( embree::LinearSpace3<embree::Vec3<float>> local_frame,  unsigned int idPoint,unsigned int geomID, Envmap & _envmap){

	glm::vec3 irradiance =glm::vec3(0.0,0.0,0.0);

	std::vector<std::pair<glm::vec3, glm::vec3>> _irradiance_dirtoColor= Emb::getIrradiancePatch(local_frame, idPoint,geomID,_envmap);

	for (unsigned int kC=0; kC< _irradiance_dirtoColor.size();kC++)
		irradiance +=  _irradiance_dirtoColor[kC].second;

	return irradiance/(float)_irradiance_dirtoColor.size();
}
	  
 std::vector<std::pair<glm::vec3, glm::vec3>>  Emb::getIrradiancePatch( embree::LinearSpace3<embree::Vec3<float>> local_frame,  unsigned int idPoint,unsigned int geomID, Envmap & _envmap){
	 
	 std::shared_ptr<Object3D> obj=instance->embreeToObj[geomID];
	 glm::vec3 position_point =obj->m_vertices[idPoint];
	 glm::vec3 normal_point=obj->m_normals[idPoint];

	
	 //store sample to project in vec with associated direction to color 
	 std::vector<std::pair<glm::vec3, glm::vec3>> _sample_dir_color;
	 
	 //-1st generate IS over the hemisphere
	 std::vector<glm::vec3> hemisphereDir;

	 int dirCount =8096;  /*CConfigLib::getSample();*/
	 
	 for(int dirIdx = 0; dirIdx < dirCount; dirIdx++){

		 float rv1 = Halton(2, dirIdx + 1);
		 float rv2 = Halton(3, dirIdx + 1);
		 hemisphereDir.push_back(glm::normalize(cosSampleHemisphere(rv1, rv2)));
	 }

	 //hemisphere done
	 normal_point=glm::normalize(normal_point);

	 /* constructs a coordinate frame form a normal */
	 embree::Vec3<float> _normal(normal_point.x,normal_point.y,normal_point.z);
	
	 glm::vec3 color =glm::vec3();

	 //intersection data
	 glm::vec2 uv; 
	 float aDist; 
	 int aTriId;
	 int aGeomId;

	 glm::vec3 direction;

	 //-> collect irradiance patch
	 for(int it=0;it<hemisphereDir.size();it++){

		// embree::Vec3<float> direction_arround_n = embree::frame(_normal)* embree::Vec3<float>(hemisphereDir[it].x,hemisphereDir[it].y,hemisphereDir[it].z);
		 
		 embree::Vec3<float> direction_arround_n = local_frame* embree::Vec3<float>(hemisphereDir[it].x,hemisphereDir[it].y,hemisphereDir[it].z);
		 
		 direction = glm::vec3(direction_arround_n.x,direction_arround_n.y,direction_arround_n.z);
		 direction=glm::normalize(direction);

		 bool isIntersecting= intersect(position_point, direction, 1e-5f,FLT_MAX,uv,aDist,aTriId,aGeomId);

		 if(isIntersecting){
			 //get color from the geometry intersected

			 color =glm::vec3(65535.0,0.0,0.0);

			 std::shared_ptr<Object3D> obj_3 = Emb::getObject3D(aGeomId);

			 unsigned int id_vertex_0 = obj_3->m_indices[aTriId*3];
			 unsigned int id_vertex_1 = obj_3->m_indices[aTriId*3+1];
			 unsigned int id_vertex_2 = obj_3->m_indices[aTriId*3+2];

			 glm::vec3 color_0 = obj_3->m_color[id_vertex_0];
			 glm::vec3 color_1 = obj_3->m_color[id_vertex_1];
			 glm::vec3 color_2 = obj_3->m_color[id_vertex_2];

			 color = (1.0f-uv.x-uv.y)*color_0 +uv.x*color_1+ uv.y*color_2;

			 glm::vec3 ni = (1.0f-uv.x-uv.y)*obj_3->m_normals[id_vertex_0] +uv.x*obj_3->m_normals[id_vertex_1]+ uv.y*obj_3->m_normals[id_vertex_2];
			 ni = glm::normalize(ni);

			 float cosTheta = glm::dot(-direction,ni);
			float solidAngle = std::max(0.f,cosTheta);

			 _sample_dir_color.push_back(std::make_pair(glm::normalize(direction), color*cosTheta*3.14f));

		 }
		 else{

			 // we dont add sample for now.
			 //=> if the envmap is loaded... sample the envmap
			  _sample_dir_color.push_back(std::make_pair(glm::normalize(direction), getEnvmapColor(glm::normalize(direction),_envmap)));
		 }

	 }


	 return _sample_dir_color;

 }


 
glm::vec3 Emb::getEnvmapColor(glm::vec3 dir, Envmap & _envmap)
{
	return _envmap.getEnvmapColor(dir);
}