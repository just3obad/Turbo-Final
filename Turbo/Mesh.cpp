//
//  Mesh.cpp
//  Turbo
//
//  Created by Hareem Mughis on 19/01/16.
//  Copyright © 2016 Deniz Kesmez. All rights reserved.
//

#include "stdafx.h"
#include "Mesh.h"
//#include "util.h"
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "gtc\matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "lodepng.h"

Mesh::Mesh(const std::string& fileName)
{
	_objModel = new OBJModel(fileName);
	InitMesh(_objModel->ToIndexedModel());
}

void Mesh::InitMesh(const IndexedModel& model)
{
    m_numIndices = model.indices.size();
    
    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);
    
    glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model.positions[0]) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.normals[0]) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model.texCoords[0]) * model.texCoords.size(), &model.texCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.indices[0]) * model.indices.size(), &model.indices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

Mesh::Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{
    IndexedModel model;
    
    for(unsigned int i = 0; i < numVertices; i++)
    {
        model.positions.push_back(*vertices[i].GetPos());
        model.texCoords.push_back(*vertices[i].GetTexCoord());
        model.normals.push_back(*vertices[i].GetNormal());
    }
    
    for(unsigned int i = 0; i < numIndices; i++)
        model.indices.push_back(indices[i]);
    
    InitMesh(model);
}

Mesh::~Mesh()
{
    glDeleteBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void Mesh::Draw()
{
	glBindVertexArray(m_vertexArrayObject);

	if (_useTexture)
	{
		glBindTexture(GL_TEXTURE_2D, _textureID);
		glActiveTexture(GL_TEXTURE0);
	}

    glDrawElementsBaseVertex(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0, 0);
    
    glBindVertexArray(0);
}

void Mesh::Draw(Shader* shader, glm::mat4 cam)
{
	glUniformMatrix4fv(shader->getUniform(0), 1, GL_FALSE, glm::value_ptr(cam * getTRS()));
	Draw();
}

GLuint Mesh::loadImage(const std::string& path)
{
	std::vector<unsigned char> image;
	unsigned int height, width;

	// load image

	//decode
	unsigned error = lodepng::decode(image, width, height, path + ".png");

	//if there's an error, display it
	if (error){
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return -1;
	}

	glGenTextures(1, &_textureID);

	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);

	_useTexture = true;

	return _textureID;
}

glm::mat4 Mesh::getTRS()
{
	return glm::translate(glm::mat4(1.f), _pos) * glm::rotate(glm::mat4(1.f), _rotLR, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.f), glm::vec3(_scale));
}

glm::vec3 Mesh::getPosition()
{
	return _pos;
}
void Mesh::setPosition(glm::vec3 pos)
{
	_pos = pos;
}
glm::vec3 Mesh::getScale()
{
	return _scale;
}
void Mesh::setScale(glm::vec3 scale)
{
	_scale = scale;
}
float Mesh::getRotationLeftRight()
{
	return _rotLR;
}
void Mesh::setRotationLeftRight(float rot)
{
	_rotLR = rot;
}