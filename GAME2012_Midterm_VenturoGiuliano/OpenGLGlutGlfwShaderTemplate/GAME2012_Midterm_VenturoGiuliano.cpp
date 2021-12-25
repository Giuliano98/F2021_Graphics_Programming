//***************************************************************************
// GAME2012_Midterm_VenturoGiuliano.cpp by Giuliano (C) 2020 All Rights Reserved.
// Student ID: 101319819
// GitHub: https://github.com/Giuliano98
// email: giuliano.venturogonzales@georgebrown.ca
//
// MidTerm Exam submission.
//
//
// Description:
// pentagonal prism as a plataform and a crystal hexagonal 
// Move the camera in the world with W A S D and R F
//
// Click run to see the results.
//
//*****************************************************************************

////http://glew.sourceforge.net/
//The OpenGL Extension Wrangler Library (GLEW) is a cross-platform open-source C/C++ extension loading library. 
//GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target
//platform. OpenGL core and extension functionality is exposed in a single header file. GLEW has been tested on a 
//variety of operating systems, including Windows, Linux, Mac OS X, FreeBSD, Irix, and Solaris.
//
//http://freeglut.sourceforge.net/
//The OpenGL Utility Toolkit(GLUT) is a library of utilities for OpenGL programs, which primarily perform system - level I / O with the host operating system.
//Functions performed include window definition, window control, and monitoring of keyboardand mouse input.
//Routines for drawing a number of geometric primitives(both in solid and wireframe mode) are also provided, including cubes, spheresand the Utah teapot.
//GLUT also has some limited support for creating pop - up menus..

//OpenGL functions are in a single library named GL (or OpenGL in Windows). Function names begin with the letters glSomeFunction*();
//Shaders are written in the OpenGL Shading Language(GLSL)
//To interface with the window system and to get input from external devices into our programs, we need another library. For the XWindow System, this library is called GLX, for Windows, it is wgl,
//and for the Macintosh, it is agl. Rather than using a different library for each system,
//we use two readily available libraries, the OpenGL Extension Wrangler(GLEW) and the OpenGLUtilityToolkit(GLUT).
//GLEW removes operating system dependencies. GLUT provides the minimum functionality that should be expected in any modern window system.
//OpenGL makes heavy use of defined constants to increase code readability and avoid the use of magic numbers.Thus, strings such as GL_FILL and GL_POINTS are defined in header(#include <GL/glut.h>)

//https://glm.g-truc.net/0.9.9/index.html
////OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
///////////////////////////////////////////////////////////////////////

using namespace std;

#include "stdlib.h"
#include "time.h"
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "prepShader.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <string>
#include <iostream>

#define BUFFER_OFFSET(x)  ((const void*) (x))
#define FPS 60
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,0.9,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)
#define SPEED 0.25f
#define PI 3.14159265

const float DEG_TO_R = (double)PI / (double)180.0;
// pisma helping points -------------- radio = 1 so no *1
const float Sin_18 = sin(18*DEG_TO_R);
const float Cos_18 = cos(18*DEG_TO_R);
const float Sin_54 = sin(54*DEG_TO_R);
const float Cos_54 = cos(54*DEG_TO_R);
// cristal helping points  ----------- radio = 0.5 so * 0.5
const float Cos_60 = cos(60*DEG_TO_R) * 0.5f;
const float Sin_60 = sin(60*DEG_TO_R) * 0.5f;

enum keyMasks {
	KEY_FORWARD  = 0b00000001,  // 0x01  or   1
	KEY_BACKWARD = 0b00000010,  // 0x02  or   2
	KEY_LEFT     = 0b00000100,  // 0x03  or   3
	KEY_RIGHT    = 0b00001000,  // 0x04  or   4
	KEY_UPWARD   = 0b00010000,  // 0x05  or   5
	KEY_DOWNWARD = 0b00100000   // 0x06  or   6

	// Any other keys you want to add.
};

static unsigned int
program,
vertexShaderId,
fragmentShaderId;

GLuint vao, ibo, points_vbo, colors_vbo, mvp_ID;
glm::mat4 MVP, View, Projection;
// Our bitflag variable. 1 byte for up to 8 key states.
unsigned char keys = 0; // Initialized to 0 or 0b00000000.


GLshort pentaP_indices[] = {
	// bottom
	0, 4, 3,
	2, 1, 0,
	0, 3, 2,
	// wall 1
	2, 8, 1,
	1, 8, 7,
	// wall 2
	7, 6, 0,
	1, 7, 0,
	// wall 3
	0, 6, 5,
	4, 0, 5,
	// wall 4
	4, 5, 9,
	3, 4, 9,
	// wall 5
	9, 8, 2,
	3, 9, 2,
	// top
	8, 9, 5,
	6, 7, 8,
	5, 6, 8
};

GLfloat pentaP_vertices[] = {
	 0.0f,   -1.0f,   -1.0f, // 0
	-Cos_18, -1.0f, -Sin_18, // 1
	-Cos_54, -1.0f,  Sin_54, // 2
	 Cos_54, -1.0f,  Sin_54, // 3
	 Cos_18, -1.0f, -Sin_18, // 4

	 Cos_18, 1.0f, -Sin_18,  // 5
	  0.0f,   1.0f,   -1.0f, // 6
	-Cos_18, 1.0f, -Sin_18,  // 7
	-Cos_54, 1.0f,  Sin_54,  // 8
	 Cos_54, 1.0f,  Sin_54   // 9
};

GLfloat pentaP_colors[] = {
	1.0f, 1.0f, 0.0f,		// 0.
	1.0f, 1.0f, 0.0f,		// 1.
	1.0f, 1.0f, 0.0f,		// 2.
	1.0f, 1.0f, 0.0f,		// 3.
	1.0f, 1.0f, 0.0f,		// 4.

	1.0f, 1.0f, 0.0f,		// 5.
	1.0f, 1.0f, 0.0f,		// 6.
	1.0f, 1.0f, 0.0f,		// 7.
	1.0f, 1.0f, 0.0f,		// 8.
	1.0f, 1.0f, 0.0f		// 9.
};

GLshort Cristal_indices[] = {
	//fast
	// wall 1
	0, 1, 2,
	7, 2, 1,
	//wall 2
	0, 6, 1,
	7, 1, 6,
	// wall 3
	0, 5, 6,
	7, 6, 5,
	// wall 4
	0, 4, 5,
	7, 5, 4,
	// wall 5
	0, 3, 4,
	7, 4, 3,
	// wall 6
	0, 2, 3,
	7, 3, 2

};

GLfloat Cristal_vertices[] = {
	 0.0f,   2.0f,   0.0f, // 0

	-0.5,    0.0f,   0.0f,  // 1
	-Cos_60, 0.0f,  Sin_60, // 2
	 Cos_60, 0.0f,  Sin_60, // 3
	 0.5,    0.0f,   0.0f,  // 4
	 Cos_60, 0.0f, -Sin_60, // 5
	-Cos_60, 0.0f, -Sin_60, // 6

	 0.0f,   -2.0f,   0.0f   // 7
};

GLfloat Cristal_colors[] = {
	0.0f, 1.0f, 0.0f,		// 0.
	0.0f, 1.0f, 0.7f,		// 1.
	0.0f, 1.0f, 1.0f,		// 2.
	1.0f, 1.0f, 0.7f,		// 3.
	0.0f, 1.0f, 0.0f,		// 4.
	1.0f, 1.0f, 0.5f,		// 5.
	1.0f, 1.0f, 0.0f,		// 6.
	0.1f, 1.0f, 0.0f,		// 7.
};


GLshort cube_indices[] = {
	// Front.
	0, 1, 2, 3,
	// Left.
	7, 4, 0, 3,
	// Bottom.
	0, 4, 5, 1,
	// Right.
	2, 1, 5, 6,
	// Back.
	5, 4, 7, 6,
	// Top.
	2, 6, 7, 3
};

GLfloat cube_vertices[] = {
	-0.9f, -0.9f, 0.9f,		// 0.
	0.9f, -0.9f, 0.9f,		// 1.
	0.9f, 0.9f, 0.9f,		// 2.
	-0.9f, 0.9f, 0.9f,		// 3.
	-0.9f, -0.9f, -0.9f,	// 4.
	0.9f, -0.9f, -0.9f,		// 5.
	0.9f, 0.9f, -0.9f,		// 6.
	-0.9f, 0.9f, -0.9f,		// 7.
};

GLfloat cube_colors[] = {
	1.0f, 0.0f, 0.0f,		// 0.
	0.0f, 1.0f, 0.0f,		// 1.
	0.0f, 0.0f, 1.0f,		// 2.
	1.0f, 1.0f, 0.0f,		// 3.
	1.0f, 0.0f, 1.0f,		// 4.
	0.0f, 1.0f, 1.0f,		// 5.
	1.0f, 1.0f, 1.0f,		// 6.
	0.0f, 0.0f, 0.0f		// 7.
};


float angle = 0.0f, angle1 = 0.0f,
	  xVal = 0.0f, yVal = 0.0f, zVal = 3.0f;
glm::vec3 scale_Prism = {1.5f, 0.25f, 1.5f}; // TODO: Scalae the prism
glm::vec3 scale_Crystal = {0.5f, 0.5f, 0.5f}; // TODO: Scalae the prism

void timer(int); // Prototype.

void init(void)
{
	srand((unsigned)time(NULL));
	// Create shader program executable.
	vertexShaderId = setShader((char*)"vertex", (char*)"cube.vert");
	fragmentShaderId = setShader((char*)"fragment", (char*)"cube.frag");
	program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);
	glUseProgram(program);

	mvp_ID = glGetUniformLocation(program, "MVP");

	// Projection matrix : 45∞ Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f); // todo: View Camera Configuration - Perspective view
	
	// Or, for an ortho camera :
	// Projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	View = glm::lookAt(		// TODO: View Camera Configuration - Camera -- (0,0,3) 
		glm::vec3(xVal, yVal, zVal), // Origin. Camera is at (0,0,3), in World Space
		glm::vec3(0, 0, 0),	  // Look target. Looks at the origin
		glm::vec3(0, 1, 0)   // Up vector. Head is up (set to 0,-1,0 to look upside-down)
	);

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		// Index
		ibo = 0;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pentaP_indices), pentaP_indices, GL_STATIC_DRAW);

		// Vertices
		points_vbo = 0;
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
		// glBufferData moved to draw function.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		// Colors
		colors_vbo = 0;
		glGenBuffers(1, &colors_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
		// glBufferData moved to draw function.
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Can optionally unbind the buffer to avoid modification.

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	glLineWidth(1.0f);

	// Enable depth testing and face culling. 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	timer(0); // Setup my recursive 'fixed' timestep/framerate.
}

//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, scale);
	// We must now update the View with potential new zVal.
	View = glm::lookAt(
		glm::vec3(xVal, yVal, zVal),  // Camera is at (0,0,3), in World Space
		glm::vec3(0, 0, 0),	   // and looks at the origin
		glm::vec3(0, 1, 0)    // Head is up (set to 0,-1,0 to look upside-down)
	);
	MVP = Projection * View * Model;
	glUniformMatrix4fv(mvp_ID, 1, GL_FALSE, &MVP[0][0]);
}

//---------------------------------------------------------------------
//
// drawCube
//
void drawCube()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
}

//---------------------------------------------------------------------
//
// drawPris
//
void drawPrism()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pentaP_indices), pentaP_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pentaP_vertices), pentaP_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pentaP_colors), pentaP_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_SHORT, 0);
}

//---------------------------------------------------------------------
//
// drawCristal
//
void drawCrystal()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Cristal_indices), Cristal_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cristal_vertices), Cristal_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cristal_colors), Cristal_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}


//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// background color
	glClearColor(0.30f, 0.0f, 0.25f, 0.0f);		// TODO: Set a color background other than black

	glBindVertexArray(vao);

		// drawing prism
		transformObject(scale_Prism, X_AXIS, 0.0f, glm::vec3(0.0f, -1.0f, 0.0f));	// TODO: scalate
		drawPrism();

		transformObject(scale_Crystal, Y_AXIS, angle += 0.5f, glm::vec3(0.0f, 0.5f, 0.0f));	// TODO: scalate
		drawCrystal();

	glBindVertexArray(0);
	// Can optionally unbind the vertex array to avoid modification.

	glutSwapBuffers(); // Now for a potentially smoother render.
}

void idle() // Not even called.
{
	glutPostRedisplay();
}

void parseKeys()
{
	// W & S
	if (keys & KEY_FORWARD)
		zVal -= SPEED;
	if (keys & KEY_BACKWARD)
		zVal += SPEED;
	// A & D
	if (keys & KEY_LEFT)
		xVal -= SPEED;
	if (keys & KEY_RIGHT)
		xVal += SPEED;
	// R & F
	if (keys & KEY_UPWARD)
		yVal += SPEED;
	if (keys & KEY_DOWNWARD)
		yVal -= SPEED;
}

void timer(int) { // Tick of the frame.
	// Get first timestamp
	int start = glutGet(GLUT_ELAPSED_TIME);
	// Update call
	parseKeys();
	// Display call
	glutPostRedisplay();
	// Calling next tick
	int end = glutGet(GLUT_ELAPSED_TIME);
	glutTimerFunc((1000 / FPS) - (end - start), timer, 0);
}

// Keyboard input processing routine.
void keyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; // keys = keys | KEY_FORWARD
		break;
	case 's':
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
		break;
	case 'a':
		if (!(keys & KEY_LEFT))
			keys |= KEY_LEFT; // keys = keys | KEY_LEFT
		break;
	case 'd':
		if (!(keys & KEY_RIGHT))
			keys |= KEY_RIGHT;
		break;
	case 'r':
		if (!(keys & KEY_UPWARD))
			keys |= KEY_UPWARD; // keys = keys | KEY_UPWARD
		break;
	case 'f':
		if (!(keys & KEY_DOWNWARD))
			keys |= KEY_DOWNWARD;
		break;
	default:
		break;
	}
}

void keyDownSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP: // Up arrow.
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; // keys = keys | KEY_FORWARD
		break;
	case GLUT_KEY_DOWN: // Down arrow.
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
		break;
	default:
		break;
	}
}

void keyUp(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		keys &= ~KEY_FORWARD; // keys = keys & ~KEY_FORWARD. ~ is bitwise NOT.
		break;
	case 's':
		keys &= ~KEY_BACKWARD;
		break;
	case 'a':
		keys &= ~KEY_LEFT; // keys = keys & ~KEY_FORWARD. ~ is bitwise NOT.
		break;
	case 'd':
		keys &= ~KEY_RIGHT;
		break;
	case 'r':
		keys &= ~KEY_UPWARD; // keys = keys & ~KEY_FORWARD. ~ is bitwise NOT.
		break;
	case 'f':
		keys &= ~KEY_DOWNWARD;
		break;
	default:
		break;
	}
}

void keyUpSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP:
		keys &= ~KEY_FORWARD; // keys = keys & ~KEY_FORWARD
		break;
	case GLUT_KEY_DOWN:
		keys &= ~KEY_BACKWARD;
		break;
	default:
		break;
	}
}

//---------------------------------------------------------------------
//
// main
//

int main(int argc, char** argv)
{
	//Before we can open a window, theremust be interaction between the windowing systemand OpenGL.In GLUT, this interaction is initiated by the following function call :
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);

	//if you comment out this line, a window is created with a default size
	glutInitWindowSize(800, 800);

	//the top-left corner of the display
	glutInitWindowPosition(0, 0);

	glutCreateWindow("GAME2012_Midterm_VenturoGiuliano");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init(); // Our own custom function.

	//If there are events in the queue, our program responds to them through functions
	//called callbacks.A callback function is associated with a specific type of event.
	//A display callback is generated when the application programm or the
	//operating system determines that the graphics in a window need to be redrawn.
	glutDisplayFunc(display); // Output.
	//glutIdleFunc(idle);

	glutKeyboardFunc(keyDown);
	glutSpecialFunc(keyDownSpec);
	glutKeyboardUpFunc(keyUp);
	glutSpecialUpFunc(keyUpSpec);

	//begin an event-processing loop
	glutMainLoop();
}