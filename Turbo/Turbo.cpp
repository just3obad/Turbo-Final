// Turbo.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_DEPRECATE
// #include "stdafx.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include <iostream>
//#include <fstream>
//#include <sstream>


#include "ShaderProgram.h"
#include "Camera.h"
#include "gtc/type_ptr.hpp"

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
Shader* shader2;
Shader* shader3;
Mesh* mesh;
BulletCar* car;
MapMesh* map;
Mesh* water;
BulletMesh* track;
BulletMesh* ramp;
BulletMesh* Coin;
Mesh* Sky;
BulletMesh* startLine;

//Particle* particles;

// Build the broadphase
btBroadphaseInterface* broadphase;

// Set up the collision configuration and dispatcher
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;

// The actual physics solver
btSequentialImpulseConstraintSolver* solver;

// The world.
btDiscreteDynamicsWorld* dynamicsWorld;


double timeNow;
double timeold;

void heightmapcollision()
{
	enum collisiontypes
	{
		COL_NOTHING = 0,        //< Collide with nothing
		COL_WALL = 2,   //< Collide with walls
		COL_CAR = 4,   //< Collide with car
		COL_TRIANGLE = 8,   //< Collide with triangles
		COL_QUAD = 16    //< Collide with quads
	};


	dynamicsWorld->addRigidBody(map->getRigidBody());
	dynamicsWorld->addRigidBody(track->getRigidBody());
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
	water->setScale(glm::vec3(2000.0f, 0.0f, 2000.0f));
	water->loadImage("img/WaterPlain0017_1_S");

	//load Start
	startLine = new BulletMesh("objects/plane.obj");
	startLine->setRotationLeftRight(0); //says degre, semes rad
	startLine->setPosition(glm::vec3(266.5f, 0.4f, 1000.0f));
	startLine->setScale(glm::vec3(237.5f, 1.0f, 10.0f));
	startLine->loadImage("img/Start");

	//load Sky
	Sky = new BulletMesh("objects/Cube.obj");
	Sky->setRotationLeftRight(0); //says degre, semes rad
	Sky->setPosition(glm::vec3(-250.0f, -150.0f, -250.0f));
	Sky->setScale(glm::vec3(2500.0f, 500.0f,2500.0f));
	Sky->loadImage("img/Start");

	//load Map
	map = new MapMesh("objects/Terrain Ramp.obj");
	map->setRotationLeftRight(0); //says degre, semes rad
	map->setPosition(glm::vec3(2000.0f, -100.0f, 0.0f));
	map->setScale(glm::vec3(1.0f, 1.0f, 1.0f));

	//load Coin
	Coin = new BulletMesh("objects/object1.obj");
	Coin->setPosition(glm::vec3(250.0f, 100.0f, 310.0f));
	Coin->setScale(glm::vec3(5.0f, 5.0f, 5.0f));

	//load Borders
	track = new BulletMesh("objects/Borders2.obj");
	track->setRotationLeftRight(0); //says degre, semes rad
	track->setPosition(glm::vec3(2000.0f, -100.0f, 0.0f));
	track->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	track->loadImage("img/wall");

	//load ramp
	//ramp = new BulletMesh("objects\\Terrain Ramp.obj");
	//Coin->setPosition(glm::vec3(250.0f, 100.0f, 310.0f));
	//Coin->setScale(glm::vec3(5.0f, 5.0f, 5.0f));


	//
	//particles = new Particle();


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

#define SC 5.0f

void moveCam()
{
	//only for test purpuses, bad design warning!!

	glm::vec3 pos;
	glm::vec3 look;

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS){
		pos += 0.1f *  cam->getDirection() * SC;
		look += 0.1f * cam->getDirection() * SC;
	}

	// Move backward
	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS){
		pos += -0.1f * cam->getDirection() * SC;
		look += -0.1f * cam->getDirection() * SC;
	}

	//Move right
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS){
		pos += 0.07f * cam->getRight() * SC;
		look += 0.07f * cam->getRight() * SC;
	}

	//Move left
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS){
		pos += -0.07f * cam->getRight() * SC;
		look += -0.07f * cam->getRight() * SC;
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
		pos += 0.1f * cam->getUp() * SC;
		look += 0.1f * cam->getUp() * SC;
	}

	// Move down
	if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS){
		pos += -0.1f * cam->getUp() * SC;
		look += -0.1f * cam->getUp() * SC;
	}

	cam->setPosition(cam->getPosition() + pos);
	cam->setLockat(cam->getLookat() + look);
}

#define SCALE 5.0f

void updateContend(double deltaTime)
{
	static float acceleration = 0;
	static float steering = 0;

	//apply bullet
	dynamicsWorld->stepSimulation(deltaTime, 10, 1.0f / 60.0f);

	//car controls
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		acceleration += 0.001 * SCALE * deltaTime;
	}
	else
	{
		acceleration *= 0.95f;
	}

	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		acceleration -= 0.001f * SCALE *  deltaTime;
	}

	//Move right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		steering -= 0.01f  * SCALE * deltaTime;
	}

	// Move left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		steering += 0.01f * SCALE *  deltaTime;
	}

	if ((glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_PRESS))
	{
		steering *= 0.95f;
	}

	//beakes
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		car->getVehicle()->setBrake(0.01f, 0);
		car->getVehicle()->setBrake(0.01f, 1);
		car->getVehicle()->setBrake(0.01f, 2);
		car->getVehicle()->setBrake(0.01f, 3);
	}
	else
	{
		car->getVehicle()->setBrake(0.0f, 0);
		car->getVehicle()->setBrake(0.0f, 1);
		car->getVehicle()->setBrake(0.0f, 2);
		car->getVehicle()->setBrake(0.0f, 3);
	}

	if (acceleration > 0.1f * SCALE)
	{
		acceleration = 0.1f * SCALE;
	}
	if (acceleration < -0.05f * SCALE)
	{
		acceleration = -0.05f * SCALE;
	}

	if (car->getVehicle()->getCurrentSpeedKmHour() > 100)
	{
		if (car->getVehicle()->getCurrentSpeedKmHour() > 200)
		{
		acceleration = 0;
		}
		else
		{
			acceleration = acceleration * (200 / car->getVehicle()->getCurrentSpeedKmHour() - 1);
		}
	}

	if (steering > 0.01f * SCALE)
		steering = 0.01f * SCALE;
	if (steering < -0.01f * SCALE)
		steering = -0.01f * SCALE;

	car->getVehicle()->setSteeringValue(steering, 0);
	car->getVehicle()->setSteeringValue(steering, 1);
	car->getVehicle()->applyEngineForce(acceleration, 2);
	car->getVehicle()->applyEngineForce(acceleration, 3);

	//float speed = car->getVehicle()->getCurrentSpeedKmHour();
	//std:cout << speed << std::endl;

	moveCam();


	//set cam behind car
	cam->setLockat(car->getPosition());
	btVector3 forward = car->getVehicle()->getForwardVector();
	glm::vec3 Backward = glm::normalize(glm::vec3(-1 * forward.x(), -1 * forward.y(), -1 * forward.z()));
	glm::vec3  camOffset  = glm::vec3(Backward.x * 100.0f, Backward.y * 100.0f + 25.0f, Backward.z * 100.0f);
	cam->setPosition(car->getPosition() + camOffset);



	//map->getRigidBody()->setUserPointer()
	
	//Perform collision detection
	dynamicsWorld->performDiscreteCollisionDetection();
	// This should be done during a simulation tick (substep) callback, because contacts might be added and removed during several substeps of a single stepSimulation call.
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	//For each contact manifold
	for (int i = 0; i < numManifolds; i++) {
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();
		contactManifold->refreshContactPoints(obA->getWorldTransform(), obB->getWorldTransform());
		int numContacts = contactManifold->getNumContacts();
		//For each contact point in that manifold
		for (int j = 0; j < numContacts; j++) {
			//Get the contact information
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			btVector3 ptA = pt.getPositionWorldOnA();
			btVector3 ptB = pt.getPositionWorldOnB();
			double ptdist = pt.getDistance();
		}
	}
	//Dann könnte man folgende if Bedingung aufstellen mit tags zu jedem Objekt
	//int 	getIslandTag () const
	//map.setIslandTag (int tag= 2 oder so..)
	//if(objA.getislandTag() == 2)
	//{
	//	dynamicsworld->removerigidBody...

	//	..



}

void drawContent(double deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//uniforms
	glUniformMatrix4fv(shader2->getUniform(1), 1, GL_FALSE, glm::value_ptr(glm::vec4(0.0f, 300.0f, 0.0f, 0.0))); //light
	glUniformMatrix4fv(shader2->getUniform(2), 1, GL_FALSE, glm::value_ptr(glm::vec4(cam->getPosition(), 0.0f))); //view

	glUniformMatrix4fv(shader3->getUniform(1), 1, GL_FALSE, glm::value_ptr(glm::vec4(0.0f, 300.0f, 0.0f, 0.0f))); //light
	glUniformMatrix4fv(shader3->getUniform(2), 1, GL_FALSE, glm::value_ptr(glm::vec4(cam->getPosition(), 0.0f))); //view


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

	track->Draw(shader3, cam->getCamera());
	water->Draw(shader3, cam->getCamera());
	startLine->Draw(shader3, cam->getCamera());
	map->Draw(shader3, cam->getCamera());

	//particles->Draw();
}


void destroy()
{
	glfwDestroyWindow(window);
	glfwTerminate();
	delete car;
	delete mesh;
	delete map;
	delete dynamicsWorld;
	delete water;
	delete Coin;
	delete ramp;
	delete track;
	delete cam;
	delete shader2;
	delete shader3;
	delete Sky;
	delete startLine;
	delete broadphase;
	delete collisionConfiguration;
	delete dispatcher;
	delete solver;

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


