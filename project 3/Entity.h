enum EntityType { PLATFORM, PLAYER, ARROW, EXPLOSION };

class Entity
{
private:
	bool m_is_active = true;

	glm::vec3 m_scale;
	glm::vec3 m_rotation;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;

	float m_width = 1;
	float m_height = 1;

public:
	GLuint m_texture_id;
	glm::mat4 m_model_matrix;
	EntityType m_type;

	float m_speed;
	glm::vec3 m_movement;

	bool m_collided_top = false;
	bool m_collided_bottom = false;
	bool m_collided_left = false;
	bool m_collided_right = false;

	Entity();

	void update(float delta_time, Entity* collidable_entities, int collidable_entity_count, bool *game_ongoing, 
		bool *mission_failed, int target_platform);
	void render(ShaderProgram* program);

	void const check_collision_y(Entity* collidable_entities, int collidable_entity_count, bool *game_ongoing, 
		bool *mission_failed, int target_platform);
	void const check_collision_x(Entity* collidable_entities, int collidable_entity_count, bool *game_ongoing, 
		bool *mission_failed);
	bool const check_collision(Entity* other) const;
	void draw_text(ShaderProgram* shader_program, std::string text, float font_size, float spacing, glm::vec3 position);

	void activate() 
	{
		m_is_active = true; 
	};
	void deactivate() 
	{ 
		m_is_active = false; 
	};

	glm::vec3 const get_position() const 
	{ 
		return m_position; 
	};
	glm::vec3 const get_movement() const 
	{ 
		return m_movement; 
	};
	glm::vec3 const get_velocity() const 
	{ 
		return m_velocity; 
	};
	glm::vec3 const get_acceleration() const 
	{ 
		return m_acceleration; 
	};
	int const get_width() const 
	{ 
		return m_width; 
	};
	int const get_height() const 
	{ 
		return m_height; 
	};

	void const set_scale(glm::vec3 new_scale)
	{
		m_scale = new_scale;
	};
	void const set_rotation(glm::vec3 new_rotation)
	{
		m_rotation = new_rotation;
	};
	void const set_position(glm::vec3 new_position) 
	{ 
		m_position = new_position; 
	};
	void const set_movement(glm::vec3 new_movement) 
	{ 
		m_movement = new_movement; 
	};
	void const set_velocity(glm::vec3 new_velocity) 
	{ 
		m_velocity = new_velocity; 
	};
	void const set_acceleration(glm::vec3 new_acceleration) 
	{ 
		m_acceleration = new_acceleration; 
	};
	void const set_width(float new_width) 
	{ 
		m_width = new_width; 
	};
	void const set_height(float new_height) 
	{ 
		m_height = new_height; 
	};
};