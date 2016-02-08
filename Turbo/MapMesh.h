#pragma once
#include "BulletMesh.h"

class MapMesh :
	public BulletMesh
{
public:
	MapMesh(const std::string& fileName);
	//void Draw();
	//void Draw(Shader* shader, glm::mat4 cam);
	//void Draw();
	~MapMesh();
private:
	//GLuint MapMesh::loadImage(const std::string& path);
};

