/**
* Author: Nico Flores
* Assignment: Rise of the AI
* Date due: 2024-7-27, 11:59pm
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

constexpr int WINDOW_WIDTH = 1100;
constexpr int WINDOW_HEIGHT = 710;

constexpr float BG_RED = 11.0f / 255.0f;
constexpr float BG_GREEN = 4.0f / 255.0f;
constexpr float BG_BLUE = 4.0f / 255.0f;
constexpr float BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0;
constexpr int VIEWPORT_Y = 0;
constexpr int VIEWPORT_WIDTH = WINDOW_WIDTH;
constexpr int VIEWPORT_HEIGHT = WINDOW_HEIGHT;

void initialize();
void process_input();
void update();
void render();
void shutdown();


void initialize()
{
	SDL_Init(SDL_INIT_VIDEO);

	g_display_window = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

	if (g_display_window == nullptr)
	{
		std::cerr << "Error: SDL window could not be created.\n";
		shutdown();
		exit(1);
	}

	SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
	SDL_GL_MakeCurrent(g_display_window, context);

	glewInit();

	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

	glUseProgram(g_shader_program.get_program_id());

	glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
		{
			g_app_status = TERMINATED;
		}
	}
}


void update()
{

}


void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(g_display_window);
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