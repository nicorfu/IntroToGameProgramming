#include "Map.h"

Map::Map(int width, int height, unsigned int* level_data, GLuint texture_id, float tile_size, int tile_count_x,
	int tile_count_y)
{
	m_width = width;
	m_height = height;

	m_level_data = level_data;
	m_texture_id = texture_id;

	m_tile_size = tile_size;
	m_tile_count_x = tile_count_x;
	m_tile_count_y = tile_count_y;

	build();
}

void Map::build();

void Map::render(ShaderProgram* program);

bool Map::is_solid(glm::vec3 position, float* penetration_x, float* penetration_y);