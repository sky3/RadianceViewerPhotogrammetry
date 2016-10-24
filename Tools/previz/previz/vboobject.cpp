#include "vboobject.h"
#include "object3d.h"

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end

    1.0f,-1.0f, 1.0f,//3
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,

    1.0f, 1.0f,-1.0f,//4
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,

    -1.0f,-1.0f,-1.0f,//5
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,

    1.0f,-1.0f, 1.0f,//6
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,

    -1.0f, 1.0f, 1.0f,//7
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,//8
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,

    1.0f,-1.0f,-1.0f,//9
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,//10
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,

    1.0f, 1.0f, 1.0f,//11
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,//12
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

GLfloat g_color_buffer_data[] = {
        1.0f, 0.0f, 0.0f,//1
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//2
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//3
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//4
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//5
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//6
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//7
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//8
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//9
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//10
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//11
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,//12
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

static const GLfloat g_uv_buffer_data[] = { 
		0.000059f, 1.0f-0.000004f, 
		0.000103f, 1.0f-0.336048f, 
		0.335973f, 1.0f-0.335903f, 
		1.000023f, 1.0f-0.000013f, 
		0.667979f, 1.0f-0.335851f, 
		0.999958f, 1.0f-0.336064f, 
		0.667979f, 1.0f-0.335851f, 
		0.336024f, 1.0f-0.671877f, 
		0.667969f, 1.0f-0.671889f, 
		1.000023f, 1.0f-0.000013f, 
		0.668104f, 1.0f-0.000013f, 
		0.667979f, 1.0f-0.335851f, 
		0.000059f, 1.0f-0.000004f, 
		0.335973f, 1.0f-0.335903f, 
		0.336098f, 1.0f-0.000071f, 
		0.667979f, 1.0f-0.335851f, 
		0.335973f, 1.0f-0.335903f, 
		0.336024f, 1.0f-0.671877f, 
		1.000004f, 1.0f-0.671847f, 
		0.999958f, 1.0f-0.336064f, 
		0.667979f, 1.0f-0.335851f, 
		0.668104f, 1.0f-0.000013f, 
		0.335973f, 1.0f-0.335903f, 
		0.667979f, 1.0f-0.335851f, 
		0.335973f, 1.0f-0.335903f, 
		0.668104f, 1.0f-0.000013f, 
		0.336098f, 1.0f-0.000071f, 
		0.000103f, 1.0f-0.336048f, 
		0.000004f, 1.0f-0.671870f, 
		0.336024f, 1.0f-0.671877f, 
		0.000103f, 1.0f-0.336048f, 
		0.336024f, 1.0f-0.671877f, 
		0.335973f, 1.0f-0.335903f, 
		0.667969f, 1.0f-0.671889f, 
		1.000004f, 1.0f-0.671847f, 
		0.667979f, 1.0f-0.335851f
	};

VboObject::VboObject(QObject *parent): QObject(parent)
{

}

VboObject::~VboObject()
{

}

VboObject::VboObject(VboObject const& v):QObject(v.parent())
{
	refObj=v.refObj;
	nbIndices=v.nbIndices;
	vertexBuffer=v.vertexBuffer;
	normalBuffer=v.normalBuffer;
	indicesBuffer=v.indicesBuffer;
	colorBuffer = v.colorBuffer;
}

VboObject& VboObject::operator=(VboObject const& v)
{
	if(this!=&v)
	{
		this->setParent(v.parent());
		refObj=v.refObj;
		nbIndices=v.nbIndices;
		vertexBuffer=v.vertexBuffer;
		normalBuffer=v.normalBuffer;
		indicesBuffer=v.indicesBuffer;
	}
	return *this;
}

void VboObject::initVBO(std::shared_ptr<Object3D> obj)
{
	refObj=obj;
	std::vector<glm::vec3> *vertices=nullptr;
	std::vector<glm::vec3> *normals=nullptr;
	std::vector<glm::vec3> *colours=nullptr;
	std::vector<unsigned int> *indices=nullptr;

	std::vector<glm::vec3> vertices_tmp;
	std::vector<glm::vec3> normals_tmp;
	std::vector<unsigned int> indices_tmp;

	if(obj->getReadyForVbo())
	{
		vertices=&obj->m_vertices;
		normals=&obj->m_normals;
		indices=&obj->m_indices;		
		colours = &obj->m_color;
	}
	else
	{
		vertices=&vertices_tmp;
		normals=&normals_tmp;
		indices=&indices_tmp;	

		indexVBO(obj->m_vertices,obj->m_normals,indices_tmp,vertices_tmp,normals_tmp);
	}

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices->size()*sizeof(glm::vec3), vertices->data(), GL_STATIC_DRAW);

	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals->size()*sizeof(glm::vec3), normals->data(), GL_STATIC_DRAW);

	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, colours->size() *sizeof(glm::vec3), colours->data() , GL_STATIC_DRAW);

	//*
	glGenBuffers(1, &indicesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data() , GL_STATIC_DRAW);
	//*/

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	nbIndices=indices->size();
}

void VboObject::draw()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,(void*)0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

	glDrawElements(GL_TRIANGLES,nbIndices,GL_UNSIGNED_INT,(void*)0);
	//glDrawArrays(GL_TRIANGLES, 0, 12*3);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VboObject::deleteVbo()
{
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &indicesBuffer);
	glDeleteBuffers(1, &colorBuffer);
}

bool VboObject::getSimilarVertexIndex_fast(PackedVertex &packed, std::map<PackedVertex,unsigned int> &VertexToOutIndex,unsigned int &result)
{
	std::map<PackedVertex,unsigned int>::iterator it = VertexToOutIndex.find(packed);
	if(it == VertexToOutIndex.end())
	{
		return false;
	}else{
		result = it->second;
		return true;
	}
}

void VboObject::indexVBO(std::vector<glm::vec3> &in_vertices,std::vector<glm::vec3> &in_normals,
				  std::vector<unsigned int> &out_indices,std::vector<glm::vec3> &out_vertices,
				  std::vector<glm::vec3> & out_normals)
{
	std::map<PackedVertex,unsigned int> VertexToOutIndex;

	// For each input vertex
	for ( unsigned int i=0; i<in_vertices.size(); i++ ){

		PackedVertex packed = {in_vertices[i], in_normals[i]};
		

		// Try to find a similar vertex in out_XXXX
		unsigned int index;
		bool found = getSimilarVertexIndex_fast(packed, VertexToOutIndex, index);

		if ( found ){ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back( index );
		}else{ // If not, it needs to be added in the output data.
			out_vertices.push_back( in_vertices[i]);
			out_normals .push_back( in_normals[i]);
			unsigned int newindex = (unsigned int)out_vertices.size() - 1;
			out_indices .push_back( newindex );
			VertexToOutIndex[ packed ] = newindex;
		}
	}
}
