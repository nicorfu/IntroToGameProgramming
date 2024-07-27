/**
* Author: Nico Flores
* Assignment: Rise of the AI
* Date due: 2024-7-28, 11:59pm
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
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <cstdlib>
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"

#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 1
#define GRAVITY -9.81
#define GRAVITY_FACTOR 0.95


struct GameState
{
	Entity* player;
	Entity* enemies;

	Map* map;
};

GameState g_state;

bool g_game_is_running = true;

bool GAME_ONGOING = true;
bool LOST = false;

SDL_Window* g_display_window;
ShaderProgram g_shader_program;

constexpr int WINDOW_WIDTH = 1100;
constexpr int WINDOW_HEIGHT = 710;

constexpr float BG_RED = 0.0f / 255.0f;
constexpr float BG_GREEN = 202.0f / 255.0f;
constexpr float BG_BLUE = 251.0f / 255.0f;
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

const char MAP_TILESET_FILEPATH[] = "assets/visual/oak_woods_tileset.png";
const float TILE_SIZE = 1.0f;
const int TILE_COUNT_X = 21;
const int TILE_COUNT_Y = 15;

const int LEVEL_WIDTH = 20;
const int LEVEL_HEIGHT = 4;
unsigned int LEVEL_DATA[] =
{
	-1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0,   1,   2,   1,   2,   1,   2,   1,   2,   1,  2,  1,  2,  1,  2,  1,  2,  1,  2,  3,
	21,  22,  22,  22,  22,  22,  22,  22,  22,  22, 22,  22,  22,  22,  22,  22,  22,  22,  22,  22
};

const char PLAYER_FILEPATH[] = "assets/visual/osiris_spritesheet.png";

const char FONTSHEET_FILEPATH[] = "assets/visual/font.png";

glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

const float MILLISECONDS_IN_SECOND = 1000.0f;
float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

constexpr int FONTBANK_SIZE = 16;

constexpr int CD_QUAL_FREQ = 44100;
constexpr int AUDIO_CHAN_AMT = 2;
constexpr int AUDIO_BUFF_SIZE = 4096;

GLuint load_texture(const char* filepath);
void initialize();
void process_input();
void update();
void render();
void shutdown();


GLuint load_texture(const char* filepath)
{
	int width;
	int height;
	int num_components;

	unsigned char* image = stbi_load(filepath, &width, &height, &num_components, STBI_rgb_alpha);

	if (image == NULL)
	{
		LOG("Can't load image. Smh. Check filepath.");
		assert(false);
	}

	GLuint textureID;
	glGenTextures(NUMBER_OF_TEXTURES, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
				 GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(image);

	return textureID;
}


void initialize()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	g_display_window = SDL_CreateWindow("Rise of the AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

	if (g_display_window == nullptr)
	{
		LOG("Error: SDL window could not be created.\n");
		shutdown();
		exit(1);
	}

	SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
	SDL_GL_MakeCurrent(g_display_window, context);
	if (context == nullptr)
	{
		LOG("ERROR: Could not create OpenGL context.\n");
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

	GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
	 
	g_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, TILE_SIZE, TILE_COUNT_X, TILE_COUNT_Y);

	glm::vec3 acceleration = glm::vec3(0.0f, GRAVITY * GRAVITY_FACTOR, 0.0f);

	GLuint player_texture_id = load_texture(PLAYER_FILEPATH);

	glm::vec3 player_scale = glm::vec3(1.0f, 1.25f, 0.0f) * 1.3f;
	glm::vec3 player_position = glm::vec3(4.5f, -0.5f, 0.0f);
	const float player_speed = 2.3f;
	
	int player_animation[3][8] =
	{
		{0,  0,  1,  1,  0,  0,  1,  1 },	// idle
		{16, 17, 18, 19, 16, 17, 18, 19},   // moving
		{56, 57, 58, 59, 60, 61, 62, 63}	// dying
	};

	g_state.player = new Entity
	(
		PLAYER,
		player_texture_id,
		player_scale,
		player_position,
		acceleration,
		1.0f,
		1.5f,
		player_speed,
		5.2f,
		8,
		9,
		8,
		0,
		0.0f,
		player_animation
	);

	//GLuint enemy_texture_id = load_texture(ENEMY_FILEPATH);

	//g_state.enemies = new Entity();

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		//g_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
		//g_state.enemies[i].set_jump_power(3.0f);
	}

	/*
	g_state.enemies[0].set_position(glm::vec3(3.0f, 0.0f, 0.0f));
	g_state.enemies[0].set_movement(glm::vec3(0.0f));
	g_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
	g_state.enemies[0].set_entity_type(ENEMY);
	g_state.enemies[0].set_ai_type(GUARD);
	g_state.enemies[0].set_ai_state(IDLE);
	*/

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
	g_state.player->dont_move();

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

					case SDLK_UP:
					case SDLK_SPACE:
						if (g_state.player->get_collided_bottom())
						{
							g_state.player->jump();
						}
						break;

					default:
						break;
				}

			default:
				break;
		}
	}

	if (GAME_ONGOING)
	{
		const Uint8* key_state = SDL_GetKeyboardState(NULL);

		if (key_state[SDL_SCANCODE_LEFT] || key_state[SDL_SCANCODE_A])
		{
			g_state.player->move_left();
		}
		else if (key_state[SDL_SCANCODE_RIGHT] || key_state[SDL_SCANCODE_D])
		{
			g_state.player->move_right();
		}

		if (glm::length(g_state.player->get_movement()) > 1.0f)
		{
			g_state.player->normalize_movement();
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
		g_state.player->update(FIXED_TIMESTEP, g_state.player, NULL, 0, g_state.map);

		for (int i = 0; i < ENEMY_COUNT; i++)
		{
			//g_state.enemies[i].update(FIXED_TIMESTEP, g_state.player, g_state.platforms, PLATFORM_COUNT);
		}

		delta_time -= FIXED_TIMESTEP;
	}

	g_accumulator = delta_time;

	
	if (g_state.player->get_position().x >= 4.5f)
	{
		g_view_matrix = glm::mat4(1.0f);
		g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_state.player->get_position().x, 0.0f, 0.0f));
	}
}


void render()
{
	g_shader_program.set_view_matrix(g_view_matrix);

	glClear(GL_COLOR_BUFFER_BIT);

	g_state.map->render(&g_shader_program);
	g_state.player->render(&g_shader_program);

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		//g_state.enemies[i].render(&g_shader_program);
	}

	SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
	SDL_Quit();

	delete g_state.map;
	delete g_state.player;
	//delete [] g_state.enemies;
	delete g_state.enemies;
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