// #include "stdafx.h"
#include "BulletMesh.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

BulletMesh::BulletMesh(const std::string& fileName)
	:Mesh(fileName)
{
}


btBvhTriangleMeshShape* BulletMesh::getBulletColisionSet()
{
	btTriangleMesh *mTriMesh = new btTriangleMesh();
	btVector3 v1, v2, v3;

	//_objModel->vertices[0];
	//_objModel->OBJIndices[0].vertexIndex;


	IndexedModel mod = _objModel->ToIndexedModel();
	
	for (unsigned int i = 0; i < _objModel->OBJIndices.size() -2; i += 3)
	{

		v1[0] = _objModel->vertices[_objModel->OBJIndices[i].vertexIndex].x*_scale.x + _pos.x;
		v1[1] = _objModel->vertices[_objModel->OBJIndices[i].vertexIndex].y*_scale.y + _pos.y;
		v1[2] = _objModel->vertices[_objModel->OBJIndices[i].vertexIndex].z*_scale.z + _pos.z;
						 
		v2[0] = _objModel->vertices[_objModel->OBJIndices[i + 1].vertexIndex].x*_scale.x + _pos.x;
		v2[1] = _objModel->vertices[_objModel->OBJIndices[i + 1].vertexIndex].y*_scale.y + _pos.y;
		v2[2] = _objModel->vertices[_objModel->OBJIndices[i + 1].vertexIndex].z*_scale.z + _pos.z;
						 
		v3[0] = _objModel->vertices[_objModel->OBJIndices[i + 2].vertexIndex].x*_scale.x + _pos.x;
		v3[1] = _objModel->vertices[_objModel->OBJIndices[i + 2].vertexIndex].y*_scale.y + _pos.y;
		v3[2] = _objModel->vertices[_objModel->OBJIndices[i + 2].vertexIndex].z*_scale.z + _pos.z;

		mTriMesh->addTriangle(v1, v2, v3);
	}
	
	return new btBvhTriangleMeshShape(mTriMesh, true);
}

glm::mat4 BulletMesh::getTRS()
{
	glm::mat4 buletMat(1.0f), meshMat;
	//no scale from bullet
	float data[16];
	if (_RigBody != NULL)
	{
		_RigBody->getCenterOfMassTransform().getOpenGLMatrix(data);
		buletMat = glm::mat4(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
	}
	else
	{
		buletMat = glm::translate(glm::mat4(1.f), _pos);
	}
	//initial Mesh rotation, scale
	meshMat = glm::rotate(glm::mat4(1.f), _rotLR, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.f), glm::vec3(_scale));
	return buletMat * meshMat;

	//return glm::translate(glm::mat4(1.f), _pos) * glm::rotate(glm::mat4(1.f), _rotLR, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.f), glm::vec3(_scale));

}

//void BulletMesh::Draw()
//{
//	Mesh::Draw();
//}
//
//void BulletMesh::Draw(Shader* shader, glm::mat4 cam)
//{
//	glUniformMatrix4fv(shader->getUniform(0), 1, GL_FALSE, glm::value_ptr(cam * getTRS()));
//	Mesh::Draw();
//}

btRigidBody* BulletMesh::getRigidBody()
{
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, this->getBulletColisionSet(), btVector3(0, 0, 0));
	_RigBody = new  btRigidBody(groundRigidBodyCI);
	return  _RigBody;
}

BulletMesh::~BulletMesh()
{
}
