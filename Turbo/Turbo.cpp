// turbo.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_DEPRECATE
// #include "stdafx.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include <iostream>
//#include <fstream>
//#include <sstream>


#include "HeightMapper.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "gtc/type_ptr.hpp"
#include "test.h"

#include "gtc/matrix_transform.hpp"
#include <iostream>
#include <fstream>
#include "Mesh.h"
#include "BulletCar.h"
#include "MapMesh.h"

#include <btBulletDynamicsCommon.h>


#include <iostream>
#include <vector>
//#include "Turbo.h"

using namespace std;

GLFWwindow* window;
Camera *cam;
//Shader* shader;
Shader* shader2;
Shader* shader3;
Mesh* mesh;
BulletCar* car;
MapMesh* map;
Mesh* water;
BulletMesh* Coin;
Mesh* Sky;

BulletMesh* startLine;

// Build the broadphase
btBroadphaseInterface* broadphase;

// Set up the collision configuration and dispatcher
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;

// The actual physics solver
btSequentialImpulseConstraintSolver* solver;

// The world.
btDiscreteDynamicsWorld* dynamicsWorld;


btCollisionShape *heightmap;
double timeNow;
double timeold;

void heightmapcollision()
{
	/*heightmap = map->getBulletColisionSet();
	
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo
	groundRigidBodyCI(0, groundMotionState, heightmap, btVector3(0, 0, 0));
	btRigidBody* map = new btRigidBody(groundRigidBodyCI);*/

	enum collisiontypes
	{
		COL_NOTHING = 0,        //< Collide with nothing
		COL_WALL = 2,   //< Collide with walls
		COL_CAR = 4,   //< Collide with car
		COL_TRIANGLE = 8,   //< Collide with triangles
		COL_QUAD = 16    //< Collide with quads
	};


	dynamicsWorld->addRigidBody(map->getRigidBody());
	dynamicsWorld->addRigidBody(Coin->getRigidBody());

}

int initOpenGL()
{
	if (!glfwInit())
		return -1;
	

	//create Window
	window = glfwCreateWindow(1280, 960, "Turbo", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	//context
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	//depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	return 0;
	
}

void loadContent()
{
	//initialise time
	timeNow = glfwGetTime();
	timeold = glfwGetTime();

	//load Map
	//mapper = new HeightMapper();
	//mapper->loadHeightMap("img\\Track");

	//load Shader
	//shader = new Shader("shader\\heightColor");
	shader2 = new Shader("shader/LightMaterial");
	shader3 = new Shader("shader/Texture");

	//create Camera object
	cam = new Camera();
	cam->setPosition(glm::vec3(-100, 200, -100));
	cam->setLockat(glm::vec3(500, 0, 250));

	//load Mesh
	mesh = new Mesh("objects/object1.obj");
	mesh->setRotationLeftRight(0); //says degre, semes rad
	mesh->setPosition(glm::vec3(250.0f, 10.0f, 310.0f));
	mesh->setScale(glm::vec3(5.0f, 5.0f, 5.0f));

	//load watersurface
	water = new Mesh("objects/plane.obj");
	water->setRotationLeftRight(0); //says degre, semes rad
	water->setPosition(glm::vec3(0.0f, -80.0f, 0.0f));
	water->setScale(glm::vec3(2000.0f, 0.0f, 1250.0f));
	water->loadImage("img/WaterPlain0017_1_S");

	//load Start
	startLine = new BulletMesh("objects/plane.obj");
	startLine->setRotationLeftRight(0); //says degre, semes rad
	startLine->setPosition(glm::vec3(300.0f, 0.0f, 325.0f));
	startLine->setScale(glm::vec3(50.0f, 1.0f, 10.0f));
	startLine->loadImage("img/Start");

	//load Sky
	Sky = new BulletMesh("objects/Cube.obj");
	Sky->setRotationLeftRight(0); //says degre, semes rad
	Sky->setPosition(glm::vec3(-250.0f, -150.0f, -250.0f));
	Sky->setScale(glm::vec3(2500.0f, 500.0f, 1750.0f));
	Sky->loadImage("img/Start");

	//load Map
	map = new MapMesh("objects/Terrain 1_8.obj");
	map->setRotationLeftRight(0); //says degre, semes rad
	map->setPosition(glm::vec3(2000.0f, -100.0f, 0.0f));
	map->setScale(glm::vec3(1.0f, 1.0f, 1.0f));

	//load Coin
	Coin = new BulletMesh("objects/object1.obj");
	Coin->setPosition(glm::vec3(250.0f, 100.0f, 310.0f));
	Coin->setScale(glm::vec3(5.0f, 5.0f, 5.0f));

	//setup Bulet
	broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -0.1, 0));
	heightmapcollision();

	//load BuletMesh
	car = new BulletCar("objects/8275_Seat_Concept.obj", dynamicsWorld);
	//car->setPosition(glm::vec3(500.0f, 0.0f, 300.0f));
	car->setRotationLeftRight(1.57f);
	car->setScale(glm::vec3(0.05f, 0.05f, 0.05f));
}

void moveCam()
{
	//only for test purpuses, bad design warning!!

	glm::vec3 pos;
	glm::vec3 look;

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS){
		pos += 0.1f * cam->getDirection();
		look += 0.1f * cam->getDirection();
	}

	// Move backward
	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS){
		pos += -0.1f * cam->getDirection();
		look += -0.1f * cam->getDirection();
	}

	//Move right
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS){
		pos += 0.07f * cam->getRight();
		look += 0.07f * cam->getRight();
	}

	//Move left
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS){
		pos += -0.07f * cam->getRight();
		look += -0.07f * cam->getRight();
	}

	//rotate right
	if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS){
		pos += 0.3f * cam->getRight();
	}

	//rotate left
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS){
		pos += -0.3f * cam->getRight();
	}

	//Move up
	if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS){
		pos += 0.1f * cam->getUp();
		look += 0.1f * cam->getUp();
	}

	// Move down
	if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS){
		pos += -0.1f * cam->getUp();
		look += -0.1f * cam->getUp();
	}

	cam->setPosition(cam->getPosition() + pos);
	cam->setLockat(cam->getLookat() + look);
}

void updateContend(double deltaTime)
{
	static float acceleration = 0;
	static float steering = 0;
	
	//apply bullet
	dynamicsWorld->stepSimulation(deltaTime, 10, 1.0f / 60.0f);
	
	//car controls
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		acceleration += 0.0001f;	
	}
	else
	{
		acceleration *= 0.95f;
	}

	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		acceleration -= 0.0001f;
	}

	//Move right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		steering -= 0.0003f;
	}

	// Move left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		steering += 0.0003f;
	}

	if ((glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_PRESS))
	{
		steering *= 0.95f;
	}

	if (acceleration > 0.01f)
		acceleration = 0.01f;
	if (acceleration < -0.05f)
		acceleration = -0.05f;


	if (steering > 0.01f)
		steering = 0.01f;
	if (steering < -0.01f)
		steering = -0.01f;

	car->getVehicle()->setSteeringValue(steering, 0);
	car->getVehicle()->setSteeringValue(steering, 1);
	car->getVehicle()->applyEngineForce(acceleration, 2);
	car->getVehicle()->applyEngineForce(acceleration, 3);

	float speed = car->getVehicle()->getCurrentSpeedKmHour();
	std:cout << speed << std::endl;

	moveCam();

	//set cam to map center (maybe car)
	cam->setLockat(car->getPosition());
	
}

void drawContent(double deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//uniforms
	glUniformMatrix4fv(shader2->getUniform(2), 1, GL_FALSE, glm::value_ptr(glm::vec4(0.0f, 300.0f, 0.0f, 0.0))); //light
	glUniformMatrix4fv(shader3->getUniform(2), 1, GL_FALSE, glm::value_ptr(glm::vec4(0.0f, 300.0f, 0.0f, 0.0))); //light

	//light
	glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(glm::vec4(0.0f, 300.0f, 0.0f, 0.0)));

	//load shader
	shader2->Bind();

	//mesh->Draw(shader2, cam->getCamera());
	car->Draw(shader2, cam->getCamera());
	Sky->Draw(shader2, cam->getCamera());
	Coin->Draw(shader2, cam->getCamera());

	//new Shader
	shader3->Bind();


	water->Draw(shader3, cam->getCamera());
	startLine->Draw(shader3, cam->getCamera());
	map->Draw(shader3, cam->getCamera());
}


void destroy()
{
	glfwDestroyWindow(window);
	glfwTerminate();
	delete car;
	delete mesh;
	delete map;
	delete dynamicsWorld;
}

int main(int argc, char* argv[])
{
	if (initOpenGL() == -1)
		return -1;

	loadContent();

	//loop
	timeold = timeNow;
	timeNow = glfwGetTime();
	double deltaTime = timeNow - timeold;
	while (!glfwWindowShouldClose(window))
	{
		updateContend(deltaTime);
		drawContent(deltaTime);
		// update other events like input handling 
		glfwPollEvents();
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);
	}

	destroy();
	return 0;
}


