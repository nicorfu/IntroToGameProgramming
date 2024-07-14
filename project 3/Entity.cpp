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

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include <vector>


constexpr int FONTBANK_SIZE = 16;


Entity::Entity()
{
	m_position = glm::vec3(0.0f);
	m_velocity = glm::vec3(0.0f);
	m_acceleration = glm::vec3(0.0f);

	m_movement = glm::vec3(0.0f);
	m_speed = 0;
	m_model_matrix = glm::mat4(1.0f);
}


void Entity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count, bool *game_ongoing, 
	bool *mission_failed, int target_platform)
{
	if (!m_is_active)
	{
		return;
	}

	m_collided_top = false;
	m_collided_bottom = false;
	m_collided_left = false;
	m_collided_right = false;

	m_velocity += m_acceleration * delta_time;

	m_position.y += m_velocity.y * delta_time;
	check_collision_y(collidable_entities, collidable_entity_count, game_ongoing, mission_failed, target_platform);

	if (m_position.y <= -3.75f || m_position.y >= 4.5f)
	{
		*game_ongoing = false;
		*mission_failed = true;
	}

	if (m_acceleration.x == 0 && m_velocity.x != 0)
	{
		float friction = (m_collided_bottom) ? 0.02f : 0.0085f;

		if (m_velocity.x > 0)
		{
			m_velocity -= friction;

			if (m_velocity.x < 0)
			{
				m_velocity.x = 0;
			}
		}
		else if (m_velocity.x < 0)
		{
			m_velocity += friction;

			if (m_velocity.x > 0)
			{
				m_velocity.x = 0;
			}
		}
	}

	m_position.x += m_velocity.x * delta_time;
	check_collision_x(collidable_entities, collidable_entity_count, game_ongoing, mission_failed);

	if (m_position.x <= -4.9f || m_position.x >= 4.9f)
	{
		*game_ongoing = false;
		*mission_failed = true;
	}

	m_model_matrix = glm::mat4(1.0f);
	m_model_matrix = glm::translate(m_model_matrix, m_position);
	m_model_matrix = glm::scale(m_model_matrix, m_scale);
	m_model_matrix = glm::rotate(m_model_matrix, m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}


void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count, bool *game_ongoing, 
	bool *mission_failed, int target_platform)
{
	for (int i = 0; i < collidable_entity_count; i++)
	{
		Entity* collidable_entity = &collidable_entities[i];

		if (check_collision(collidable_entity))
		{
			if (fabs(m_velocity.y) > 0.8f)
			{
				*game_ongoing = false;
				*mission_failed = true;
				return;
			}

			float y_distance = fabs(m_position.y - (collidable_entity->m_position.y - 0.25f));
			float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));

			if (m_velocity.y > 0)
			{
				m_position.y -= y_overlap;
				m_velocity.y = 0;
				m_collided_top = true;
			}
			else if (m_velocity.y < 0)
			{
				m_position.y += y_overlap;
				m_velocity.y = 0;
				m_collided_bottom = true;
				
				*game_ongoing = false;

				if (i != target_platform)
				{
					*mission_failed = true;
				}
			}
		}
	}
}


void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count, bool *game_ongoing, 
	bool *mission_failed)
{
	for (int i = 0; i < collidable_entity_count; i++)
	{
		Entity* collidable_entity = &collidable_entities[i];

		if (check_collision(collidable_entity))
		{
			if (fabs(m_velocity.x) > 0.9f)
			{
				*game_ongoing = false;
				*mission_failed = true;
				return;
			}

			float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
			float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
			if (!m_collided_bottom && !m_collided_top)
			{
				if (m_velocity.x > 0)
				{
					m_position.x -= x_overlap;
					m_velocity.x = 0;
					m_collided_right = true;
				}
				else if (m_velocity.x < 0)
				{
					m_position.x += x_overlap;
					m_velocity.x = 0;
					m_collided_left = true;
				}
			}
		}
	}
}


void Entity::render(ShaderProgram* program)
{
	if (!m_is_active) return;

	program->set_model_matrix(m_model_matrix);

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


bool const Entity::check_collision(Entity* other) const
{
	if (!m_is_active || !other->m_is_active)
	{
		return false;
	}

	float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
	float y_distance = fabs(m_position.y - (other->m_position.y - 0.25f)) - ((m_height + other->m_height) / 2.0f);

	return x_distance < 0.0f && y_distance < 0.0f;
}

void Entity::draw_text(ShaderProgram* shader_program, std::string text, float font_size, float spacing, glm::vec3 position)
{
	float width = 1.0f / FONTBANK_SIZE;
	float height = 1.0f / FONTBANK_SIZE;

	std::vector<float> vertices;
	std::vector<float> texture_coordinates;

	for (int i = 0; i < text.size(); i++)
	{
		int spritesheet_index = (int)text[i];
		float offset = (font_size + spacing) * i;

		float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
		float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

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