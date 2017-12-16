// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
// Include GLEW
#include <GL/glew.h>
#include <gl/gl.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

//#include "tessterrain.h"
#include "terrain.h"
#include "shader.hpp"
#include "camera.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 720

//TessBumpTerrain m_terrain;
Terrain m_terrain;

Camera camera;
bool keys[1024];
int Textureflag = 0;
int AOflag = 1;
int line_mode = 0;

void init()
{
	camera.setGrid(GRID);

	camera.setPos(1416, 4000, 1999);
//	camera.rotate(-120, 120);

	m_terrain.Init();
}

void render()
{
	glm::mat4 projection = glm::perspective(fov, (float)WIDTH / (float)HEIGHT,
		znear, zfar);

	glm::vec3 dir = camera.getDir();
	glm::vec3 up = camera.getUp();
	glm::vec3 pos = camera.getPos();
	pos.x = 0;
	pos.z = 0;

	glm::mat4 view = glm::lookAt(pos, pos + dir, up);

	glm::mat4 model = glm::mat4(1.0f);

	m_terrain.m_WVP = projection * view * model;

	m_terrain.m_View = view;
	m_terrain.m_World = model;
	m_terrain.currentPos = camera.getPos();
	m_terrain.currentDir = camera.getDir();
	m_terrain.m_camerapos = glm::vec3(pos.x, pos.y, pos.z);
	m_terrain.m_ar = 1.0 * HEIGHT / WIDTH;
	m_terrain.m_fov = fov;

	m_terrain.Render();
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		keys[key] = true;
	else
		if (action == GLFW_RELEASE)
			keys[key] = false;


	if (keys[GLFW_KEY_P])
		printf("Current Positon: %d %d %d\n", int(camera.getPos().x),
			int(camera.getPos().z), int(camera.getPos().y));

	if (keys[GLFW_KEY_O])
		camera.setPos(585, 100, 441);

	if (keys[GLFW_KEY_L])
	{
		line_mode = 1 - line_mode;
		m_terrain.m_linemode = line_mode;
	}

	if (keys[GLFW_KEY_T])
	{
		Textureflag = (Textureflag + 1) % 3;
		m_terrain.m_Textureflag = Textureflag;
	}

	if (keys[GLFW_KEY_I])
	{
		//AOflag = 1 - AOflag;
		//m_terrain.AOflag = AOflag;
	}

	if (keys[GLFW_KEY_ESCAPE])
		exit(0);
}


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Terrain Rendering", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
							 // Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, keyCallback);

	//vsync

	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);

	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

	//wglSwapIntervalEXT(1);
	wglSwapIntervalEXT(0);

	memset(keys, 0, sizeof(keys));

	glViewport(0, 0, WIDTH, HEIGHT);

	init();

	GLfloat lastTime = glfwGetTime();
	GLfloat currentTime;


	do {
		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currentTime = glfwGetTime();
//		std::cout << 1.0 / (currentTime - lastTime) << std::endl;
		camera.revise();
		camera.move(keys, currentTime - lastTime, CHUNKNUMBER * CHUNKSIZE,
			CHUNKNUMBER * CHUNKSIZE);
		lastTime = currentTime;

		// Draw nothing, see you in tutorial 2 !

		render();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwWindowShouldClose(window) == 0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

