//
//  Mesh.hpp
//  Turbo
//
//  Created by Hareem Mughis on 19/01/16.
//  Copyright © 2016 Deniz Kesmez. All rights reserved.
//

#ifndef Mesh_h
#define Mesh_h

#include "obj_loader.h"
#include <GL/glew.h>
#include "glm.hpp"
#include "ShaderProgram.h"

//#include <string>
//#include <vector>

struct Vertex
{
public:
    Vertex(const glm::vec3& pos, const glm::vec2& texCoord, const glm::vec3& normal)
    {
        this->pos = pos;
        this->texCoord = texCoord;
        this->normal = normal;
    }
    
    glm::vec3* GetPos() { return &pos; }
    glm::vec2* GetTexCoord() { return &texCoord; }
    glm::vec3* GetNormal() { return &normal; }
    
private:
    glm::vec3 pos;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

enum MeshBufferPositions
{
    POSITION_VB,
    TEXCOORD_VB,
    NORMAL_VB,
    INDEX_VB
};

class Mesh
{
public:
    Mesh(const std::string& fileName);
    Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
   
	void Draw();
	void Draw(Shader* shader, glm::mat4 cam);
    
    virtual ~Mesh();

	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);
	glm::vec3 getScale();
	void setScale(glm::vec3 scale);
	float getRotationLeftRight();
	void setRotationLeftRight(float rot);
	GLuint loadImage(const std::string& path);

	glm::mat4 getTRS();

protected:
	glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 0.0f);
	float _rotUD = 0.0f;
	float _rotLR = 0.0f;
	glm::vec3 _scale = glm::vec3(1.0f, 1.0f, 1.0f);

	bool _useTexture = false;
	GLuint _textureID;

	GLuint m_vertexArrayObject;
	unsigned int m_numIndices;
	OBJModel* _objModel;

private:
    static const unsigned int NUM_BUFFERS = 4;
    void operator=(const Mesh& mesh) {}
    Mesh(const Mesh& mesh) {}
    
    void InitMesh(const IndexedModel& model);
    

    GLuint m_vertexArrayBuffers[NUM_BUFFERS];



};

#endif /* Mesh_h */
