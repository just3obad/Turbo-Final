// #include "stdafx.h"
#include "BulletCar.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <btBulletDynamicsCommon.h>

#define SCALE 10.0f

enum collisiontypes
{
	COL_NOTHING = 0,        //< Collide with nothing
	COL_WALL = 2,   //< Collide with walls
	COL_CAR = 4,   //< Collide with car
	COL_TRIANGLE = 8,   //< Collide with triangles
	COL_QUAD = 16    //< Collide with quads
};

BulletCar::BulletCar(const std::string& fileName, btDiscreteDynamicsWorld* dynWorld)
	:BulletMesh(fileName)
{
	generateVehicle(dynWorld);
}

glm::mat4 BulletCar::getTRS()
{
	glm::mat4 buletMat, meshMat;
	//no scale from bullet
	float data[16];
	_vehicle->getChassisWorldTransform().getOpenGLMatrix(data);
	buletMat = glm::mat4(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
	//initial Mesh rotation, scale
	meshMat = glm::rotate(glm::mat4(1.f), _rotLR, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.f), glm::vec3(_scale));
	return buletMat * meshMat;

}

glm::vec3 BulletCar::getPosition()
{
	float data[16];
	_vehicle->getChassisWorldTransform().getOpenGLMatrix(data);
	return glm::vec3(data[12], data[13], data[14]);
}

void BulletCar::Draw(Shader* shader, glm::mat4 cam)
{
	glUniformMatrix4fv(shader->getUniform(0), 1, GL_FALSE, glm::value_ptr(cam * getTRS()));
	BulletMesh::Draw();
}

void BulletCar::generateVehicle(btDiscreteDynamicsWorld* dynamicsWorld)
{
	// The vehicle
	btScalar chassisMass(1.0f*SCALE);
	btVector3 chassisInertia(0.0f, 0.0f, 0.0f);
	btCollisionShape* chassisShape = new btBoxShape(btVector3(1.0f*SCALE, 0.5f*SCALE, 2.0f*SCALE));
	btDefaultMotionState* chassisMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(400.0f, 20.0f, 900.0f)));
	//btDefaultMotionState* chassisMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(_pos.x, _pos.y, _pos.z)));
	chassisShape->calculateLocalInertia(chassisMass, chassisInertia);
	btRigidBody::btRigidBodyConstructionInfo chassisRigidBodyCI(chassisMass, chassisMotionState, chassisShape, chassisInertia);
	btRigidBody* chassisRigidBody = new btRigidBody(chassisRigidBodyCI);
	chassisRigidBody->setActivationState(DISABLE_DEACTIVATION);
	// Be sure to add the chassis of the vehicle into the world as a rigid body
	//dynamicsWorld->addRigidBody(chassisRigidBody, COL_CAR, COL_WALL);
	dynamicsWorld->addRigidBody(chassisRigidBody);

	btRaycastVehicle::btVehicleTuning tuning;
	btVehicleRaycaster* raycaster = new btDefaultVehicleRaycaster(dynamicsWorld);
	_vehicle = new btRaycastVehicle(tuning, chassisRigidBody, raycaster);
	_vehicle->setCoordinateSystem(0, 1, 2);

	btVector3 wheelDirection(0.0f, -1.0f, 0.0f);
	btVector3 wheelAxis(-1.0f, 0.0f, 0.0f);
	btScalar suspensionRestLength(0.6f);
	btScalar wheelRadius(0.5f * SCALE);
	// Be sure to attach the wheels not higher than the upper bounds of the hull of the vehicle chassis
	_vehicle->addWheel(btVector3(-0.5f * SCALE, 0.0f, 1.0f * SCALE), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, true);
	_vehicle->addWheel(btVector3(0.5f * SCALE, 0.0f, 1.0f * SCALE), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, true);
	_vehicle->addWheel(btVector3(-0.5f * SCALE, 0.0f, -1.0f * SCALE), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, false);
	_vehicle->addWheel(btVector3(0.5f * SCALE, 0.0f, -1.0f * SCALE), wheelDirection, wheelAxis, suspensionRestLength, wheelRadius, tuning, false);

	dynamicsWorld->addAction(_vehicle);
}

void BulletCar::setPosition(glm::vec3 pos)
{
	Mesh::setPosition(pos);
	btDefaultMotionState* chassisMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(_pos.x, _pos.y, _pos.z)));
}

BulletCar::~BulletCar()
{
	delete _vehicle;
}
