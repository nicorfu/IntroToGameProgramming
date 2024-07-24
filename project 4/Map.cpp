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

void Map::build()
{
	for (int y_coord = 0; y_coord < m_height; y_coord++)
	{
		for (int x_coord = 0; x_coord < m_width; x_coord++)
		{
			int tile = m_level_data[y_coord * m_width + x_coord];

			if (tile == 0)
			{
				continue;
			}

			float u_coord = (float)(tile % m_tile_count_x) / (float)m_tile_count_x;
			float v_coord = (float)(tile / m_tile_count_x) / (float)m_tile_count_y;

			float tile_width = 1.0f / (float)m_tile_count_x;
			float tile_height = 1.0f / (float)m_tile_count_y;

			float x_offset = -(m_tile_size / 2);
			float y_offset = (m_tile_size / 2);

			m_vertices.insert(m_vertices.end(), {
				x_offset + (m_tile_size * x_coord),  y_offset + -m_tile_size * y_coord,
				x_offset + (m_tile_size * x_coord),  y_offset + (-m_tile_size * y_coord) - m_tile_size,
				x_offset + (m_tile_size * x_coord) + m_tile_size, y_offset + (-m_tile_size * y_coord) - m_tile_size,
				x_offset + (m_tile_size * x_coord), y_offset + -m_tile_size * y_coord,
				x_offset + (m_tile_size * x_coord) + m_tile_size, y_offset + (-m_tile_size * y_coord) - m_tile_size,
				x_offset + (m_tile_size * x_coord) + m_tile_size, y_offset + -m_tile_size * y_coord
				});

			m_texture_coordinates.insert(m_texture_coordinates.end(), {
				u_coord, v_coord,
				u_coord, v_coord + (tile_height),
				u_coord + tile_width, v_coord + (tile_height),
				u_coord, v_coord,
				u_coord + tile_width, v_coord + (tile_height),
				u_coord + tile_width, v_coord
				});
		}
	}

	m_left_bound = 0 - (m_tile_size / 2);
	m_right_bound = (m_tile_size * m_width) - (m_tile_size / 2);
	m_top_bound = 0 + (m_tile_size / 2);
	m_bottom_bound = -(m_tile_size * m_height) + (m_tile_size / 2);
}

void Map::render(ShaderProgram* program);

bool Map::is_solid(glm::vec3 position, float* penetration_x, float* penetration_y);