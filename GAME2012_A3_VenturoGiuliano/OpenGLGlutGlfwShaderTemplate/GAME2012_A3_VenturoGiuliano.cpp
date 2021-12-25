//***************************************************************************
// GAME2012_A3_VenturoGiuliano.cpp by Giuliano (C) 2020 All Rights Reserved.
// Student ID: 101319819
// GitHub: https://github.com/Giuliano98
// email: giuliano.venturogonzales@georgebrown.ca
//
// Assignment 3 submission.
//
// Description:
// Click run to see the results.
// You'll see 2 dice rotating in oppositve directions
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
#define MOVESPEED 0.1f
#define TURNSPEED 0.05f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,0.9,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)
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

GLuint vao, ibo, points_vbo, colors_vbo, mvp_ID;
glm::mat4 MVP, View, Projection;

// Our bitflag variable. 1 byte for up to 8 key states.
unsigned char keys = 0; // Initialized to 0 or 0b00000000.

// Texture variables.
GLuint textureID;
GLint width, height, bitDepth;

// dice-tex coorddinates
// Dice-Texture-Coordinates = dTC
float dTC[4][5][2] = {
	{{0.0f,0.0f}, {0.25f,0.0f}, {0.5f,0.0f}, {0.75f,0.0f}, {1.0f,0.0f}},
	{{0.0f,0.3333f}, {0.25f,0.3333f}, {0.5f,0.3333f}, {0.75f,0.3333f}, {1.0f,0.3333f}},
	{{0.0f,0.6666f}, {0.25f,0.6666f}, {0.5f,0.6666f}, {0.75f,0.6666f}, {1.0f,0.6666f}},
	{{0.0f,1.0f}, {0.25f,1.0f}, {0.5f,1.0f}, {0.75f,1.0f}, {1.0f,1.0f}}
};

// Geometry data.
GLshort indices[] = {
	// Front.
	0, 1, 2, 3,
	// Left.
	4, 5, 6, 7,			// 7, 4, 0, 3,
	// Bottom.
	8, 9, 10, 11,		// 0, 4, 5, 1,
	// Right.
	12, 13, 14, 15,		// 2, 1, 5, 6,
	// Back.
	16, 17, 18, 19,		// 5, 4, 7, 6,
	// Top.
	20, 21, 22, 23		// 2, 6, 7, 3
};

GLfloat vertices[] = {
	//	x,		y,			z,				u,					v
	// 2
	-0.9f,		-0.9f,		0.9f,		dTC[1][1][0],		dTC[1][1][1],	// 0.		
	0.9f,		-0.9f,		0.9f,		dTC[1][2][0],		dTC[1][2][1],	// 1.
	0.9f,		0.9f,		0.9f,		dTC[2][2][0],		dTC[2][2][1],	// 2.
	-0.9f,		0.9f,		0.9f,		dTC[2][1][0],		dTC[2][1][1],	// 3.
	// 1															   	
	-0.9f,		0.9f,		-0.9f,		dTC[2][0][0],		dTC[2][0][1],	// 4. (copy of old 7)
	-0.9f,		-0.9f,		-0.9f,		dTC[1][0][0],		dTC[1][0][1],	// 5. (copy of old 4)
	-0.9f,		-0.9f,		0.9f,		dTC[1][1][0],		dTC[1][1][1],	// 6. (copy of old 0)
	-0.9f,		0.9f,		0.9f,		dTC[2][1][0],		dTC[2][1][1],	// 7. (copy of old 3)
	// 6
	-0.9f,		-0.9f,		0.9f,		dTC[1][1][0],		dTC[1][1][1],	// 8. (copy of old 0)
	-0.9f,		-0.9f,		-0.9f,		dTC[0][1][0],		dTC[0][1][1],	// 9. (copy of old 4)
	0.9f,		-0.9f,		-0.9f,		dTC[0][2][0],		dTC[0][2][1],	// 10. (copy of old 5)
	0.9f,		-0.9f,		0.9f,		dTC[1][2][0],		dTC[1][2][1],	// 11. (copy of old 1)
	// 3
	0.9f,		0.9f,		0.9f,		dTC[2][2][0],		dTC[2][2][1],	// 12. (copy of old 2)
	0.9f,		-0.9f,		0.9f,		dTC[1][2][0],		dTC[1][2][1],	// 13. (copy of old 1)
	0.9f,		-0.9f,		-0.9f,		dTC[1][3][0],		dTC[1][3][1],	// 14. (copy of old 5)
	0.9f,		0.9f,		-0.9f,		dTC[2][3][0],		dTC[2][3][1],	// 15. (copy of old 6)
	// 4
	0.9f,		-0.9f,		-0.9f,		dTC[1][3][0],		dTC[1][3][1],	// 16. (copy of old 5)
	-0.9f,		-0.9f,		-0.9f,		dTC[1][4][0],		dTC[1][4][1],	// 17. (copy of old 4)
	-0.9f,		 0.9f,		-0.9f,		dTC[2][4][0],		dTC[2][4][1],	// 18. (copy of old 7)
	0.9f,		0.9f,		-0.9f,		dTC[2][3][0],		dTC[2][3][1],	// 19. (copy of old 6)
	// 5
	0.9f,		0.9f,		0.9f,		dTC[2][2][0],		dTC[2][2][1],	// 20. (copy of old 2)
	0.9f,		0.9f,		-0.9f,		dTC[3][2][0],		dTC[3][2][1],	// 21. (copy of old 6)
	-0.9f,		 0.9f,		-0.9f,		dTC[3][1][0],		dTC[3][1][1],	// 22. (copy of old 7)
	-0.9f,		0.9f,		0.9f,		dTC[2][1][0],		dTC[2][1][1]	// 23. (copy of old 3)
};

GLfloat colors[] = {
	1.0f, 1.0f, 1.0f,		// 0.
	1.0f, 1.0f, 1.0f,		// 1.
	1.0f, 1.0f, 1.0f,		// 2.
	1.0f, 1.0f, 1.0f,		// 3.

	1.0f, 1.0f, 1.0f,		// 4.
	1.0f, 1.0f, 1.0f,		// 5.
	1.0f, 1.0f, 1.0f,		// 6.
	1.0f, 1.0f, 1.0f,		// 7.

	1.0f, 1.0f, 1.0f,		// 8.
	1.0f, 1.0f, 1.0f,		// 9.
	1.0f, 1.0f, 1.0f,		// 10.
	1.0f, 1.0f, 1.0f,		// 11.

	1.0f, 1.0f, 1.0f,		// 12.
	1.0f, 1.0f, 1.0f,		// 13.
	1.0f, 1.0f, 1.0f,		// 14.
	1.0f, 1.0f, 1.0f,		// 15.

	1.0f, 1.0f, 1.0f,		// 16.
	1.0f, 1.0f, 1.0f,		// 17.
	1.0f, 1.0f, 1.0f,		// 18.
	1.0f, 1.0f, 1.0f,		// 19.

	1.0f, 1.0f, 1.0f,		// 20.
	1.0f, 1.0f, 1.0f,		// 21.
	1.0f, 1.0f, 1.0f,		// 22.
	1.0f, 1.0f, 1.0f		// 23.
};

float scale = 0.5f, angle = 0.0f, angle1 = 0.0f;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;
int lastX, lastY;

void timer(int); // Prototype.

void resetView()
{
	position = glm::vec3(0.0f, 0.0f, 12.0f);
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

	mvp_ID = glGetUniformLocation(program, "MVP");

	// Projection matrix : 45∞ Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	resetView();

	// Image loading.
	stbi_set_flip_vertically_on_load(true);

	unsigned char* image = stbi_load("d6.png", &width, &height, &bitDepth, 0);
	if (!image) {
		cout << "Unable to load file!" << endl;
		// Could add a return too if you modify init.
	}

	cout << "width: " << width << " height: " << height << " bd: " << bitDepth << endl;

	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glUniform1i(glGetUniformLocation(program, "texture0"), 0);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Clean up. But we don't want to unbind the texture or we cannot use it.
	stbi_image_free(image);

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		ibo = 0;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		points_vbo = 0;
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 5, 0);
			glEnableVertexAttribArray(0);
	
		// Now for the UV/ST values.
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 5, (void*)(sizeof(vertices[0]) * 3));
			glEnableVertexAttribArray(2);

		colors_vbo = 0;
		glGenBuffers(1, &colors_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Can optionally unbind the buffer to avoid modification.

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	// Enable depth testing and face culling. 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
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
void transformObject(float scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, glm::vec3(scale));
	
	// We must now update the View.
	calculateView();

	MVP = Projection * View * Model;
	glUniformMatrix4fv(mvp_ID, 1, GL_FALSE, &MVP[0][0]);
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
		transformObject(scale, YZ_AXIS, angle += 2.0f, glm::vec3(-3.0f, 0.0f, 0.0f));
		//Ordering the GPU to start the pipeline
		glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);

	transformObject(scale, YZ_AXIS, angle1 -= 2.0f, glm::vec3(3.0f, 0.0f, 0.0f));
		//Ordering the GPU to start the pipeline
		glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.
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
	glDeleteTextures(1, &textureID);
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

	glutCreateWindow("GAME2012_A3_VenturoGiuliano");

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