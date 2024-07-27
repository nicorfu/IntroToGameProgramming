#include <GL/glew.h>

#pragma once

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#include <vector>
#include <math.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"


class Map
{
private:
	int m_width;
	int m_height;

	unsigned int* m_level_data;
	GLuint m_texture_id;

	glm::mat4 model_matrix;

	float m_tile_size;
	int m_tile_count_x;
	int m_tile_count_y;

	std::vector<float> m_vertices;
	std::vector<float> m_texture_coordinates;

	float m_top_bound;
	float m_bottom_bound;
	float m_left_bound;
	float m_right_bound;

public:
	Map(int width, int height, unsigned int* level_data, GLuint texture_id, float tile_size, int tile_count_x,
		int tile_count_y);

	void build();
	void render(ShaderProgram *program);

	void center();

	bool is_solid(glm::vec3 position, float *penetration_x, float *penetration_y);

	int const get_width() const
	{
		return m_width;
	}

	int const get_height() const
	{
		return m_height;
	}

	unsigned int* const get_level_data() const
	{
		return m_level_data;
	}

	GLuint const get_texture_id() const
	{
		return m_texture_id;
	}

	float const get_tile_size()    const 
	{ 
		return m_tile_size; 
	}

	int   const get_tile_count_x() const 
	{ 
		return m_tile_count_x; 
	}

	int   const get_tile_count_y() const 
	{ 
		return m_tile_count_y; 
	}

	std::vector<float> const get_vertices() const 
	{ 
		return m_vertices; 
	}

	std::vector<float> const get_texture_coordinates() const 
	{ 
		return m_texture_coordinates; 
	}

	float const get_top_bound()    const
	{
		return m_top_bound;
	}

	float const get_bottom_bound() const
	{
		return m_bottom_bound;
	}

	float const get_left_bound()   const 
	{ 
		return m_left_bound; 
	}

	float const get_right_bound()  const 
	{ 
		return m_right_bound; 
	}
};