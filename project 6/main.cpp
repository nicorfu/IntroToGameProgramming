/**
* Author: Nico Flores
* Assignment: [Your game's name here]
* Date due: 2024-08-15, 1:00pm
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
#define ENEMY_COUNT 3
/*
#define WALK_SFX_COUNT 2
#define HIT_SFX_COUNT 3
#define GRUNT_SFX_COUNT 4
#define PAIN_SFX_COUNT 4
*/


struct GameState
{
	Entity* player;

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

constexpr float BG_RED = 221.0f / 255.0f;
constexpr float BG_GREEN = 18.0f / 255.0f;
constexpr float BG_BLUE = 25.0f / 255.0f;
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

//const char MAP_TILESET_FILEPATH[] = "assets/visual/.png";
const float TILE_SIZE = 1.0f;
const int TILE_COUNT_X = 0.0f;
const int TILE_COUNT_Y = 0.0f;

const int LEVEL_WIDTH = 10;
const int LEVEL_HEIGHT = 5;
unsigned int LEVEL_DATA[] =
{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

constexpr int FONTBANK_SIZE = 16;

constexpr int CD_QUAL_FREQ = 44100;
constexpr int AUDIO_CHAN_AMT = 2;
constexpr int AUDIO_BUFF_SIZE = 512;

constexpr int PLAY_ONCE = 0;
constexpr int NEXT_CHNL = -1;
constexpr int MUTE_VOL = 0;
constexpr int MILS_IN_SEC = 1000;
constexpr int ALL_SFX_CHN = -1;

//const char PLAYER_FILEPATH[] = "assets/visual/.png";
//const char ENEMY_FILEPATH[] = "assets/visual/.png";

const char FONTSHEET_FILEPATH[] = "assets/visual/font.png";

glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

const float MILLISECONDS_IN_SECOND = 1000.0f;
float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

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
	SDL_Init(SDL_INIT_VIDEO);

	g_display_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

			default:
				break;
			}
		}
	}

	/*
	if (GAME_ONGOING)
	{
		const Uint8* key_state = SDL_GetKeyboardState(NULL);

		if (key_state[SDL_SCANCODE_LEFT])
		{
		}
		else if (key_state[SDL_SCANCODE_RIGHT])
		{
		}
		else if (key_state[SDL_SCANCODE_SPACE])
		{
		}

		if (glm::length(g_state.player->m_movement) > 1.0f)
		{
			g_state.player->m_movement = glm::normalize(g_state.player->m_movement);
		}
	}
	*/
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

	delete g_state.player;
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