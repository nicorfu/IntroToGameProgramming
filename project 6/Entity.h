#include <GL/glew.h>

#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include <SDL_mixer.h>


enum EntityType { PLAYER, ENEMY, COIN, PORTAL };
enum AIType { WALKER, GUARDIAN };
enum AIState { WALKING, IDLING, DYING };
enum AIWalkingOrientation { HORIZONTAL, VERTICAL };

enum AnimationAction { IDLE=0, WALK=4, DIE=8 };
enum AnimationDirection { UP, DOWN, LEFT, RIGHT };

class Entity
{
private:
	bool m_is_active = true;

	EntityType m_entity_type;

	AIType m_ai_type;
	AIState m_ai_state;

	AIWalkingOrientation m_ai_walking_orientation;
	float m_ai_walking_range[2];

	GLuint m_texture_id;

	glm::mat4 m_model_matrix;

	glm::vec3 m_scale;
	glm::vec3 m_position;
	glm::vec3 m_movement;
	glm::vec3 m_velocity;

	float m_width = 1.0f;
	float m_height = 1.0f;

	float m_speed;

	int m_animation_cols;
	int m_animation_rows;
	int m_animation_frames;
	int m_animation_index;

	AnimationDirection m_animation_direction;
	int* m_animation_indices = nullptr;

	float m_animation_time = 0.0f;

	int m_animation[12][4];

	bool m_attacking = false;
	float m_last_attack_time = 0.0f;

	bool m_enemy_hit = false;

	bool m_coin_grabbed = false;

	bool m_died = false;

	bool m_dying = false;

	bool m_portal_touched = false;

	bool m_player_won = false;

	bool m_collided_top = false;
	bool m_collided_bottom = false;
	bool m_collided_left = false;
	bool m_collided_right = false;


public:
	int SECONDS_PER_FRAME = 8;

	Entity();

	Entity(EntityType entity_type, GLuint texture_id, glm::vec3 scale, glm::vec3 position,
		float width, float height, float speed, int animation_cols, int animation_rows,
		int animation_frames, int animation_index, float animation_time, int animation[12][4]);

	Entity(EntityType entity_type, GLuint texture_id, glm::vec3 scale, float width, float height);

	~Entity();

	void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);

	void draw_text(ShaderProgram* shader_program, std::string text, float font_size, float spacing, glm::vec3 position,
		int fontbank_size);

	int get_random_sfx_index(int sfx_count);

	bool const check_collision(Entity* other) const;
	void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
	void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);

	void const check_collision_x(Map* map);
	void const check_collision_y(Map* map);

	void update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map,
		float curr_ticks);
	void render(ShaderProgram* program);

	void ai_activate(Entity* player, float curr_ticks);
	void ai_walk(Entity* player, float curr_ticks);

	void normalize_movement()
	{
		m_movement = glm::normalize(m_movement);
	}

	void idle()
	{
		m_movement = glm::vec3(0.0f, 0.0f, 0.0f);

		m_animation_indices = m_animation[IDLE + m_animation_direction];
	}

	void walk()
	{
		switch (m_animation_direction)
		{
			case UP:
				m_movement.y = 1.0f;
				break;

			case DOWN:
				m_movement.y = -1.0f;
				break;

			case LEFT:
				m_movement.x = -1.0f;
				break;

			case RIGHT:
				m_movement.x = 1.0f;
				break;

			default:
				break;
		}

		m_animation_indices = m_animation[WALK + m_animation_direction];
	}

	void attack(Entity* hittable_entities, int hittable_entity_count)
	{
		idle();

		for (int i = 0; i < hittable_entity_count; i++)
		{
			if (glm::distance(m_position, hittable_entities[i].m_position) < 0.75f && hittable_entities[i].m_is_active)
			{
				m_enemy_hit = true;
				hittable_entities[i].m_ai_state = DYING;
			}
		}
	}

	void die()
	{
		m_animation_indices = m_animation[DIE + m_animation_direction];
		m_animation_index = 0;

		m_movement = glm::vec3(0.0f);

		m_dying = true;
	}

	void activate()
	{
		m_is_active = true;
	};

	void deactivate()
	{
		m_is_active = false;
	};

	bool const get_is_active() const
	{
		return m_is_active;
	}

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

	bool const get_portal_touched() const
	{
		return m_portal_touched;
	}

	bool const get_player_won() const
	{
		return m_player_won;
	}

	float const get_last_attack_time() const
	{
		return m_last_attack_time;
	}

	bool const get_attacking() const
	{
		return m_attacking;
	}

	bool const get_dying() const
	{
		return m_dying;
	}

	bool const get_enemy_hit() const
	{
		return m_enemy_hit;
	}

	bool const get_coin_grabbed() const
	{
		return m_coin_grabbed;
	}

	bool const get_died() const
	{
		return m_died;
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

	void const set_attacking(bool new_attacking)
	{
		m_attacking = new_attacking;
	}

	void const set_ai_walking_orientation(AIWalkingOrientation new_ai_walking_orientation)
	{
		m_ai_walking_orientation = new_ai_walking_orientation;
	}

	void const set_ai_walking_range(float new_ai_walking_range[2])
	{
		m_ai_walking_range[0] = new_ai_walking_range[0];
		m_ai_walking_range[1] = new_ai_walking_range[1];
	}

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

	void const set_enemy_hit(float new_enemy_hit)
	{
		m_enemy_hit = new_enemy_hit;
	}

	void const set_coin_grabbed(float new_coin_grabbed)
	{
		m_coin_grabbed = new_coin_grabbed;
	}

	void const set_died(float new_died)
	{
		m_died = new_died;
	}

	void const set_portal_touched(float new_portal_touched)
	{
		m_portal_touched = new_portal_touched;
	}

	void const set_player_won(float new_player_won)
	{
		m_player_won = new_player_won;
	}

	void const set_last_attack_time(float new_attack_time)
	{
		m_last_attack_time = new_attack_time;
	}

	void set_animation_direction(AnimationDirection new_animation_direction)
	{
		m_animation_direction = new_animation_direction;
	}

	void set_animation(int new_animation[12][4])
	{
		for (int i = 0; i < 12; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m_animation[i][j] = new_animation[i][j];
			}
		}
	}
};

#endif