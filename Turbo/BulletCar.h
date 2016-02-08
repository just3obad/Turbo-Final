#pragma once
#include "BulletMesh.h"
#include <btBulletDynamicsCommon.h>

class BulletCar: public BulletMesh
{
public:
	BulletCar(const std::string& fileName, btDiscreteDynamicsWorld* dynWorld);
	glm::mat4 getTRS();
	void Draw(Shader* shader, glm::mat4 cam);
	btRaycastVehicle* getVehicle(){ return _vehicle; }
	void setPosition(glm::vec3 pos);
	glm::vec3 getPosition();
	~BulletCar();
private:
	void generateVehicle(btDiscreteDynamicsWorld* dynamicsWorld);
	btRaycastVehicle* _vehicle;
};

