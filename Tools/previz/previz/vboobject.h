#ifndef VBOOBJECT_H
#define VBOOBJECT_H

#include "Shader.h"

#include <QObject>
#include <memory>
#include <map>

class Object3D;

class VboObject : public QObject
{
	Q_OBJECT

public:
	VboObject(QObject *parent=nullptr);
	VboObject(VboObject const& v);
	VboObject& VboObject::operator=(VboObject const& v);
	~VboObject();

	struct PackedVertex{
		glm::vec3 position;
		glm::vec3 normal;
		bool operator<(const PackedVertex that) const{
			return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
		};
	};

	void initVBO(std::shared_ptr<Object3D> obj);
	void deleteVbo();
	void draw();

	const std::shared_ptr<Object3D> getObjectRef(){return refObj;};
	const unsigned int getnbIndices(){return nbIndices;};	

private:

	bool getSimilarVertexIndex_fast(PackedVertex &packed, std::map<PackedVertex,unsigned int> &VertexToOutIndex,unsigned int &result);
	void indexVBO(std::vector<glm::vec3> &in_vertices,std::vector<glm::vec3> &in_normals,
				  std::vector<unsigned int> &out_indices,std::vector<glm::vec3> &out_vertices,
				  std::vector<glm::vec3> & out_normals);

	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint indicesBuffer;

	GLuint colorBuffer;

	std::shared_ptr<Object3D> refObj;
	unsigned int nbIndices;

};

#endif // VBOOBJECT_H
