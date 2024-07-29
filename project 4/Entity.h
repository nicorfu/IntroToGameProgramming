#include <GL/glew.h>

#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include <SDL_mixer.h>

#define WALK_SFX_COUNT 2
#define HIT_SFX_COUNT 3
#define GRUNT_SFX_COUNT 4


enum EntityType { PLAYER, ENEMY };
enum AIType { WALKER, GUARDIAN, WAITER};
enum AIState { WALKING, IDLING, ATTACKING, DYING};

enum AnimationAction { IDLE, MOVE, DIE, ATTACK };

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

	float m_width = 1.0f;
	float m_height = 1.0f;

	float m_speed;
	float m_jump_power;

	bool m_is_jumping = false;

	int m_animation_cols;
	int m_animation_rows;
	int m_animation_frames;
	int m_animation_index;

	int* m_animation_indices = nullptr;

	float m_animation_time = 0.0f;

	int m_animation[4][8];

	bool m_facing_left = false;
	bool m_attacking = false;
	bool m_dying = false;

	float m_last_attack_time = 0.0f;
	float m_lethal_distance;

	bool m_collided_top = false;
	bool m_collided_bottom = false;
	bool m_collided_left = false;
	bool m_collided_right = false;

	Mix_Chunk* m_land_sfx;
	bool m_play_land = false;

	Mix_Chunk* m_walk_sfx[WALK_SFX_COUNT];
	Mix_Chunk* m_hit_sfx[HIT_SFX_COUNT];
	Mix_Chunk* m_grunt_sfx[GRUNT_SFX_COUNT];

public:
	static constexpr int SECONDS_PER_FRAME = 6;

	Entity();

	Entity(EntityType entity_type, GLuint texture_id, glm::vec3 scale, glm::vec3 position, glm::vec3 acceleration, 
		float width, float height, float speed, float jump_power, int animation_cols, int animation_rows, 
		int animation_frames, int animation_index, float animation_time, int animation[4][8], Mix_Chunk* land_sfx,
		Mix_Chunk* walk_sfx[2], Mix_Chunk* hit_sfx[3], Mix_Chunk* grunt_sfx[4]);

	Entity(EntityType entity_type, GLuint texture_id, float width, float height, float speed);

	~Entity();

	void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index, bool facing_left);

	void draw_text(ShaderProgram* shader_program, std::string text, float font_size, float spacing, glm::vec3 position, 
		int fontbank_size);

	int get_random_sfx_index(int sfx_count);

	bool const check_collision(Entity* other) const;
	void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
	void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);

	void const check_collision_x(Map* map);
	void const check_collision_y(Map* map);

	void update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map *map, 
		float curr_ticks);
	void render(ShaderProgram* program);

	void ai_activate(Entity *player, float curr_ticks);
	void ai_walk();
	void ai_guard(Entity* player, float curr_ticks);	
	void ai_wait(Entity* player, float curr_ticks);

	void normalize_movement()
	{
		m_movement = glm::normalize(m_movement);
	}

	void face_left()
	{
		m_facing_left = true;
	}

	void face_right()
	{
		m_facing_left = false;
	}

	void dont_move()
	{
		m_movement.x = 0.0f;

		if (!m_attacking)
		{
			m_animation_indices = m_animation[IDLE];
		}
	}

	void move_left()
	{
		m_movement.x = -1.0f;

		m_animation_indices = m_animation[MOVE];
		face_left();

		if (m_collided_bottom)
		{
			Mix_PlayChannel(-1, m_walk_sfx[get_random_sfx_index(WALK_SFX_COUNT)], 0);
		}
	}

	void move_right()
	{
		m_movement.x = 1.0f;

		m_animation_indices = m_animation[MOVE];
		face_right();

		if (m_collided_bottom)
		{
			Mix_PlayChannel(-1, m_walk_sfx[get_random_sfx_index(WALK_SFX_COUNT)], 0);
		}
	}

	void attack(Entity* hittable_entities, int hittable_entity_count)
	{
		Mix_PlayChannel((m_entity_type == PLAYER) ? 3 : 4, m_grunt_sfx[get_random_sfx_index(GRUNT_SFX_COUNT)], 0);

		m_animation_indices = m_animation[ATTACK];
		m_animation_index = 0;

		for (int i = 0; i < hittable_entity_count; i++)
		{
			if (glm::distance(m_position, hittable_entities[i].m_position) < m_lethal_distance && hittable_entities[i].m_is_active)
			{
				Mix_PlayChannel((m_entity_type == PLAYER) ? 5 : 6, m_hit_sfx[get_random_sfx_index(HIT_SFX_COUNT)], 0);

				if (hittable_entities[i].m_entity_type == ENEMY)
				{
					hittable_entities[i].m_ai_state = DYING;
				}
				else
				{
					hittable_entities[i].die();
				}
				
			}
		}

		m_attacking = true;
	}

	void die()
	{
		m_animation_indices = m_animation[DIE];
		m_animation_index = 0;

		m_movement = glm::vec3(0.0f);

		m_dying = true;
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

	float const get_last_attack_time() const
	{
		return m_last_attack_time;
	}

	bool const get_dying() const
	{
		return m_dying;
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

	void const set_last_attack_time(float new_attack_time)
	{
		m_last_attack_time = new_attack_time;
	}

	void set_animation(int new_animation[4][8])
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				m_animation[i][j] = new_animation[i][j];
			}
		}
	}

	void set_walk_sfx(Mix_Chunk* new_walk_sfx[WALK_SFX_COUNT])
	{
		for (int i = 0; i < WALK_SFX_COUNT; i++)
		{
			m_walk_sfx[i] = new_walk_sfx[i];
		}
	}

	void set_hit_sfx(Mix_Chunk* new_hit_sfx[HIT_SFX_COUNT])
	{
		for (int i = 0; i < HIT_SFX_COUNT; i++)
		{
			m_hit_sfx[i] = new_hit_sfx[i];
		}
	}

	void set_grunt_sfx(Mix_Chunk* new_grunt_sfx[GRUNT_SFX_COUNT])
	{
		for (int i = 0; i < GRUNT_SFX_COUNT; i++)
		{
			m_grunt_sfx[i] = new_grunt_sfx[i];
		}
	}
};

#endif