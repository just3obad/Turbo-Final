// #include "stdafx.h"
#include "MapMesh.h"
#include <iostream>
#include "gtc/type_ptr.hpp"

MapMesh::MapMesh(const std::string& fileName)
	:BulletMesh(fileName)
{
	loadImage("img\\Grass");
}

//void MapMesh::Draw()
//{
//	glBindVertexArray(m_vertexArrayObject);
//
//	if (_useTexture)
//	{
//		glBindTexture(GL_TEXTURE_2D, _textureID);
//		glActiveTexture(GL_TEXTURE0);
//	}
//
//	glDrawElementsBaseVertex(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0, 0);
//
//	glBindVertexArray(0);
//}
//
//void MapMesh::Draw(Shader* shader, glm::mat4 cam)
//{
//	glUniformMatrix4fv(shader->getUniform(0), 1, GL_FALSE, glm::value_ptr(cam * getTRS()));
//	Draw();
//}

//void MapMesh::Draw()
//{
//	glBindVertexArray(m_vertexArrayObject);
//
//	glDrawElementsBaseVertex(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0, 0);
//
//	glBindVertexArray(0);
//}

MapMesh::~MapMesh()
{
}

