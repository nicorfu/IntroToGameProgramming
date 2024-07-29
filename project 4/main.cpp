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
#define ENEMY_COUNT 3
#define WALK_SFX_COUNT 2
#define HIT_SFX_COUNT 3
#define GRUNT_SFX_COUNT 4
#define PAIN_SFX_COUNT 4
#define GRAVITY -9.81
#define GRAVITY_FACTOR 1.0


struct GameState
{
	Entity* player;
	Entity* enemies;

	Entity* text;

	Map* map;

	Mix_Chunk* jump_sfx;
	Mix_Chunk* land_sfx;
	Mix_Chunk* walk_sfx[WALK_SFX_COUNT];
	Mix_Chunk* punch_sfx[HIT_SFX_COUNT];
	Mix_Chunk* skull_sfx[HIT_SFX_COUNT];
	Mix_Chunk* osiris_grunt_sfx[GRUNT_SFX_COUNT];
	Mix_Chunk* irina_grunt_sfx[GRUNT_SFX_COUNT];
	Mix_Chunk* osiris_pain_sfx[PAIN_SFX_COUNT];
	Mix_Chunk* irina_pain_sfx[PAIN_SFX_COUNT];
};

GameState g_state;

bool g_game_is_running = true;

bool GAME_ONGOING = true;
bool LOST = false;

SDL_Window* g_display_window;
ShaderProgram g_shader_program;

constexpr int WINDOW_WIDTH = 1100;
constexpr int WINDOW_HEIGHT = 710;

constexpr float BG_RED = 43.0f / 255.0f;
constexpr float BG_GREEN = 76.0f / 255.0f;
constexpr float BG_BLUE = 115.0f / 255.0f;
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

const int LEVEL_WIDTH = 26;
const int LEVEL_HEIGHT = 6;
unsigned int LEVEL_DATA[] =
{
	 52,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   68,  180,   70,   -1,   -1,
	 94,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,   1,    2,    1,    2,    1,    3,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	 94,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   21, 255,  254,   22,  255,  255,  252,  138,    1,    3,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  0,    1,    2,    1,    2,    1,    2,    1,  139,  213, 212,  275,  255,  254,  273,  231,  275,   255,   24,  -1,   -1,   -1,   -1,   -1,   -1,   -1,
	 21,   22,   22,   22,   22,   22,   22,   22,   22,  276, 253,   22,   22,   22,   22,   22,   22,   22,  252,  138,    3,   -1,   -1,   -1,   -1,   -1,
	 22,   22,   22,   22,   22,   22,   22,   22,   22,   22,  22,   22,   22,   22,    22,  22,   22,   22,   22,   22,   22,  - 1,   -1,   -1,   -1,   -1
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

const char PLAYER_FILEPATH[] = "assets/visual/osiris_spritesheet.png";
const char ENEMY_FILEPATH[] = "assets/visual/irina_spritesheet.png";

const char FONTSHEET_FILEPATH[] = "assets/visual/font.png";

const char JUMP_SFX_FILEPATH[] = "assets/audio/jump.wav";
const char LAND_SFX_FILEPATH[] = "assets/audio/land.wav";
const char WALK1_SFX_FILEPATH[] = "assets/audio/grass_walk_1.wav";
const char WALK2_SFX_FILEPATH[] = "assets/audio/grass_walk_2.wav";
const char PUNCH1_SFX_FILEPATH[] = "assets/audio/punch_1.wav";
const char PUNCH2_SFX_FILEPATH[] = "assets/audio/punch_2.wav";
const char PUNCH3_SFX_FILEPATH[] = "assets/audio/punch_3.wav";
const char SKULL1_SFX_FILEPATH[] = "assets/audio/bone_hit_1.wav";
const char SKULL2_SFX_FILEPATH[] = "assets/audio/bone_hit_2.wav";
const char SKULL3_SFX_FILEPATH[] = "assets/audio/bone_hit_3.wav";
const char OSIRISGRUNT1_SFX_FILEPATH[] = "assets/audio/osiris_grunt_1.wav";
const char OSIRISGRUNT2_SFX_FILEPATH[] = "assets/audio/osiris_grunt_2.wav";
const char OSIRISGRUNT3_SFX_FILEPATH[] = "assets/audio/osiris_grunt_3.wav";
const char OSIRISGRUNT4_SFX_FILEPATH[] = "assets/audio/osiris_grunt_4.wav";
const char IRINAGRUNT1_SFX_FILEPATH[] = "assets/audio/irina_grunt_1.wav";
const char IRINAGRUNT2_SFX_FILEPATH[] = "assets/audio/irina_grunt_2.wav";
const char IRINAGRUNT3_SFX_FILEPATH[] = "assets/audio/irina_grunt_3.wav";
const char IRINAGRUNT4_SFX_FILEPATH[] = "assets/audio/irina_grunt_4.wav";
const char OSIRISPAIN1_SFX_FILEPATH[] = "assets/audio/osiris_pain_1.wav";
const char OSIRISPAIN2_SFX_FILEPATH[] = "assets/audio/osiris_pain_2.wav";
const char OSIRISPAIN3_SFX_FILEPATH[] = "assets/audio/osiris_pain_3.wav";
const char OSIRISPAIN4_SFX_FILEPATH[] = "assets/audio/osiris_pain_4.wav";
const char IRINAPAIN1_SFX_FILEPATH[] = "assets/audio/irina_pain_1.wav";
const char IRINAPAIN2_SFX_FILEPATH[] = "assets/audio/irina_pain_2.wav";
const char IRINAPAIN3_SFX_FILEPATH[] = "assets/audio/irina_pain_3.wav";
const char IRINAPAIN4_SFX_FILEPATH[] = "assets/audio/irina_pain_4.wav";

glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

const float MILLISECONDS_IN_SECOND = 1000.0f;
float g_previous_ticks = 0.0f;
float g_curr_ticks = 0.0f;
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

	Mix_OpenAudio(CD_QUAL_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHAN_AMT, AUDIO_BUFF_SIZE);

	g_state.jump_sfx = Mix_LoadWAV(JUMP_SFX_FILEPATH);
	Mix_VolumeChunk(g_state.jump_sfx, int(MIX_MAX_VOLUME * 0.6));

	g_state.land_sfx = Mix_LoadWAV(LAND_SFX_FILEPATH);
	Mix_VolumeChunk(g_state.land_sfx, int(MIX_MAX_VOLUME * 0.75));

	g_state.walk_sfx[0] = Mix_LoadWAV(WALK1_SFX_FILEPATH);
	g_state.walk_sfx[1] = Mix_LoadWAV(WALK2_SFX_FILEPATH);
	for (int i = 0; i < WALK_SFX_COUNT; i++)
	{
		Mix_VolumeChunk(g_state.walk_sfx[i], int(MIX_MAX_VOLUME * 0.1));
	}

	g_state.punch_sfx[0] = Mix_LoadWAV(PUNCH1_SFX_FILEPATH);
	g_state.punch_sfx[1] = Mix_LoadWAV(PUNCH2_SFX_FILEPATH);
	g_state.punch_sfx[2] = Mix_LoadWAV(PUNCH3_SFX_FILEPATH);
	for (int i = 0; i < HIT_SFX_COUNT; i++)
	{
		Mix_VolumeChunk(g_state.punch_sfx[i], int(MIX_MAX_VOLUME * 1.0));
	}

	g_state.skull_sfx[0] = Mix_LoadWAV(SKULL1_SFX_FILEPATH);
	g_state.skull_sfx[1] = Mix_LoadWAV(SKULL2_SFX_FILEPATH);
	g_state.skull_sfx[2] = Mix_LoadWAV(SKULL3_SFX_FILEPATH);
	for (int i = 0; i < HIT_SFX_COUNT; i++)
	{
		Mix_VolumeChunk(g_state.skull_sfx[i], int(MIX_MAX_VOLUME * 0.7));
	}

	g_state.osiris_grunt_sfx[0] = Mix_LoadWAV(OSIRISGRUNT1_SFX_FILEPATH);
	g_state.osiris_grunt_sfx[1] = Mix_LoadWAV(OSIRISGRUNT2_SFX_FILEPATH);
	g_state.osiris_grunt_sfx[2] = Mix_LoadWAV(OSIRISGRUNT3_SFX_FILEPATH);
	g_state.osiris_grunt_sfx[3] = Mix_LoadWAV(OSIRISGRUNT4_SFX_FILEPATH);
	for (int i = 0; i < GRUNT_SFX_COUNT; i++)
	{
		Mix_VolumeChunk(g_state.osiris_grunt_sfx[i], int(MIX_MAX_VOLUME * 0.8));
	}

	g_state.irina_grunt_sfx[0] = Mix_LoadWAV(IRINAGRUNT1_SFX_FILEPATH);
	g_state.irina_grunt_sfx[1] = Mix_LoadWAV(IRINAGRUNT2_SFX_FILEPATH);
	g_state.irina_grunt_sfx[2] = Mix_LoadWAV(IRINAGRUNT3_SFX_FILEPATH);
	g_state.irina_grunt_sfx[3] = Mix_LoadWAV(IRINAGRUNT4_SFX_FILEPATH);
	for (int i = 0; i < GRUNT_SFX_COUNT; i++)
	{
		Mix_VolumeChunk(g_state.irina_grunt_sfx[i], int(MIX_MAX_VOLUME * 0.62));
	}

	g_state.osiris_pain_sfx[0] = Mix_LoadWAV(OSIRISPAIN1_SFX_FILEPATH);
	g_state.osiris_pain_sfx[1] = Mix_LoadWAV(OSIRISPAIN2_SFX_FILEPATH);
	g_state.osiris_pain_sfx[2] = Mix_LoadWAV(OSIRISPAIN3_SFX_FILEPATH);
	g_state.osiris_pain_sfx[3] = Mix_LoadWAV(OSIRISPAIN4_SFX_FILEPATH);
	for (int i = 0; i < PAIN_SFX_COUNT; i++)
	{
		Mix_VolumeChunk(g_state.osiris_pain_sfx[i], int(MIX_MAX_VOLUME * 0.8));
	}

	g_state.irina_pain_sfx[0] = Mix_LoadWAV(IRINAPAIN1_SFX_FILEPATH);
	g_state.irina_pain_sfx[1] = Mix_LoadWAV(IRINAPAIN2_SFX_FILEPATH);
	g_state.irina_pain_sfx[2] = Mix_LoadWAV(IRINAPAIN3_SFX_FILEPATH);
	g_state.irina_pain_sfx[3] = Mix_LoadWAV(IRINAPAIN4_SFX_FILEPATH);
	for (int i = 0; i < PAIN_SFX_COUNT; i++)
	{
		Mix_VolumeChunk(g_state.irina_pain_sfx[i], int(MIX_MAX_VOLUME * 1.0));
	}

	GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
	 
	g_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, TILE_SIZE, TILE_COUNT_X, TILE_COUNT_Y);

	glm::vec3 acceleration = glm::vec3(0.0f, GRAVITY * GRAVITY_FACTOR, 0.0f);

	GLuint player_texture_id = load_texture(PLAYER_FILEPATH);

	glm::vec3 player_scale = glm::vec3(1.0f, 1.25f, 0.0f) * 1.3f;
	glm::vec3 player_position = glm::vec3(4.75f, -0.5f, 0.0f);
	const float player_speed = 2.3f;
	
	int player_animation[4][8] =
	{
		{0,  0,  1,  1,  0,  0,  1,  1 },	// idle
		{16, 17, 18, 19, 16, 17, 18, 19},   // moving
		{56, 57, 58, 59, 60, 61, 62, 63},	// dying
		{66, 68, 70,  0,  0,  1,  1,  0}    // attacking
	};

	g_state.player = new Entity
	(
		PLAYER,
		player_texture_id,
		player_scale,
		player_position,
		acceleration,
		0.58f,
		1.5f,
		player_speed,
		7.0f,
		8,
		9,
		8,
		0,
		0.0f,
		player_animation,
		g_state.land_sfx,
		g_state.walk_sfx,
		g_state.punch_sfx,
		g_state.osiris_grunt_sfx,
		g_state.osiris_pain_sfx
	);

	GLuint enemy_texture_id = load_texture(ENEMY_FILEPATH);

	glm::vec3 enemy_scale = glm::vec3(1.0f, 1.25f, 0.0f) * 4.0f;

	glm::vec3 enemy1_position = glm::vec3(20.0f, -0.5f, 0.0f);
	glm::vec3 enemy2_position = glm::vec3(23.0f, 0.5f, 0.0f);
	glm::vec3 enemy3_position = glm::vec3(11.8f, 0.5f, 0.0f);

	int enemy_animation[4][8] =
	{
		{36, 39, 42, 44, 37, 40, 43, 45},	// idle
		{54, 56, 58, 60, 55, 57, 59, 61},   // moving
		{19, 21, 23, 25, 27, 29, 32, 35},	// dying
		{ 6,  8, 10, 11, 12, 40, 43, 45}    // attacking
	};

	g_state.enemies = new Entity[ENEMY_COUNT];

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		g_state.enemies[i] = Entity
		(
			ENEMY,
			enemy_texture_id,
			enemy_scale,
			glm::vec3(0.0f),
			acceleration,
			0.5f,
			1.8f,
			0.0f,
			6.3f,
			18,
			4,
			8,
			0,
			0.0f,
			enemy_animation,
			g_state.land_sfx,
			g_state.walk_sfx,
			g_state.skull_sfx,
			g_state.irina_grunt_sfx,
			g_state.irina_pain_sfx
		);
	}

	g_state.enemies[0].set_ai_type(GUARDIAN);
	g_state.enemies[0].set_ai_state(IDLING);
	g_state.enemies[0].set_position(enemy1_position);
	g_state.enemies[0].face_left();
	g_state.enemies[0].set_speed(1.5f);

	g_state.enemies[1].set_ai_type(WAITER);
	g_state.enemies[1].set_ai_state(IDLING);
	g_state.enemies[1].set_position(enemy2_position);
	g_state.enemies[1].face_left();

	g_state.enemies[2].set_ai_type(WALKER);
	g_state.enemies[2].set_ai_state(WALKING);
	g_state.enemies[2].set_position(enemy3_position);
	g_state.enemies[2].move_left();
	g_state.enemies[2].set_speed(0.95f);

	GLuint font_texture_id = load_texture(FONTSHEET_FILEPATH);
	g_state.text = new Entity();
	g_state.text->set_texture_id(font_texture_id);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
	if (!g_state.player->get_dying())
	{
		g_state.player->dont_move();
	}
	else
	{
		GAME_ONGOING = false;
		LOST = true;
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

					case SDLK_UP:
					case SDLK_SPACE:
						if (GAME_ONGOING && g_state.player->get_collided_bottom())
						{
							Mix_PlayChannel(1, g_state.jump_sfx, 0);
							g_state.player->jump();
						}
						break;

					case SDLK_f:
						if (GAME_ONGOING && (g_curr_ticks - g_state.player->get_last_attack_time()) >= 0.7f &&
							(g_state.player->get_velocity().x == 0.0f))
						{
							g_state.player->attack(g_state.enemies, ENEMY_COUNT);

							g_state.player->set_last_attack_time(g_curr_ticks);
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
	g_curr_ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
	float delta_time = g_curr_ticks - g_previous_ticks;
	g_previous_ticks = g_curr_ticks;

	delta_time += g_accumulator;

	if (delta_time < FIXED_TIMESTEP)
	{
		g_accumulator = delta_time;

		return;
	}

	while (delta_time >= FIXED_TIMESTEP)
	{
		g_state.player->update(FIXED_TIMESTEP, g_state.player, g_state.enemies, ENEMY_COUNT, g_state.map, g_curr_ticks);

		bool one_enemy_active = false;
		for (int i = 0; i < ENEMY_COUNT; i++)
		{
			g_state.enemies[i].update(FIXED_TIMESTEP, g_state.player, g_state.player, 1, g_state.map, g_curr_ticks);

			if (g_state.enemies[i].get_is_active())
			{
				one_enemy_active = true;
			}
		}

		if (!one_enemy_active)
		{
			GAME_ONGOING = false;
		}

		delta_time -= FIXED_TIMESTEP;
	}

	g_accumulator = delta_time;

	
	if (g_state.player->get_position().x >= 4.75f)
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

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		g_state.enemies[i].render(&g_shader_program);
	}

	g_state.player->render(&g_shader_program);

	if (!GAME_ONGOING)
	{
		if (!LOST)
		{
			g_state.text->draw_text(&g_shader_program, "YOU WON...", 0.4f, 0.0001f, 
				glm::vec3(g_state.player->get_position().x, 2.9f, 0.0f), FONTBANK_SIZE);
		}
		else
		{
			g_state.text->draw_text(&g_shader_program, "YOU LOST LMAOO", 0.4f, 0.0001f, 
				glm::vec3(g_state.player->get_position().x, 2.9f, 0.0f), FONTBANK_SIZE);
		}
	}

	SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
	SDL_Quit();

	delete g_state.map;
	delete g_state.player;
	delete [] g_state.enemies;

	Mix_FreeChunk(g_state.jump_sfx);
	Mix_FreeChunk(g_state.land_sfx);
	Mix_FreeChunk(*g_state.walk_sfx);
	Mix_FreeChunk(*g_state.punch_sfx);
	Mix_FreeChunk(*g_state.skull_sfx);
	Mix_FreeChunk(*g_state.osiris_grunt_sfx);
	Mix_FreeChunk(*g_state.irina_grunt_sfx);
	Mix_FreeChunk(*g_state.osiris_pain_sfx);
	Mix_FreeChunk(*g_state.irina_pain_sfx);
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