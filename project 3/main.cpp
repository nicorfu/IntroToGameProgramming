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
#include <cstdlib>
#include <ctime>
#include <vector>
#include "Entity.h"


struct GameState
{
	Entity* ship;
	Entity* platforms;
	Entity* arrow;
	Entity* explosion;
};

bool GAME_ONGOING = true;
bool MISSION_FAILED = false;

SDL_Window* g_display_window;
ShaderProgram g_program;

constexpr int WINDOW_WIDTH = 1100;
constexpr int WINDOW_HEIGHT = 710;

constexpr float BG_RED = 15.0f / 255.0f;
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
const char ARROW_FILEPATH[] = "arrow.png";
const char EXPLOSION_FILEPATH[] = "explosion.png";

glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

GameState g_state;

bool g_game_is_running = true;

const float MILLISECONDS_IN_SECOND = 1000.0f;
float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

const float GRAVITY = -1.9f;
const float THRUST = 1.7f;

int TARGET_PLATFORM;

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

int choose_random_platform()
{
	srand(static_cast<unsigned int>(time(0)));

	return rand() % PLATFORM_COUNT;
}

glm::vec3 choose_random_postion()
{
	srand(static_cast<unsigned int>(time(0)));

	int range_x = 3;

	float x_offset = float(rand() % range_x);
	float x_direction = float((rand() % 2) ? 1 : -1);

	return glm::vec3(0.0f + (x_offset * x_direction), 4.0f, 0.0f);
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

	constexpr glm::vec3 platform_scale = glm::vec3(1.2f, 3.0f, 0.0f);

	g_state.platforms = new Entity[PLATFORM_COUNT];

	for (int i = 0; i < PLATFORM_COUNT; i++)
	{
		g_state.platforms[i].m_type = PLATFORM;
		g_state.platforms[i].m_texture_id = platform_texture_id;
		g_state.platforms[i].set_scale(platform_scale);
		g_state.platforms[i].set_rotation(glm::vec3(0.0f));
		g_state.platforms[i].set_height(1.525f);
		g_state.platforms[i].set_width(1.1f);
		g_state.platforms[i].update(0.0f, NULL, 0, &GAME_ONGOING, &MISSION_FAILED);
	}

	g_state.platforms[0].set_position(glm::vec3(-3.7f, 1.7f, 0.0f));
	g_state.platforms[1].set_position(glm::vec3(3.7f, 1.7f, 0.0f));
	g_state.platforms[2].set_position(glm::vec3(0.0f, 0.0f, 0.0f));
	g_state.platforms[3].set_position(glm::vec3(-1.9f, -1.7f, 0.0f));
	g_state.platforms[4].set_position(glm::vec3(1.9f, -1.7f, 0.0f));

	GLuint ship_texture_id = load_texture(SHIP_FILEPATH);	

	constexpr float ship_scale_multiplier = 0.75f;

	constexpr glm::vec3 ship_scale = glm::vec3(1.0f, 1.2f, 0.0f) * ship_scale_multiplier;

	glm::vec3 ship_init_position = choose_random_postion();

	g_state.ship = new Entity();

	g_state.ship->m_type = PLAYER;
	g_state.ship->m_texture_id = ship_texture_id;
	g_state.ship->set_scale(ship_scale);
	g_state.ship->set_rotation(glm::vec3(0.0f));
	g_state.ship->set_position(ship_init_position);
	g_state.ship->set_width(0.4f);
	g_state.ship->set_movement(glm::vec3(0.0f));
	g_state.ship->m_speed = 1.0f;
	g_state.ship->set_acceleration(glm::vec3(0.0f, GRAVITY, 0.0f));

	GLuint arrow_texture_id = load_texture(ARROW_FILEPATH);

	constexpr glm::vec3 arrow_scale = glm::vec3(1.0f, 1.3f, 0.0f);
	constexpr glm::vec3 arrow_rotation = glm::vec3(0.0f, 0.0f, 10.4f);

	TARGET_PLATFORM = choose_random_platform();
	glm::vec3 arrow_position = glm::vec3(g_state.platforms[TARGET_PLATFORM].get_position().x + 0.75f,
		g_state.platforms[TARGET_PLATFORM].get_position().y + 1.35f, 0.0f);

	g_state.arrow = new Entity();

	g_state.arrow->m_type = ARROW;
	g_state.arrow->m_texture_id = arrow_texture_id;
	g_state.arrow->set_scale(arrow_scale);
	g_state.arrow->set_rotation(arrow_rotation);
	g_state.arrow->set_position(arrow_position);
	g_state.arrow->update(0.0f, NULL, 0, &GAME_ONGOING, &MISSION_FAILED);

	GLuint explosion_texture_id = load_texture(EXPLOSION_FILEPATH);

	constexpr float explosion_scale_multiplier = 1.5f;

	constexpr glm::vec3 explosion_scale = glm::vec3(1.0f, 1.0f, 0.0f) * explosion_scale_multiplier;

	g_state.explosion = new Entity();

	g_state.explosion->m_type = EXPLOSION;
	g_state.explosion->m_texture_id = explosion_texture_id;
	g_state.explosion->set_scale(explosion_scale);
	g_state.explosion->set_rotation(glm::vec3(0.0f));
	g_state.explosion->set_position(g_state.ship->get_position());
	g_state.explosion->update(0.0f, NULL, 0, &GAME_ONGOING, &MISSION_FAILED);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
	g_state.ship->set_acceleration(glm::vec3(0.0f, GRAVITY, 0.0f));

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

	if (GAME_ONGOING)
	{
		const Uint8* key_state = SDL_GetKeyboardState(NULL);

		if (key_state[SDL_SCANCODE_LEFT])
		{
			g_state.ship->set_acceleration(glm::vec3(-1.0f * THRUST, GRAVITY, 0.0f));
		}
		else if (key_state[SDL_SCANCODE_RIGHT])
		{
			g_state.ship->set_acceleration(glm::vec3(THRUST, GRAVITY, 0.0f));
		}
		else if (key_state[SDL_SCANCODE_SPACE])
		{
			g_state.ship->set_acceleration(glm::vec3(0.0f, 1.75f, 0.0f));
		}

		if (glm::length(g_state.ship->m_movement) > 1.0f)
		{
			g_state.ship->m_movement = glm::normalize(g_state.ship->m_movement);
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
		if (GAME_ONGOING)
		{
			g_state.explosion->set_position(g_state.ship->get_position());
		}
		g_state.explosion->update(FIXED_TIMESTEP, NULL, 0, &GAME_ONGOING, &MISSION_FAILED);

		g_state.ship->update(FIXED_TIMESTEP, g_state.platforms, PLATFORM_COUNT, &GAME_ONGOING, &MISSION_FAILED);

		for (int i = 0; i < PLATFORM_COUNT; i++)
		{
			g_state.platforms[i].update(0.0f, NULL, 0, &GAME_ONGOING, &MISSION_FAILED);
		}

		delta_time -= FIXED_TIMESTEP;
	}

	g_accumulator = delta_time;
}


void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < PLATFORM_COUNT; i++)
	{
		g_state.platforms[i].render(&g_program);
	}

	g_state.arrow->render(&g_program);

	if (MISSION_FAILED)
	{
		g_state.explosion->render(&g_program);
	}
	else
	{
		g_state.ship->render(&g_program);
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