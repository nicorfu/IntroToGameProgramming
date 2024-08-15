/**
* Author: Nico Flores
* Assignment: Hamza's Revenge
* Date due: 2024-08-15, 1:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include <GL/glew.h>

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include <SDL_mixer.h>


Entity::Entity()
{
	m_texture_id = 0;
	m_model_matrix = glm::mat4(1.0f);
	m_scale = glm::vec3(1.0f, 1.0f, 0.0f);
	m_position = glm::vec3(0.0f);
	m_movement = glm::vec3(0.0f);
	m_velocity = glm::vec3(0.0f);
	m_animation_cols = 0;
	m_animation_rows = 0;
	m_animation_frames = 0;
	m_animation_index = 0;
}


Entity::Entity(EntityType entity_type, GLuint texture_id, glm::vec3 scale, glm::vec3 position,
	float width, float height, float speed, int animation_cols, int animation_rows,
	int animation_frames, int animation_index, float animation_time, int animation[12][4])//, Mix_Chunk* land_sfx,
	//Mix_Chunk* walk_sfx[2], Mix_Chunk* hit_sfx[3], Mix_Chunk* grunt_sfx[4], Mix_Chunk* pain_sfx[4])
{
	m_entity_type = entity_type;
	m_texture_id = texture_id;
	m_model_matrix = glm::mat4(1.0f);
	m_scale = scale;
	m_position = position;
	m_movement = glm::vec3(0.0f, 0.0f, 0.0f);
	m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	m_width = width;
	m_height = height;
	m_speed = speed;
	m_animation_cols = animation_cols;
	m_animation_rows = animation_rows;
	m_animation_frames = animation_frames;
	m_animation_index = animation_index;
	m_animation_time = animation_time;

	set_animation(animation);
	m_animation_direction = DOWN;
	m_animation_indices = m_animation[IDLE + m_animation_direction];

	if (entity_type == ENEMY)
	{
		SECONDS_PER_FRAME = 5;
	}

	//m_land_sfx = land_sfx;
	/*
	set_walk_sfx(walk_sfx);
	set_hit_sfx(hit_sfx);
	set_grunt_sfx(grunt_sfx);
	set_pain_sfx(pain_sfx);
	*/
}


Entity::Entity(EntityType entity_type, GLuint texture_id, glm::vec3 scale, float width, float height)
{
	m_entity_type = entity_type;
	m_texture_id = texture_id;
	m_model_matrix = glm::mat4(1.0f);
	m_scale = scale;
	m_position = glm::vec3(0.0f);
	m_movement = glm::vec3(0.0f);
	m_velocity = glm::vec3(0.0f);
	m_width = width;
	m_height = height;
}


Entity::~Entity()
{ }


void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
	float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
	float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

	float width = 1.0f / (float)m_animation_cols;
	float height = 1.0f / (float)m_animation_rows;

	float tex_coords[] =
	{
		u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width,
		v_coord, u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
	};

	float vertices[] =
	{
		-0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
		-0.5, -0.5, 0.5,  0.5, -0.5, 0.5
	};

	glBindTexture(GL_TEXTURE_2D, texture_id);

	glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->get_position_attribute());

	glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
	glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->get_position_attribute());
	glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


void Entity::draw_text(ShaderProgram* shader_program, std::string text, float font_size, float spacing, glm::vec3 position,
	int fontbank_size)
{
	float width = 1.0f / fontbank_size;
	float height = 1.0f / fontbank_size;

	std::vector<float> vertices;
	std::vector<float> texture_coordinates;

	for (int i = 0; i < text.size(); i++)
	{
		int spritesheet_index = (int)text[i];
		float offset = (font_size + spacing) * i;

		float u_coordinate = (float)(spritesheet_index % fontbank_size) / fontbank_size;
		float v_coordinate = (float)(spritesheet_index / fontbank_size) / fontbank_size;

		vertices.insert(vertices.end(),
			{
				offset + (-0.5f * font_size), 0.5f * font_size,
				offset + (-0.5f * font_size), -0.5f * font_size,
				offset + (0.5f * font_size), 0.5f * font_size,
				offset + (0.5f * font_size), -0.5f * font_size,
				offset + (0.5f * font_size), 0.5f * font_size,
				offset + (-0.5f * font_size), -0.5f * font_size,
			});

		texture_coordinates.insert(texture_coordinates.end(),
			{
				u_coordinate, v_coordinate,
				u_coordinate, v_coordinate + height,
				u_coordinate + width, v_coordinate,
				u_coordinate + width, v_coordinate + height,
				u_coordinate + width, v_coordinate,
				u_coordinate, v_coordinate + height,
			});
	}

	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, position);

	shader_program->set_model_matrix(model_matrix);
	glUseProgram(shader_program->get_program_id());

	glVertexAttribPointer(shader_program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
	glEnableVertexAttribArray(shader_program->get_position_attribute());

	glVertexAttribPointer(shader_program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
		texture_coordinates.data());
	glEnableVertexAttribArray(shader_program->get_tex_coordinate_attribute());

	glBindTexture(GL_TEXTURE_2D, m_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

	glDisableVertexAttribArray(shader_program->get_position_attribute());
	glDisableVertexAttribArray(shader_program->get_tex_coordinate_attribute());
}


int Entity::get_random_sfx_index(int sfx_count)
{
	srand(time(nullptr));

	return rand() % sfx_count;
}


bool const Entity::check_collision(Entity* other) const
{
	if (other == this)
	{
		return false;
	}

	if (this->get_dying() || other->get_dying())
	{
		return false;
	}

	if (!m_is_active || !other->m_is_active)
	{
		return false;
	}

	float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
	float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

	return x_distance < 0.0f && y_distance < 0.0f;
}


void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
	for (int i = 0; i < collidable_entity_count; i++)
	{
		Entity* collidable_entity = &collidable_entities[i];

		if (check_collision(collidable_entity))
		{
			float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
			float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));

			if (m_velocity.x > 0)
			{
				m_position.x -= x_overlap;
				m_velocity.x = 0;

				//m_collided_right = true;
			}
			else if (m_velocity.x < 0)
			{
				m_position.x += x_overlap;
				m_velocity.x = 0;

				//m_collided_left = true;
			}
		}
	}
}


void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
	for (int i = 0; i < collidable_entity_count; i++)
	{
		Entity* collidable_entity = &collidable_entities[i];

		if (check_collision(collidable_entity))
		{
			float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
			float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));

			if (m_velocity.y > 0)
			{
				m_position.y -= y_overlap;
				m_velocity.y = 0;

				//m_collided_top = true;
			}
			else if (m_velocity.y < 0)
			{
				m_position.y += y_overlap;
				m_velocity.y = 0;

				//m_collided_bottom = true;
			}
		}
	}
}


void const Entity::check_collision_x(Map* map)
{
	glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
	glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

	float penetration_x = 0;
	float penetration_y = 0;

	if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
	{
		m_position.x += penetration_x;
		m_velocity.x = 0;

		m_collided_left = true;
	}

	else if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
	{
		m_position.x -= penetration_x;
		m_velocity.x = 0;

		m_collided_right = true;
	}
}


void const Entity::check_collision_y(Map* map)
{
	glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
	glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
	glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

	glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
	glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
	glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

	float penetration_x = 0;
	float penetration_y = 0;

	if (m_velocity.y > 0)
	{
		if (map->is_solid(top, &penetration_x, &penetration_y))
		{
			m_position.y -= penetration_y;
			m_velocity.y = 0;

			m_collided_top = true;
		}
	}
	else if (m_velocity.y < 0)
	{
		if (map->is_solid(bottom, &penetration_x, &penetration_y))
		{
			m_position.y += penetration_y;
			m_velocity.y = 0;

			m_collided_bottom = true;
		}
	}
}


void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map,
	float curr_ticks)
{
	if (!m_is_active)
	{
		return;
	}

	m_collided_top = false;
	m_collided_bottom = false;
	m_collided_left = false;
	m_collided_right = false;

	if (m_entity_type == ENEMY)
	{
		ai_activate(player, curr_ticks);
	}

	if (m_animation_indices != nullptr)
	{
		m_animation_time += delta_time;

		float frames_per_sec = (float)1 / SECONDS_PER_FRAME;

		if (m_animation_time >= frames_per_sec)
		{
			m_animation_time = 0.0f;

			m_animation_index++;

			if (m_animation_index >= m_animation_frames)
			{
				if (m_dying)
				{
					deactivate();
				}
				m_animation_index = 0;
			}
		}
	}

	m_velocity.x = m_movement.x * m_speed;
	m_position.x += m_velocity.x * delta_time;

	if (map != nullptr)
	{
		check_collision_x(collidable_entities, collidable_entity_count);
		check_collision_x(map);
	}

	m_velocity.y = m_movement.y * m_speed;
	m_position.y += m_velocity.y * delta_time;

	if (map != nullptr)
	{
		check_collision_y(collidable_entities, collidable_entity_count);
		check_collision_y(map);
	}

	m_model_matrix = glm::mat4(1.0f);

	m_model_matrix = glm::translate(m_model_matrix, m_position);
	m_model_matrix = glm::scale(m_model_matrix, m_scale);
}


void Entity::render(ShaderProgram* program)
{
	if (!m_is_active)
	{
		return;
	}

	program->set_model_matrix(m_model_matrix);

	if (m_animation_indices != nullptr)
	{
		draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);

		return;
	}

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

	glBindTexture(GL_TEXTURE_2D, m_texture_id);

	glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->get_position_attribute());
	glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
	glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->get_position_attribute());
	glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


void Entity::ai_activate(Entity* player, float curr_ticks)
{
	ai_walk(player, curr_ticks);
}


void Entity::ai_walk(Entity* player, float curr_ticks)
{
	switch (m_ai_state)
	{
		case IDLING:
			idle();
			if (glm::distance(m_position, player->get_position()) < 4.0f)
			{
				m_ai_state = WALKING;
			}
			break;

		case WALKING:
			if (m_ai_walking_orientation == HORIZONTAL)
			{
				if (m_position.x <= m_ai_walking_range[0])
				{
					set_animation_direction(RIGHT);
				}
				else if (m_position.x >= m_ai_walking_range[1])
				{
					set_animation_direction(LEFT);
				}
			}
			else if (m_ai_walking_orientation == VERTICAL)
			{
				if (m_position.y >= m_ai_walking_range[0])
				{
					set_animation_direction(DOWN);
				}
				else if (m_position.y <= m_ai_walking_range[1])
				{
					set_animation_direction(UP);
				}
			}

			walk();

			if (glm::distance(m_position, player->get_position()) >= 4.0f)
			{
				m_ai_state = IDLING;
			}

			break;

		case DYING:
			if (!m_dying)
			{
				die();
			}
			break;
	}
}