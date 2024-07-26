#include <GL/glew.h>

#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"


enum EntityType { PLATFORM, PLAYER, ENEMY };
enum AIType { WALKER, GUARD};
enum AIState { WALKING, IDLE, ATTACKING};

enum AnimationAction { LEFT, RIGHT };


class Entity
{
private:
	bool m_is_active = true;

	EntityType m_entity_type;

	AIType m_ai_type;
	AIState m_ai_state;

	GLuint m_texture_id;

	glm::mat4 m_model_matrix;

	glm::vec3 m_scale;
	glm::vec3 m_position;
	glm::vec3 m_movement;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;

	float m_width = 1;
	float m_height = 1;

	float m_speed;
	float m_jump_power;

	bool m_is_jumping;

	int m_animation_cols;
	int m_animation_rows;
	int m_animation_frames;
	int m_animation_index;

	std::vector<int> m_animation_indices;

	float m_animation_time = 0.0f;

	std::vector<std::vector<int>> m_animation;

	bool m_collided_top = false;
	bool m_collided_bottom = false;
	bool m_collided_left = false;
	bool m_collided_right = false;

public:
	static constexpr int SECONDS_PER_FRAME = 4;

	Entity();

	Entity(EntityType entity_type, GLuint texture_id, glm::vec3 scale, glm::vec3 acceleration, float width, float height,
		float speed, float jump_power, int animation_cols, int animation_rows, int animation_frames, int animation_index,
		float animation_time, std::vector<std::vector<int>> animation);

	Entity(EntityType entity_type, GLuint texture_id, float width, float height, float speed);

	Entity(EntityType entity_type, AIType ai_type, AIState ai_state, GLuint texture_id, float width, float height, 
		   float speed);

	~Entity();

	void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);

	bool const check_collision(Entity* other) const;
	void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
	void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);

	void const check_collision_x(Map* map);
	void const check_collision_y(Map* map);

	void update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map *map);
	void render(ShaderProgram* program);

	void ai_activate(Entity *player);
	void ai_walk();
	void ai_guard(Entity* player);	

	void normalize_movement()
	{
		m_movement = glm::normalize(m_movement);
	}

	void face_left()
	{
		m_animation_indices = m_animation[LEFT];
	}

	void face_right()
	{
		m_animation_indices = m_animation[RIGHT];
	}

	void move_left()
	{
		m_movement.x = -1.0f;
		face_left();
	}

	void move_right()
	{
		m_movement.x = 1.0f;
		face_right();
	}

	void const jump()
	{
		m_is_jumping = true;
	}

	void activate()
	{
		m_is_active = true;
	};

	void deactivate()
	{
		m_is_active = false;
	};

	EntityType const get_entity_type() const
	{
		return m_entity_type;
	};

	AIType const get_ai_type() const
	{
		return m_ai_type;
	};

	AIState const get_ai_state() const
	{
		return m_ai_state;
	};

	GLuint const get_texture_id() const
	{
		return m_texture_id;
	}

	glm::vec3 const get_scale() const
	{
		return m_scale;
	}

	glm::vec3 const get_position() const
	{
		return m_position;
	}

	glm::vec3 const get_movement() const
	{
		return m_movement;
	}

	glm::vec3 const get_velocity() const
	{
		return m_velocity;
	}

	glm::vec3 const get_acceleration() const
	{
		return m_acceleration;
	}

	int const get_width() const
	{
		return int(m_width);
	}

	int const get_height() const
	{
		return int(m_height);
	}

	float const get_speed() const
	{
		return m_speed;
	}

	bool const get_collided_top() const
	{
		return m_collided_top;
	}

	bool const get_collided_bottom() const
	{
		return m_collided_bottom;
	}

	bool const get_collided_left() const
	{
		return m_collided_left;
	}

	bool const get_collided_right() const
	{
		return m_collided_right;
	}

	void const set_entity_type(EntityType new_entity_type)
	{
		m_entity_type = new_entity_type;
	};

	void const set_ai_type(AIType new_ai_type)
	{
		m_ai_type = new_ai_type;
	};

	void const set_ai_state(AIState new_ai_state)
	{
		m_ai_state = new_ai_state;
	};

	void const set_texture_id(GLuint new_texture_id)
	{
		m_texture_id = new_texture_id;
	}

	void const set_scale(glm::vec3 new_scale)
	{
		m_scale = new_scale;
	}

	void const set_position(glm::vec3 new_position)
	{
		m_position = new_position;
	}

	void const set_movement(glm::vec3 new_movement)
	{
		m_movement = new_movement;
	}

	void const set_velocity(glm::vec3 new_velocity)
	{
		m_velocity = new_velocity;
	}

	void const set_acceleration(glm::vec3 new_acceleration)
	{
		m_acceleration = new_acceleration;
	}

	void const set_width(float new_width)
	{
		m_width = new_width;
	}

	void const set_height(float new_height)
	{
		m_height = new_height;
	}

	void const set_speed(float new_speed)
	{
		m_speed = new_speed;
	}

	void const set_jump_power(float new_jump_power)
	{
		m_jump_power = new_jump_power;
	}

	void const set_animation_cols(int new_animation_cols) 
	{ 
		m_animation_cols = new_animation_cols; 
	}

	void const set_animation_rows(int new_animation_rows) 
	{ 
		m_animation_rows = new_animation_rows; 
	}

	void const set_animation_frames(int new_animation_frames) 
	{ 
		m_animation_frames = new_animation_frames; 
	}

	void const set_animation_index(int new_animation_index) 
	{ 
		m_animation_index = new_animation_index; 
	}

	void const set_animation_time(float new_animation_time) 
	{ 
		m_animation_time = new_animation_time; 
	}

	void set_animation(std::vector<std::vector<int>> new_animation)
	{
		for (int i = 0; i < m_animation_rows; i++)
		{
			for (int j = 0; j < m_animation_cols; j++)
			{
				m_animation[i][j] = new_animation[i][j];
			}
		}
	}
};

#endif