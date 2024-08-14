/**
* Author: Nico Flores
* Assignment: Hamza's Revenge
* Date due: 2024-08-15, 1:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include <GL/glew.h>

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

#include <iostream>
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <cstdlib>
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "MenuScreen.h"
#include "Level1.h"


#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_LEFT_EDGE 5

/*
#define WALK_SFX_COUNT 2
#define HIT_SFX_COUNT 3
#define GRUNT_SFX_COUNT 4
#define PAIN_SFX_COUNT 4
*/

bool g_game_is_running = true;

bool GAME_ONGOING = false;
bool LOST = false;

SDL_Window* g_display_window;
ShaderProgram g_shader_program;

constexpr int WINDOW_WIDTH = 1100;
constexpr int WINDOW_HEIGHT = 710;

constexpr float BG_RED = 29.0f / 255.0f;
constexpr float BG_GREEN = 25.0f / 255.0f;
constexpr float BG_BLUE = 54.0f / 255.0f;
constexpr float BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0;
constexpr int VIEWPORT_Y = 0;
constexpr int VIEWPORT_WIDTH = WINDOW_WIDTH;
constexpr int VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

//constexpr int FONTBANK_SIZE = 16;

glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

const float MILLISECONDS_IN_SECOND = 1000.0f;
float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

Scene* g_current_scene;
MenuScreen* g_menu_screen;
Level1* g_level_1;

void switch_to_scene(Scene* scene)
{
	g_current_scene = scene;
	g_current_scene->initialize();
}

void initialize();
void process_input();
void update();
void render();
void shutdown();


void initialize()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	g_display_window = SDL_CreateWindow("Hamza's Revenge", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

	if (g_display_window == nullptr)
	{
		std::cerr << "Error: SDL window could not be created.\n";
		shutdown();
		exit(1);
	}

	SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
	SDL_GL_MakeCurrent(g_display_window, context);
	
	if (context == nullptr)
	{
		shutdown();
	}

	glewInit();

	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

	g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

	g_view_matrix = glm::mat4(1.0f);
	g_projection_matrix = glm::ortho(-5.0f, 5.0f, -4.0f, 4.0f, -1.0f, 1.0f);

	g_shader_program.set_projection_matrix(g_projection_matrix);
	g_shader_program.set_view_matrix(g_view_matrix);

	glUseProgram(g_shader_program.get_program_id());

	glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

	//g_level_1 = new Level1();
	//switch_to_scene(g_level_1);

	g_menu_screen = new MenuScreen();
	switch_to_scene(g_menu_screen);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
	if (g_current_scene != g_menu_screen)
	{
		g_current_scene->get_state().player->idle();
	}

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			case SDL_WINDOWEVENT_CLOSE:
				g_game_is_running = false;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						g_game_is_running = false;
						break;

					case SDLK_RETURN:
						if (g_current_scene == g_menu_screen)
						{
							GAME_ONGOING = true;
							switch_to_scene(g_level_1);
						}
						break;

					default:
						break;
				}
		}
	}

	if (GAME_ONGOING)
	{
		const Uint8* key_state = SDL_GetKeyboardState(NULL);

		if (key_state[SDL_SCANCODE_UP] || key_state[SDL_SCANCODE_W])
		{
			g_current_scene->get_state().player->set_animation_direction(UP);
			g_current_scene->get_state().player->walk();
		}
		else if (key_state[SDL_SCANCODE_DOWN] || key_state[SDL_SCANCODE_S])
		{
			g_current_scene->get_state().player->set_animation_direction(DOWN);
			g_current_scene->get_state().player->walk();
		}
		else if (key_state[SDL_SCANCODE_LEFT] || key_state[SDL_SCANCODE_A])
		{
			g_current_scene->get_state().player->set_animation_direction(LEFT);
			g_current_scene->get_state().player->walk();
		}
		else if (key_state[SDL_SCANCODE_RIGHT] || key_state[SDL_SCANCODE_D])
		{
			g_current_scene->get_state().player->set_animation_direction(RIGHT);
			g_current_scene->get_state().player->walk();
		}

		if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
		{
			(g_current_scene->get_state().player->normalize_movement());
		}
	}
}


void update()
{
	float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
	float delta_time = ticks - g_previous_ticks;
	g_previous_ticks = ticks;

	delta_time += g_accumulator;

	if (delta_time < FIXED_TIMESTEP)
	{
		g_accumulator = delta_time;

		return;
	}

	while (delta_time >= FIXED_TIMESTEP)
	{
		g_current_scene->update(FIXED_TIMESTEP);

		delta_time -= FIXED_TIMESTEP;
	}

	g_accumulator = delta_time;

	g_view_matrix = glm::mat4(1.0f);

	if (g_current_scene != g_menu_screen)
	{
		if (g_current_scene->get_state().player->get_position().x >= LEVEL1_LEFT_EDGE &&
			g_current_scene->get_state().player->get_position().y <= -3.175)
		{
			g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x,
				-g_current_scene->get_state().player->get_position().y, 0));
		}
		else if (g_current_scene->get_state().player->get_position().x < LEVEL1_LEFT_EDGE &&
			g_current_scene->get_state().player->get_position().y <= -3.175)
		{
			g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, -g_current_scene->get_state().player->get_position().y, 0));
		}
		else if (g_current_scene->get_state().player->get_position().x >= LEVEL1_LEFT_EDGE &&
			g_current_scene->get_state().player->get_position().y > -3.175)
		{
			g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.175, 0));
		}
		else
		{
			g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5.0f, 3.175f, 0.0f));
		}
	}
}


void render()
{
	g_shader_program.set_view_matrix(g_view_matrix);

	glClear(GL_COLOR_BUFFER_BIT);

	g_current_scene->render(&g_shader_program);

	SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
	SDL_Quit();

	delete g_level_1;
}


int main(int argc, char* argv[])
{
	initialize();

	while (g_game_is_running)
	{
		process_input();
		update();
		render();
	}

	shutdown();

	return 0;
}