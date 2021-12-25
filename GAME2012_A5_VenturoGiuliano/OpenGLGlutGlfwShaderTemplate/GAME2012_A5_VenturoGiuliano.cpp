//***************************************************************************
// GAME2012_A5_VenturoGiuliano by Giuliano (C) 2020 All Rights Reserved.
// Student ID: 101319819
// GitHub: https://github.com/Giuliano98
// email: giuliano.venturogonzales@georgebrown.ca
//
// MidTerm Exam submission.
//
//
// Description:
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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include "Shape.h"
#include "Light.h"

#define BUFFER_OFFSET(x)  ((const void*) (x))
#define FPS 60
#define MOVESPEED 0.1f
#define TURNSPEED 0.05f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,0.9,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,1,1)
#define SPEED 0.25f

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


enum keyMasks {
	KEY_FORWARD = 0b00000001,		// 0x01 or   1	or   01
	KEY_BACKWARD = 0b00000010,		// 0x02 or   2	or   02
	KEY_LEFT = 0b00000100,
	KEY_RIGHT = 0b00001000,
	KEY_UP = 0b00010000,
	KEY_DOWN = 0b00100000,
	KEY_MOUSECLICKED = 0b01000000

	// Any other keys you want to add.
};

static unsigned int
program,
vertexShaderId,
fragmentShaderId;

GLuint modelID, viewID, projID;
glm::mat4 View, Projection;

// Our bitflag variable. 1 byte for up to 8 key states.
unsigned char keys = 0; // Initialized to 0 or 0b00000000.

// Texture variables.
GLuint blankID, groundID, glassID, cubeID;
GLint width, height, bitDepth;

// Light objects. Now OOP.
AmbientLight aLight(
	glm::vec3(1.0f, 1.0f, 1.0f),	// Diffuse colour.
	0.5f);							// Diffuse strength.


/*SpotLight sLight(
	glm::vec3(5.0f, 8.0f, -5.0f),	// Position.
	glm::vec3(1.0f, 0.1f, 1.0f),	// Diffuse colour.
	1.0f,							// Diffuse strength.
	glm::vec3(0.0f, -1.0f, 0.0f),   // Direction. Normally opposite because it's used in dot product. See constructor.
	20.0f);*/							// Edge.

float m_Radius = 2.0f; // Distances from the middle of the cube to the position of the sLight
float m_Height = 7.0f;
float m_Cos60R = static_cast<float>(cos(60 * PI / 180))*m_Radius;
float m_Sin60R = static_cast<float>(sin(60 * PI / 180))*m_Radius;

float m_lightStr = 0.1f;

SpotLight sLight[3] = {
	// Red
	{glm::vec3(0.0f + 5.0f, m_Height, -m_Radius - 5.0f),	// Position.
	glm::vec3(1.0f, 0.1f, 0.1f),	// Diffuse colour.
	m_lightStr,					     		// Diffuse strength.
	glm::vec3(0.0f, -1.0f, 0.0f), // Direction. 
	30.0f},
	// Green
	{glm::vec3(-m_Sin60R+ 5.0f, m_Height, m_Cos60R- 5.0f),	// Position.
	glm::vec3(0.1f, 1.0f, 0.1f),	// Diffuse colour.
	m_lightStr,						    	// Diffuse strength.
	glm::vec3(0.0f, -1.0f, 0.0f), // Direction. 
	30.0f},
	// Blue
	{glm::vec3(m_Sin60R+ 5.0f, m_Height, m_Cos60R- 5.0f),	// Position.
	glm::vec3(0.1f, 0.1f, 1.0f),	// Diffuse colour.
	m_lightStr,						    	// Diffuse strength.
	glm::vec3(0.0f, -1.0f, 0.0f), // Direction. 
	30.0f}
};


// Camera and transform variables.
float scale = 1.0f, angle = 0.0f;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;
int lastX, lastY;

// Geometry data.
Grid g_grid(10);
Cube g_cube;
Cube g_glass; // Setting a scale for the cube.
Prism g_prism(7);

void timer(int); // Prototype.

void resetView()
{
	position = glm::vec3(5.0f, 5.0f, 15.0f); // Super pulled back because of grid size.
	frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0.0f;
	yaw = -90.0f;
	// View will now get set only in transformObject
}

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



	modelID = glGetUniformLocation(program, "model");
	viewID = glGetUniformLocation(program, "view");
	projID = glGetUniformLocation(program, "projection");
	
	// Projection matrix : 45∞ Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	resetView();

	// Image loading.
	stbi_set_flip_vertically_on_load(true);


	// Load first image.
	unsigned char* image = stbi_load("blank.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &blankID);
	glBindTexture(GL_TEXTURE_2D, blankID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End first image.

	// Load second image.
	image = stbi_load("ground-texture.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &groundID);
	glBindTexture(GL_TEXTURE_2D, groundID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End second image.

	// Load third image.
	image = stbi_load("glass.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &glassID);
	glBindTexture(GL_TEXTURE_2D, glassID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End third image.

	// Load first image.
	image = stbi_load("cube.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &cubeID);
	glBindTexture(GL_TEXTURE_2D, cubeID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End first image.


	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	// Setting ambient light.
	glUniform3f(glGetUniformLocation(program, "aLight.base.diffuseColour"), aLight.diffuseColour.x, aLight.diffuseColour.y, aLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "aLight.base.diffuseStrength"), aLight.diffuseStrength);

	// Setting spot light.
	/*glUniform3f(glGetUniformLocation(program, "sLight.base.diffuseColour"), sLight.diffuseColour.x, sLight.diffuseColour.y, sLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "sLight.base.diffuseStrength"), sLight.diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "sLight.position"), sLight.position.x, sLight.position.y, sLight.position.z);
	glUniform3f(glGetUniformLocation(program, "sLight.direction"), sLight.direction.x, sLight.direction.y, sLight.direction.z);
	glUniform1f(glGetUniformLocation(program, "sLight.edge"), sLight.edgeRad);*/

	glUniform3f(glGetUniformLocation(program, "sLight[0].base.diffuseColour"), sLight[0].diffuseColour.x, sLight[0].diffuseColour.y, sLight[0].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "sLight[0].base.diffuseStrength"), sLight[0].diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "sLight[0].position"), sLight[0].position.x, sLight[0].position.y, sLight[0].position.z);
	glUniform3f(glGetUniformLocation(program, "sLight[0].direction"), sLight[0].direction.x, sLight[0].direction.y, sLight[0].direction.z);
	glUniform1f(glGetUniformLocation(program, "sLight[0].edge"), sLight[0].edgeRad);


	glUniform3f(glGetUniformLocation(program, "sLight[1].base.diffuseColour"), sLight[1].diffuseColour.x, sLight[1].diffuseColour.y, sLight[1].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "sLight[1].base.diffuseStrength"), sLight[1].diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "sLight[1].position"), sLight[1].position.x, sLight[1].position.y, sLight[1].position.z);
	glUniform3f(glGetUniformLocation(program, "sLight[1].direction"), sLight[1].direction.x, sLight[1].direction.y, sLight[1].direction.z);
	glUniform1f(glGetUniformLocation(program, "sLight[1].edge"), sLight[1].edgeRad);


	glUniform3f(glGetUniformLocation(program, "sLight[2].base.diffuseColour"), sLight[2].diffuseColour.x, sLight[2].diffuseColour.y, sLight[2].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "sLight[2].base.diffuseStrength"), sLight[2].diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "sLight[2].position"), sLight[2].position.x, sLight[2].position.y, sLight[2].position.z);
	glUniform3f(glGetUniformLocation(program, "sLight[2].direction"), sLight[2].direction.x, sLight[2].direction.y, sLight[2].direction.z);
	glUniform1f(glGetUniformLocation(program, "sLight[2].edge"), sLight[2].edgeRad);


	// All VAO/VBO data now in Shape.h! But we still need to do this AFTER OpenGL is initialized.
	g_grid.BufferShape();
	g_glass.BufferShape();
	g_prism.BufferShape();
	g_cube.BufferShape();

	// Enable depth test and blend.
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBlendEquation(GL_FUNC_ADD);

	// Enable smoothing.
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	// Enable face culling.
	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	timer(0); // Setup my recursive 'fixed' timestep/framerate.
}

//---------------------------------------------------------------------
//
// calculateView
//
void calculateView()
{
	frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec.y = sin(glm::radians(pitch));
	frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec = glm::normalize(frontVec);
	rightVec = glm::normalize(glm::cross(frontVec, worldUp));
	upVec = glm::normalize(glm::cross(rightVec, frontVec));

	View = glm::lookAt(
		position, // Camera position
		position + frontVec, // Look target
		upVec); // Up vector
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
	
	// We must now update the View.
	calculateView();

	glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projID, 1, GL_FALSE, &Projection[0][0]);
}

float m_Flow;
float m_Timer = 0.0f;
//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glBindTexture(GL_TEXTURE_2D, blankID); // Use this texture for all shapes.
	glClearColor(0.20f, 0.0f, 0.45f, 0.0f);	

	// Grid. Note: I rendered it solid!
	glBindTexture(GL_TEXTURE_2D, groundID);
	transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	g_grid.RecolorShape(1.0f, 1.0f, 1.0f);
	g_grid.DrawShape(GL_TRIANGLES);

	// Prism.
	glBindTexture(GL_TEXTURE_2D, blankID);
	transformObject(glm::vec3(1.0f, 2.0f, 1.0f), X_AXIS, 0.0f, glm::vec3(4.5f, 0.0f, -5.5f));
	g_prism.DrawShape(GL_TRIANGLES);

	// Cube.
	glBindTexture(GL_TEXTURE_2D, cubeID);
	transformObject(glm::vec3(0.5f, 0.5f, 0.5f), XZ_AXIS, angle+=3, glm::vec3(4.75f, 2.75f, -5.25f));
	g_cube.DrawShape(GL_TRIANGLES);
	

	// Glass.
	glBindTexture(GL_TEXTURE_2D, glassID);
	transformObject(glm::vec3(2.0f, 2.0f, 2.0f), X_AXIS, 0.0f, glm::vec3(4.0f, 2.0f, -6.0f));
	g_glass.RecolorShape(1.0f, 1.0f, 1.0f, 0.35f);
	g_glass.DrawShape(GL_TRIANGLES);

	// glBindTexture(GL_TEXTURE_2D, 0);
	m_Flow = sin(m_Timer += 0.016f);
	if (m_Flow > 0.5f)
	{
		glUniform1f(glGetUniformLocation(program, "sLight[0].base.diffuseStrength"), 1.0f);
		glUniform1f(glGetUniformLocation(program, "sLight[1].base.diffuseStrength"), 0.0f);
		glUniform1f(glGetUniformLocation(program, "sLight[2].base.diffuseStrength"), 0.0f);
	}
	if (m_Flow < -0.5f)
	{
		glUniform1f(glGetUniformLocation(program, "sLight[0].base.diffuseStrength"), 0.0f);
		glUniform1f(glGetUniformLocation(program, "sLight[1].base.diffuseStrength"), 1.0f);
		glUniform1f(glGetUniformLocation(program, "sLight[2].base.diffuseStrength"), 0.0f);
	}
	if (-0.5f<m_Flow && m_Flow<0.5f)
	{
		glUniform1f(glGetUniformLocation(program, "sLight[0].base.diffuseStrength"), 0.0f);
		glUniform1f(glGetUniformLocation(program, "sLight[1].base.diffuseStrength"), 0.0f);
		glUniform1f(glGetUniformLocation(program, "sLight[2].base.diffuseStrength"), 1.0f);
	}

	glutSwapBuffers(); // Now for a potentially smoother render.
}

void idle() // Not even called.
{
	glutPostRedisplay();
}

void parseKeys()
{
	if (keys & KEY_FORWARD)
		position += frontVec * MOVESPEED;
	if (keys & KEY_BACKWARD)
		position -= frontVec * MOVESPEED;
	if (keys & KEY_LEFT)
		position -= rightVec * MOVESPEED;
	if (keys & KEY_RIGHT)
		position += rightVec * MOVESPEED;
	if (keys & KEY_UP)
		position += upVec * MOVESPEED;
	if (keys & KEY_DOWN)
		position -= upVec * MOVESPEED;
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
	glutTimerFunc((1000 / FPS) - (end-start), timer, 0);
}

// Keyboard input processing routine.
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
			keys |= KEY_LEFT;
		break;
	case 'd':
		if (!(keys & KEY_RIGHT))
			keys |= KEY_RIGHT;
		break;
	case 'r':
		if (!(keys & KEY_UP))
			keys |= KEY_UP;
		break;
	case 'f':
		if (!(keys & KEY_DOWN))
			keys |= KEY_DOWN;
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
		keys &= ~KEY_LEFT;
		break;
	case 'd':
		keys &= ~KEY_RIGHT;
		break;
	case 'r':
		keys &= ~KEY_UP;
		break;
	case 'f':
		keys &= ~KEY_DOWN;
		break;
	case ' ':
		resetView();
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

void mouseMove(int x, int y)
{
	if (keys & KEY_MOUSECLICKED)
	{
		pitch += (GLfloat)((y - lastY) * TURNSPEED);
		yaw -= (GLfloat)((x - lastX) * TURNSPEED);
		lastY = y;
		lastX = x;
	}
}

void mouseClick(int btn, int state, int x, int y)
{
	if (state == 0)
	{
		lastX = x;
		lastY = y;
		keys |= KEY_MOUSECLICKED; // Flip flag to true
		glutSetCursor(GLUT_CURSOR_NONE);
		//cout << "Mouse clicked." << endl;
	}
	else
	{
		keys &= ~KEY_MOUSECLICKED; // Reset flag to false
		glutSetCursor(GLUT_CURSOR_INHERIT);
		//cout << "Mouse released." << endl;
	}
}

//---------------------------------------------------------------------
//
// clean
//
void clean()
{
	cout << "Cleaning up!" << endl;
	glDeleteTextures(1, &groundID);
	glDeleteTextures(1, &glassID);
	glDeleteTextures(1, &blankID);
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

	glutCreateWindow("GAME2012_A5_VenturoGiuliano");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init(); // Our own custom function.

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutSpecialFunc(keyDownSpec);
	glutKeyboardUpFunc(keyUp);
	glutSpecialUpFunc(keyUpSpec);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove); // Requires click to register.

	atexit(clean); // This useful GLUT function calls specified function before exiting program. 
	glutMainLoop();

	return 0;
}