#pragma once

#include "glm.hpp"


class Camera
{
public:
	Camera();
	glm::mat4 getCamera();
	glm::vec3 getUp();
	glm::vec3 getRight();
	glm::vec3 getPosition();
	void setPosition(glm::vec3 inVec);
	glm::vec3 getLookat();
	void setLockat(glm::vec3 inVec);
	glm::vec3 getDirection();
	~Camera();
private:
	void test();
	glm::vec3 _position;
	float _near;
	float _far;
	float _xang;
	float _yang;
	float _fov;
	float _aspect;
	glm::vec3 _up;
	glm::vec3 _look;
};

