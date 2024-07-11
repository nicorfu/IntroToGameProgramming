enum EntityType { PLATFORM, PLAYER };

class Entity
{
private:
	bool m_is_active = true;

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

	Entity();

	void update(float delta_time, Entity* collidable_entities, int collidable_entity_count);
	void render(ShaderProgram* program);

	void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
	void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
	bool const check_collision(Entity* other) const;

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