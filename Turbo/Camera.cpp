// #include "stdafx.h"
#include "Camera.h"
#include "gtc/matrix_transform.hpp"

Camera::Camera()
{
	_position = glm::vec3(1, 0, 0);
	_up = glm::vec3(0, 1, 0);
	_look = glm::vec3(0, 0, 0);
	_near = 0.1f;
	_far = 5000.0f;
	_xang = 0.5f;
	_yang = 0.0f;
	_fov = 45.f;
	_aspect = 4.0f / 3.0f;
}

glm::mat4 Camera::getCamera()
{
	glm::mat4 camera = glm::perspective(glm::radians(_fov), _aspect, _near, _far) * glm::lookAt(_position, _look, _up);;
	return camera;
}

glm::vec3 Camera::getUp()
{
	return glm::vec3(0.0, 1.0, 0.0);
}
glm::vec3 Camera::getRight()
{
	return glm::normalize(glm::cross(getDirection(), _up));
}

glm::vec3 Camera::getPosition()
{
	return _position;
}
void Camera::setPosition(glm::vec3 inVec)
{
	_position = inVec;
}

glm::vec3 Camera::getDirection()
{
	return glm::normalize(_look - _position);
}

glm::vec3 Camera::getLookat()
{
	return _look;
}

void Camera::setLockat(glm::vec3 inVec)
{
	_look = inVec;
}

Camera::~Camera()
{
}
