/**
* Author: Nico Flores
* Assignment: Simple 2D Scene
* Date due: 2023-06-11, 11:59pm
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

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 720;

constexpr float BG_RED = 0.451f;
constexpr float BG_GREEN = 0.3098f;
constexpr float BG_BLUE = 0.58824f;
constexpr float BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0;
constexpr int VIEWPORT_Y = 0;
constexpr int VIEWPORT_WIDTH = WINDOW_WIDTH;
constexpr int VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
constexpr char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

constexpr char SPIDERMAN_SPRITE_FILEPATH[] = "spider-man.png";
constexpr char SPIDERWEB_SPRITE_FILEPATH[] = "spider web.png";

constexpr float SPIDERMAN_MULTIPLIER = 1.25f;
constexpr float SPIDERWEB_MULTIPLIER = 0.005f;

constexpr glm::vec3 INIT_SCALE_SPIDERMAN = glm::vec3(2.500f, 3.333f, 0.000f) 
										             * SPIDERMAN_MULTIPLIER;
constexpr glm::vec3 INIT_SCALE_SPIDERWEB = glm::vec3(1.500f, 1.500f, 0.000f)
													 * SPIDERWEB_MULTIPLIER;
constexpr glm::vec3 INIT_POS_SPIDERMAN = glm::vec3(3.35f, 1.75f, 0.0f);
constexpr glm::vec3 INIT_POS_SPIDERWEB = glm::vec3(-0.16f, -0.13f, 0.0f); // true coords: 2.8, 1.185

constexpr float SECOND_IN_MILLISECONDS = 1000.0f;
float g_previous_ticks = 0.0f;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;

glm::mat4 g_view_matrix;
glm::mat4 g_spiderman_matrix;
glm::mat4 g_spiderweb_matrix;
glm::mat4 g_projection_matrix;
glm::mat4 g_translation_matrix;

glm::vec3 g_position_spiderman = INIT_POS_SPIDERMAN;
glm::vec3 g_movement_spiderman = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_rotation_spiderman = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_position_spiderweb = INIT_POS_SPIDERWEB;
//glm::vec3 g_position_spiderweb_actual = INIT_POS_SPIDERMAN + INIT_POS_SPIDERWEB;
glm::vec3 g_movement_spiderweb = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_rotation_spiderweb = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_scale_spiderweb = INIT_SCALE_SPIDERWEB;

GLuint g_spiderman_texture_id;
GLuint g_spiderweb_texture_id;

constexpr float g_spiderman_speed = 2.35f;
constexpr float g_spiderweb_speed = 1.0f;

constexpr float g_spiderweb_rot_increment = 14.0f;
constexpr float g_spiderweb_scale_increment = 0.5f;

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

	g_display_window = SDL_CreateWindow("Hi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		                                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
	SDL_GL_MakeCurrent(g_display_window, context);

	if (g_display_window == nullptr)
	{
		std::cerr << "Error: SDL window could not be created.\n";
		shutdown();
		exit(1);
	}

	glewInit();

	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

	g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

	g_spiderman_matrix = glm::mat4(1.0f);
	g_spiderweb_matrix = glm::mat4(1.0f);
	g_view_matrix = glm::mat4(1.0f);
	g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	g_shader_program.set_projection_matrix(g_projection_matrix);
	g_shader_program.set_view_matrix(g_view_matrix);

	//glUseProgram(g_shader_program.get_program_id());

	glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

	g_spiderman_texture_id = load_texture(SPIDERMAN_SPRITE_FILEPATH);
	g_spiderweb_texture_id = load_texture(SPIDERWEB_SPRITE_FILEPATH);

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

	if (ticks > 1.2f && g_position_spiderman.y >= -1.75f)
	{
		g_movement_spiderman.x = -1.9f;
		g_movement_spiderman.y = -1.0f;
	}
	else
	{
		g_movement_spiderman = glm::vec3(0.0f);
	}

	if (g_position_spiderman.x <= 2.8f)
	{
		g_rotation_spiderweb.z += -1 * g_spiderweb_rot_increment * delta_time;

		g_scale_spiderweb += g_spiderweb_scale_increment * delta_time;

		g_movement_spiderweb.x = -1.0f;
		g_movement_spiderweb.y = 1.0f;
	}
	else
	{
		g_movement_spiderweb = glm::vec3(0.0f);
	}

	g_position_spiderman += g_movement_spiderman * g_spiderman_speed * delta_time;
	g_position_spiderweb += g_movement_spiderweb * g_spiderweb_speed * delta_time;
	//g_position_spiderweb_actual = g_position_spiderman + g_position_spiderweb;

	g_spiderman_matrix = glm::mat4(1.0f);
	g_spiderweb_matrix = glm::mat4(1.0f);

	g_spiderman_matrix = glm::translate(g_spiderman_matrix, g_position_spiderman);
	g_spiderman_matrix = glm::scale(g_spiderman_matrix, INIT_SCALE_SPIDERMAN);

	g_spiderweb_matrix = glm::translate(g_spiderman_matrix, g_position_spiderweb);
	g_spiderweb_matrix = glm::rotate(g_spiderweb_matrix, g_rotation_spiderweb.z,
									 glm::vec3(0.0f, 0.0f, 1.0f));
	g_spiderweb_matrix = glm::scale(g_spiderweb_matrix, g_scale_spiderweb);
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

	glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false,
			              0, texture_coordinates);
	glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

	draw_object(g_spiderman_matrix, g_spiderman_texture_id);
	draw_object(g_spiderweb_matrix, g_spiderweb_texture_id);

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