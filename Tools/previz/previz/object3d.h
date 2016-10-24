#ifndef OBJECT3D_H
#define OBJECT3D_H


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QObject>
#include <vector>
#include <memory>

class Object3D : public QObject
{
	Q_OBJECT

public:
	Object3D(QObject *parent=nullptr);
	~Object3D();

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec3> m_color;

	std::vector<unsigned int>   m_indices;
	unsigned int nbTriangles;
	unsigned int vertexSize;
	unsigned int embreeID;
	QString objName;
	QString filePath;

	bool getReadyForVbo(){return readyForVbo;};
	void setReadyForVbo(bool val){readyForVbo=val;};

	//radiance study...
	std::vector<std::vector<std::pair<unsigned int, glm::vec3>>>  _cache_radiance_per_points;

private:

	bool readyForVbo;
	
};

Q_DECLARE_METATYPE(std::shared_ptr<Object3D>);

#endif // OBJECT3D_H
