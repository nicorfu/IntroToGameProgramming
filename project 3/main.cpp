/**
* Author: Nico Flores
* Assignment: Lunar Lander
* Date due: 2024-07-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include <GL/glew.h>

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define PLATFORM_COUNT 5
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
#include <vector>
#include "Entity.h"


struct GameState
{
	Entity* ship;
	Entity* platforms;
};

SDL_Window* g_display_window;
ShaderProgram g_program;

constexpr int WINDOW_WIDTH = 1100;
constexpr int WINDOW_HEIGHT = 710;

constexpr float BG_RED = 13.0f / 255.0f;
constexpr float BG_GREEN = 4.0f / 255.0f;
constexpr float BG_BLUE = 4.0f / 255.0f;
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

const char SHIP_FILEPATH[] = "ship.png";
const char PLATFORM_FILEPATH[] = "platform.png";

glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

GameState g_state;

bool g_game_is_running = true;

const float MILLISECONDS_IN_SECOND = 1000.0f;
float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

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

	g_program.load(V_SHADER_PATH, F_SHADER_PATH);

	g_view_matrix = glm::mat4(1.0f);
	g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	g_program.set_projection_matrix(g_projection_matrix);
	g_program.set_view_matrix(g_view_matrix);

	glUseProgram(g_program.get_program_id());

	glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

	GLuint platform_texture_id = load_texture(PLATFORM_FILEPATH);

	g_state.platforms = new Entity[PLATFORM_COUNT];

	g_state.platforms[PLATFORM_COUNT - 1].m_texture_id = platform_texture_id;
	g_state.platforms[PLATFORM_COUNT - 1].set_position(glm::vec3(-1.5f, -2.35f, 0.0f));
	g_state.platforms[PLATFORM_COUNT - 1].set_width(0.4f);
	g_state.platforms[PLATFORM_COUNT - 1].update(0.0f, NULL, 0);

	for (int i = 0; i < PLATFORM_COUNT - 2; i++)
	{
		g_state.platforms[i].m_texture_id = platform_texture_id;
		g_state.platforms[i].set_position(glm::vec3(i - 1.0f, -3.0f, 0.0f));
		g_state.platforms[i].set_width(0.4f);
		g_state.platforms[i].update(0.0f, NULL, 0);
	}

	g_state.platforms[PLATFORM_COUNT - 2].m_texture_id = platform_texture_id;
	g_state.platforms[PLATFORM_COUNT - 2].set_position(glm::vec3(2.5f, -2.5f, 0.0f));
	g_state.platforms[PLATFORM_COUNT - 2].set_width(0.4f);
	g_state.platforms[PLATFORM_COUNT - 2].update(0.0f, NULL, 0);

	GLuint ship_texture_id = load_texture(SHIP_FILEPATH);

	g_state.ship = new Entity();

	g_state.ship->m_texture_id = ship_texture_id;
	g_state.ship->set_position(glm::vec3(0.0f));
	g_state.ship->set_width(0.4f);
	g_state.ship->set_movement(glm::vec3(0.0f));
	g_state.ship->m_speed = 1.0f;
	g_state.ship->set_acceleration(glm::vec3(0.0f, -4.7f, 0.0f));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
	g_state.ship->set_movement(glm::vec3(0.0f));

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
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						g_game_is_running = false;
						break;

				default:
					break;
			}
		}
	}

	const Uint8* key_state = SDL_GetKeyboardState(NULL);

	if (key_state[SDL_SCANCODE_LEFT])
	{
		g_state.ship->m_movement.x = -1.0f;
	}
	else if (key_state[SDL_SCANCODE_RIGHT])
	{
		g_state.ship->m_movement.x = 1.0f;
	}

	if (glm::length(g_state.ship->m_movement) > 1.0f)
	{
		g_state.ship->m_movement = glm::normalize(g_state.ship->m_movement);
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
		g_state.ship->update(FIXED_TIMESTEP, g_state.platforms, PLATFORM_COUNT);
		delta_time -= FIXED_TIMESTEP;
	}

	g_accumulator = delta_time;
}


void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	g_state.ship->render(&g_program);

	for (int i = 0; i < PLATFORM_COUNT; i++)
	{
		g_state.platforms[i].render(&g_program);
	}

	SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
	SDL_Quit();

	delete[] g_state.platforms;
	delete g_state.ship;
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