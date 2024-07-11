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


Entity::Entity()
{
	m_position = glm::vec3(0.0f);
	m_velocity = glm::vec3(0.0f);
	m_acceleration = glm::vec3(0.0f);

	m_movement = glm::vec3(0.0f);
	m_speed = 0;
	m_model_matrix = glm::mat4(1.0f);
}


void Entity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count)
{
	if (!m_is_active)
	{
		return;
	}

	m_collided_top = false;
	m_collided_bottom = false;
	m_collided_left = false;
	m_collided_right = false;

	m_velocity.x = m_movement.x * m_speed;
	m_velocity += m_acceleration * delta_time;

	m_position.y += m_velocity.y * delta_time;
	check_collision_y(collidable_entities, collidable_entity_count);

	m_position.x += m_velocity.x * delta_time;
	check_collision_x(collidable_entities, collidable_entity_count);

	m_model_matrix = glm::mat4(1.0f);
	m_model_matrix = glm::translate(m_model_matrix, m_position);
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
				m_collided_top = true;
			}
			else if (m_velocity.y < 0)
			{
				m_position.y += y_overlap;
				m_velocity.y = 0;
				m_collided_bottom = true;
			}
		}
	}
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