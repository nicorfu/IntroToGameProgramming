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
#include <cstdlib>


enum AppStatus { RUNNING, TERMINATED };
AppStatus g_app_status = RUNNING;

SDL_Window* g_display_window;
ShaderProgram g_shader_program;

constexpr int WINDOW_WIDTH = 1100;
constexpr int WINDOW_HEIGHT = 710;

constexpr float BG_RED = 0.3333f;
constexpr float BG_GREEN = 0.4196f;
constexpr float BG_BLUE = 0.1843f;
constexpr float BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0;
constexpr int VIEWPORT_Y = 0;
constexpr int VIEWPORT_WIDTH = WINDOW_WIDTH;
constexpr int VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
constexpr char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr char LINE_SPRITE_FILEPATH[] = "line.png";
constexpr char CIRCLE_SPRITE_FILEPATH[] = "circle.png";
constexpr char DOT_SPRITE_FILEPATH[] = "dot.png";
constexpr char ALEXIS_SPRITE_FILEPATH[] = "alexis.png";
constexpr char MESSI_SPRITE_FILEPATH[] = "messi.png";
constexpr char BALL_SPRITE_FILEPATH[] = "ball.png";

GLuint g_line_texture_id;
GLuint g_circle_texture_id;
GLuint g_dot_texture_id;
GLuint g_alexis_texture_id;
GLuint g_messi_texture_id;
GLuint g_ball_texture_id;

constexpr float LINE_SCALE_MULTIPLIER = 1.125f;
constexpr float CIRCLE_SCALE_MULTIPLIER = 0.3f;
constexpr float DOT_SCALE_MULTIPLIER = 0.009f;
constexpr float ALEXIS_SCALE_MULTIPLIER = 0.25f;
constexpr float MESSI_SCALE_MULTIPLIER = 0.24f;
constexpr float BALL_SCALE_MULTIPLIER = 0.095f;

constexpr glm::vec3 INIT_SCALE_LINE = glm::vec3(1.0f, 6.6666f, 0.000f) * LINE_SCALE_MULTIPLIER;
constexpr glm::vec3 INIT_SCALE_CIRCLE = glm::vec3(9.2f, 10.0f, 0.000f) * CIRCLE_SCALE_MULTIPLIER;
constexpr glm::vec3 INIT_SCALE_DOT = glm::vec3(9.2f, 10.0f, 0.000f) * DOT_SCALE_MULTIPLIER;
constexpr glm::vec3 INIT_SCALE_ALEXIS = glm::vec3(8.7f, 13.9752f, 0.000f) * ALEXIS_SCALE_MULTIPLIER;
constexpr glm::vec3 INIT_SCALE_MESSI = glm::vec3(8.7f, 10.0f, 0.000f) * MESSI_SCALE_MULTIPLIER;
constexpr glm::vec3 INIT_SCALE_BALL = glm::vec3(9.0f, 10.0f, 0.000f) * BALL_SCALE_MULTIPLIER;

constexpr glm::vec3 INIT_POS_LINE = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_CIRCLE = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_DOT = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_ALEXIS = glm::vec3(-4.1f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_MESSI = glm::vec3(4.0f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_BALL = glm::vec3(0.0f, 0.0f, 0.0f);

glm::mat4 g_view_matrix;
glm::mat4 g_line_matrix;
glm::mat4 g_circle_matrix;
glm::mat4 g_dot_matrix;
glm::mat4 g_alexis_matrix;
glm::mat4 g_messi_matrix;
glm::mat4 g_ball_matrix;
glm::mat4 g_projection_matrix;
glm::mat4 g_translation_matrix;

constexpr glm::vec3 G_POSITION_LINE = INIT_POS_LINE;

constexpr glm::vec3 G_POSITION_CIRCLE = INIT_POS_CIRCLE;

constexpr glm::vec3 G_POSITION_DOT = INIT_POS_DOT;

glm::vec3 g_position_alexis = INIT_POS_ALEXIS;
glm::vec3 g_movement_alexis = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_position_messi = INIT_POS_MESSI;
glm::vec3 g_movement_messi = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_position_ball = INIT_POS_BALL;
glm::vec3 g_movement_ball = glm::vec3(0.0f, 0.0f, 0.0f);

constexpr float g_alexis_speed = 4.0f;

float g_messi_speed = 4.0f;

float g_ball_speed = 4.0f;

constexpr float SECOND_IN_MILLISECONDS = 1000.0f;
float g_previous_ticks = 0.0f;

enum GameMode { ONE_PLAYER, TWO_PLAYER };
GameMode g_gamemode = TWO_PLAYER;

enum GameStatus { ONGOING, OVER};
GameStatus g_game_status = ONGOING;

enum MessiDirection { UP, DOWN };
MessiDirection g_messi_dir = DOWN;

constexpr float ALEXIS_HEAD_WIDTH = INIT_SCALE_ALEXIS.x * 0.333f;
constexpr float ALEXIS_HEAD_HEIGHT = INIT_SCALE_ALEXIS.y * 0.4f;
constexpr float ALEXIS_TORSO_HEIGHT = INIT_SCALE_ALEXIS.y * 0.55f;

float g_position_alexis_head_y;
float g_position_alexis_torso_y;

float x_distance_ball_alexis_head;
float y_distance_ball_alexis_head;
float x_distance_ball_alexis_torso;
float y_distance_ball_alexis_torso;

float g_prev_border_hit_time = 0.0f;
float g_curr_border_hit_time;

float g_prev_collision_time = 0.0f;
float g_curr_collision_time;

float x_distance_ball_messi;
float y_distance_ball_messi;

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

	stbi_image_free(image);

	return textureID;
}


void randomize_ball_dir()
{
	srand(time(nullptr));

	bool negative = rand() % 2;

	g_movement_ball.x = -1.0f;
	g_movement_ball.y = (negative) ? -1.0f : 1.0f;
}


void initialize()
{
	SDL_Init(SDL_INIT_VIDEO);

	g_display_window = SDL_CreateWindow("FutPong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

	g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

	g_line_matrix = glm::mat4(1.0f);
	g_circle_matrix = glm::mat4(1.0f);
	g_dot_matrix = glm::mat4(1.0f);
	g_alexis_matrix = glm::mat4(1.0f);
	g_messi_matrix = glm::mat4(1.0f);
	g_ball_matrix = glm::mat4(1.0f);

	g_view_matrix = glm::mat4(1.0f);
	g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	g_shader_program.set_projection_matrix(g_projection_matrix);
	g_shader_program.set_view_matrix(g_view_matrix);

	glUseProgram(g_shader_program.get_program_id());

	glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

	g_line_texture_id = load_texture(LINE_SPRITE_FILEPATH);
	g_circle_texture_id = load_texture(CIRCLE_SPRITE_FILEPATH);
	g_dot_texture_id = load_texture(DOT_SPRITE_FILEPATH);
	g_alexis_texture_id = load_texture(ALEXIS_SPRITE_FILEPATH);
	g_messi_texture_id = load_texture(MESSI_SPRITE_FILEPATH);
	g_ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	randomize_ball_dir();
}


void process_input()
{
	g_movement_alexis = glm::vec3(0.0f);
	g_movement_messi = glm::vec3(0.0f);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			case SDL_WINDOWEVENT_CLOSE:
				g_app_status = TERMINATED;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_w:
						g_movement_alexis.y = 1.0f;
						break;

					case SDLK_s:
						g_movement_alexis.y = -1.0f;
						break;

					case SDLK_UP:
						if (g_gamemode == TWO_PLAYER)
						{
							g_movement_messi.y = 1.0f;

						}
						break;

					case SDLK_DOWN:
						if (g_gamemode == TWO_PLAYER)
						{
							g_movement_messi.y = -1.0f;

						}
						break;

					case SDLK_t:
						g_gamemode = (g_gamemode == TWO_PLAYER) ? ONE_PLAYER : TWO_PLAYER;
						break;

					case SDLK_ESCAPE:
						g_app_status = TERMINATED;
						break;

					default:
						break;
				}

			default:
				break;
		}
	}

	const Uint8* key_state = SDL_GetKeyboardState(NULL);

	if (key_state[SDL_SCANCODE_W])
	{
		g_movement_alexis.y = 1.0f;
	}
	else if (key_state[SDL_SCANCODE_S])
	{
		g_movement_alexis.y = -1.0f;
	}


	if (g_gamemode == TWO_PLAYER)
	{
		if (key_state[SDL_SCANCODE_UP])
		{
			g_movement_messi.y = 1.0f;
		}
		else if (key_state[SDL_SCANCODE_DOWN])
		{
			g_movement_messi.y = -1.0f;
		}
	}
}


void calculate_ball_collision_distances()
{
	x_distance_ball_alexis_torso = fabs(g_position_ball.x - g_position_alexis.x) -
								 ((INIT_SCALE_BALL.x + INIT_SCALE_ALEXIS.x) / 2.0f);
	y_distance_ball_alexis_torso = fabs(g_position_ball.y - g_position_alexis_torso_y) -
								 ((INIT_SCALE_BALL.y + ALEXIS_TORSO_HEIGHT) / 2.0f);

	x_distance_ball_alexis_head = fabs(g_position_ball.x - g_position_alexis.x) -
		((INIT_SCALE_BALL.x + ALEXIS_HEAD_WIDTH) / 2.0f);
	y_distance_ball_alexis_head = fabs(g_position_ball.y - g_position_alexis_head_y) -
		((INIT_SCALE_BALL.y + ALEXIS_HEAD_HEIGHT) / 2.0f);

	x_distance_ball_messi = fabs(g_position_ball.x - g_position_messi.x) -
								((INIT_SCALE_BALL.x + INIT_SCALE_MESSI.x) / 2.0f);
	y_distance_ball_messi = fabs(g_position_ball.y - g_position_messi.y) -
								((INIT_SCALE_BALL.y + INIT_SCALE_MESSI.y) / 2.0f);
}


void update()
{
	float ticks = (float)SDL_GetTicks() / SECOND_IN_MILLISECONDS;
	float delta_time = ticks - g_previous_ticks;
	g_previous_ticks = ticks;

	g_line_matrix = glm::mat4(1.0f);
	g_line_matrix = glm::scale(g_line_matrix, INIT_SCALE_LINE);

	g_circle_matrix = glm::mat4(1.0f);
	g_circle_matrix = glm::scale(g_circle_matrix, INIT_SCALE_CIRCLE);

	g_dot_matrix = glm::mat4(1.0f);
	g_dot_matrix = glm::scale(g_dot_matrix, INIT_SCALE_DOT);

	if (g_game_status == ONGOING)
	{
		g_position_alexis += g_movement_alexis * g_alexis_speed * delta_time;
	}

	if (g_position_alexis.y >= 2.52f)
	{
		g_position_alexis.y -= 0.05f;
	}
	if (g_position_alexis.y <= -2.85f)
	{
		g_position_alexis.y += 0.05f;
	}

	g_position_alexis_head_y = g_position_alexis.y + 1.0f;
	g_position_alexis_torso_y = g_position_alexis.y - 0.75f;

	g_alexis_matrix = glm::mat4(1.0f);
	g_alexis_matrix = glm::translate(g_alexis_matrix, g_position_alexis);
	g_alexis_matrix = glm::scale(g_alexis_matrix, INIT_SCALE_ALEXIS);

	if (g_gamemode == ONE_PLAYER)
	{
		g_movement_messi.y = (g_messi_dir == UP) ? 1.0f : -1.0f;
	}

	if (g_game_status == ONGOING)
	{
		g_position_messi += g_movement_messi * g_messi_speed * delta_time;
	}

	if (g_position_messi.y >= 2.65f)
	{
		g_position_messi.y -= 0.05f;
		g_messi_dir = DOWN;
	}
	else if (g_position_messi.y <= -2.65f)
	{
		g_position_messi.y += 0.05f;
		g_messi_dir = UP;
	}

	g_messi_matrix = glm::mat4(1.0f);
	g_messi_matrix = glm::translate(g_messi_matrix, g_position_messi);
	g_messi_matrix = glm::scale(g_messi_matrix, INIT_SCALE_MESSI);

	if (ticks >= 1.75f && g_game_status == ONGOING)
	{
		if (glm::length(g_movement_ball) > 1.0f)
		{
			g_movement_ball = glm::normalize(g_movement_ball);
		}
		g_position_ball += g_movement_ball * g_ball_speed * delta_time;
	}

	if (g_position_ball.y >= 3.3 || g_position_ball.y <= -3.3f)
	{
		g_curr_border_hit_time = ticks;
		if ((g_curr_border_hit_time - g_prev_border_hit_time) > 0.1)
		{
			g_movement_ball.y *= -1.0f;
		}
		g_prev_border_hit_time = g_curr_border_hit_time;
	}

	calculate_ball_collision_distances();

	if ((x_distance_ball_alexis_torso < -0.2f && y_distance_ball_alexis_torso < -1.0f) || 
		(x_distance_ball_alexis_head < -0.15f && y_distance_ball_alexis_head < -0.45f) ||
		(x_distance_ball_messi < -0.5f && y_distance_ball_messi < -0.5f))
	{
		g_curr_collision_time = ticks;
		if (((g_curr_collision_time - g_prev_collision_time) > 0.1) && (g_position_ball.x > -3.5f))
		{
			g_movement_ball.x *= -1.0f;
			g_ball_speed *= 1.08f;

			if (g_gamemode == ONE_PLAYER)
			{
				g_messi_speed *= 1.1f;
			}
			else
			{
				g_messi_speed = 4.0f;
			}
		}
		else if (((g_curr_collision_time - g_prev_collision_time) > 0.1) && (g_position_ball.x <= -3.5f))
		{
			g_movement_ball.y *= -1.0f;
		}
		g_prev_collision_time = g_curr_collision_time;
	}

	if (g_position_ball.x <= -5.4f || g_position_ball.x >= 5.4f)
	{
		g_game_status = OVER;
	}

	g_ball_matrix = glm::mat4(1.0f);
	g_ball_matrix = glm::translate(g_ball_matrix, g_position_ball);
	g_ball_matrix = glm::scale(g_ball_matrix, INIT_SCALE_BALL);
}


void draw_object(glm::mat4& object_g_model_matrix, GLuint& object_texture_id)
{
	g_shader_program.set_model_matrix(object_g_model_matrix);
	glBindTexture(GL_TEXTURE_2D, object_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	float vertices[] =
	{
		-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
	};

	float texture_coordinates[] =
	{
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	};

	glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, 
					      vertices);
	glEnableVertexAttribArray(g_shader_program.get_position_attribute());

	glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, 
						  texture_coordinates);
	glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

	draw_object(g_line_matrix, g_line_texture_id);
	draw_object(g_circle_matrix, g_circle_texture_id);
	draw_object(g_dot_matrix, g_dot_texture_id);
	draw_object(g_alexis_matrix, g_alexis_texture_id);
	draw_object(g_messi_matrix, g_messi_texture_id);
	draw_object(g_ball_matrix, g_ball_texture_id);

	glDisableVertexAttribArray(g_shader_program.get_position_attribute());
	glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

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