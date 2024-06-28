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
constexpr char ALEXIS_SPRITE_FILEPATH[] = "alexis.png";
constexpr char MESSI_SPRITE_FILEPATH[] = "messi.png";
constexpr char BALL_SPRITE_FILEPATH[] = "ball.png";

GLuint g_line_texture_id;
GLuint g_circle_texture_id;
GLuint g_alexis_texture_id;
GLuint g_messi_texture_id;
GLuint g_ball_texture_id;

constexpr float LINE_SCALE_MULTIPLIER = 20.0f;
constexpr float CIRCLE_SCALE_MULTIPLIER = 20.0f;
constexpr float ALEXIS_SCALE_MULTIPLIER = 0.25f;
constexpr float MESSI_SCALE_MULTIPLIER = 0.24f;
constexpr float BALL_SCALE_MULTIPLIER = 0.095f;

constexpr glm::vec3 INIT_SCALE_LINE = glm::vec3(10.0f, 6.6666f, 0.000f) * LINE_SCALE_MULTIPLIER;
constexpr glm::vec3 INIT_SCALE_CIRCLE = glm::vec3(10.0f, 10.0f, 0.000f) * CIRCLE_SCALE_MULTIPLIER;
constexpr glm::vec3 INIT_SCALE_ALEXIS = glm::vec3(8.7f, 13.9752f, 0.000f) * ALEXIS_SCALE_MULTIPLIER;
constexpr glm::vec3 INIT_SCALE_MESSI = glm::vec3(8.7f, 10.0f, 0.000f) * MESSI_SCALE_MULTIPLIER;
constexpr glm::vec3 INIT_SCALE_BALL = glm::vec3(9.0f, 10.0f, 0.000f) * BALL_SCALE_MULTIPLIER;

constexpr glm::vec3 INIT_POS_LINE = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_CIRCLE = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_ALEXIS = glm::vec3(-4.1f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_MESSI = glm::vec3(4.0f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_BALL = glm::vec3(-0.5f, 0.0f, 0.0f);

glm::mat4 g_view_matrix;
glm::mat4 g_line_matrix;
glm::mat4 g_circle_matrix;
glm::mat4 g_alexis_matrix;
glm::mat4 g_messi_matrix;
glm::mat4 g_ball_matrix;
glm::mat4 g_projection_matrix;
glm::mat4 g_translation_matrix;

constexpr glm::vec3 G_POSITION_LINE = INIT_POS_LINE;

constexpr glm::vec3 G_POSITION_CIRCLE = INIT_POS_CIRCLE;

glm::vec3 g_position_alexis = INIT_POS_ALEXIS;
glm::vec3 g_movement_alexis = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_position_messi = INIT_POS_MESSI;
glm::vec3 g_movement_messi = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_position_ball = INIT_POS_BALL;
glm::vec3 g_movement_ball = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_rotation_ball = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_scale_ball = INIT_SCALE_BALL;

constexpr float g_alexis_speed = 1.0f;

constexpr float g_messi_speed = 1.0f;

constexpr float g_ball_speed = 1.0f;
constexpr float g_ball_rot_increment = 1.0f;
constexpr float g_ball_scale_increment = 1.0f;

constexpr float SECOND_IN_MILLISECONDS = 1000.0f;
float g_previous_ticks = 0.0f;

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
	g_alexis_texture_id = load_texture(ALEXIS_SPRITE_FILEPATH);
	g_messi_texture_id = load_texture(MESSI_SPRITE_FILEPATH);
	g_ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);

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
	float ticks = (float)SDL_GetTicks() / SECOND_IN_MILLISECONDS;
	float delta_time = ticks - g_previous_ticks;
	g_previous_ticks = ticks;

	g_line_matrix = glm::mat4(1.0f);
	g_line_matrix = glm::scale(g_line_matrix, INIT_SCALE_LINE);

	g_circle_matrix = glm::mat4(1.0f);
	g_circle_matrix = glm::scale(g_circle_matrix, INIT_SCALE_CIRCLE);

	g_position_alexis += g_movement_alexis * g_alexis_speed * delta_time;
	g_alexis_matrix = glm::mat4(1.0f);
	g_alexis_matrix = glm::translate(g_alexis_matrix, g_position_alexis);
	g_alexis_matrix = glm::scale(g_alexis_matrix, INIT_SCALE_ALEXIS);

	g_position_messi += g_movement_messi * g_messi_speed * delta_time;
	g_messi_matrix = glm::mat4(1.0f);
	g_messi_matrix = glm::translate(g_messi_matrix, g_position_messi);
	g_messi_matrix = glm::scale(g_messi_matrix, INIT_SCALE_MESSI);
	
	g_position_ball += g_movement_ball * g_ball_speed * delta_time;
	g_ball_matrix = glm::mat4(1.0f);
	g_ball_matrix = glm::translate(g_ball_matrix, g_position_ball);
	g_ball_matrix = glm::rotate(g_ball_matrix, g_rotation_ball.z, glm::vec3(0.0f, 0.0f, 1.0f));
	g_ball_matrix = glm::scale(g_ball_matrix, INIT_SCALE_BALL);
	//g_ball_matrix = glm::scale(g_ball_matrix, g_scale_ball);
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