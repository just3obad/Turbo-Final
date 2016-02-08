#include "stdafx.h"
#include "Particle.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h> 

Particle::Particle()
{
}


Particle::~Particle()
{
}




#define MAX_PARTICLES 1000
#define WCX  640
#define WCY  480
#define RAIN 0


float slowdown = 2.0;
float velocity = 0.0;
float zoom = +40.0;
float hailsize = 1.0f;

int loop;


//floor colors
float b = 1.0;
float r = 0.0;
float g = 0.0;
float ground_points[1000][1000][3];
float ground_colors[1000][1000][4];
float accum = -10.0;

typedef struct {
	// Life
	bool alive; // is the particle alive?
	float life; // particle lifespan
	float fade; // decay
	// color
	float blue;
	// Position/direction
	float xpos;
	float ypos;
	float zpos;
	// Velocity/Direction, only goes down in y dir
	float vel;
	// Gravity
	float gravity;
}particles;

// Paticle System
particles par_sys[MAX_PARTICLES];

/*void normal_keys(unsigned char key, int x, int y) {
if (key == '=') { //really '+' - make hail bigger
hailsize += 0.01;
}
if (key == '-') { // make hail smaller
if (hailsize > 0.1) hailsize -= 0.01;
}
if (key == ',') { // really '<' slow down
if (slowdown > 4.0) slowdown += 0.01;
}
if (key == '.') { // really '>' speed up
if (slowdown > 1.0) slowdown -= 0.01;
}
if (key == 'q') { // QUIT
exit(0);
}
}*/


// Initialize/Reset Particles - give them their attributes
void initParticles(int i) {
	par_sys[i].alive = true;
	par_sys[i].life = 1.0;
	par_sys[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

	par_sys[i].xpos = (float)(rand() % 1000) - 10;
	par_sys[i].ypos = 10.0;
	par_sys[i].zpos = (float)(rand() % 1000) - 10;

	par_sys[i].blue = 1.0;

	par_sys[i].vel = velocity;
	par_sys[i].gravity = -0.8;//-0.8;

}

void init() {
	int x, z;

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);

	// Ground Verticies
	// Ground Colors
	for (z = 0; z < 1000; z++) {
		for (x = 0; x < 1000; x++) {
			ground_points[x][z][0] = x - 10.0;
			ground_points[x][z][1] = accum;
			ground_points[x][z][2] = z - 10.0;
			ground_colors[z][x][2] = b; // blue value
			ground_colors[z][x][3] = 0.0; // acummulation factor
		}
	}

	// Initialize particles
	for (loop = 0; loop < MAX_PARTICLES; loop++) {
		initParticles(loop);
	}
}

// For Rain
void drawRain() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(0.5, 0.5, 1.0);
			glBegin(GL_LINES);
			glVertex3f(x, y, z);
			glVertex3f(x, y + 0.5, z);
			glEnd();

			// Update values
			//Move
			// Adjust slowdown for speed!
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].ypos <= -10) {
				par_sys[loop].life = -1.0;
			}
			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}


void rain() {
	init();
	drawRain();
	//   glutKeyboardFunc(normal_keys);
	//   glutMainLoop();
}
void Particle::Draw()
{
	rain();
}