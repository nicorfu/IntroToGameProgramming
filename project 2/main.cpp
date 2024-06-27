/**
* Author: Nico Flores
* Assignment: Pong Clone
* Date due: 2024-06-29, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include <GL/glew.h>

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>

enum AppStatus { RUNNING, TERMINATED };

AppStatus g_app_status = RUNNING;
SDL_Window* g_display_window;
ShaderProgram g_shader_program;


void initialize();
void process_input();
void update();
void render();
void shutdown();


void initialize()
{

}


void process_input()
{

}


void update()
{

}


void render()
{

}


void shutdown()
{
	SDL_Quit();
}


int main(int argc, char* argv[])
{
	initialize();

	while (g_app_status == RUNNING)
	{
		process_input();
		update();
		render();
	}

	shutdown();

	return 0;
}