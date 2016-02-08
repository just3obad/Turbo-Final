#pragma once
#include "Mesh.h"
#include <btBulletDynamicsCommon.h>

class BulletMesh :
	public Mesh
{
public:
	BulletMesh(const std::string& fileName);
	btRigidBody* getRigidBody();
	glm::mat4 getTRS();
	//void Draw();
	//void Draw(Shader* shader, glm::mat4 cam);
	~BulletMesh();
protected:
	btBvhTriangleMeshShape* getBulletColisionSet();
	btRigidBody* _RigBody;
};

