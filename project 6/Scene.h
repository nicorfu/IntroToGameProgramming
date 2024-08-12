#include <GL/glew.h>

#pragma once

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"


struct GameState
{
	Map* map;

	Entity* player;
	Entity* enemies;

	Mix_Music* music;

	int next_scene_id;
};


class Scene
{
public:
	GameState m_game_state;

	int m_num_enemies = 1;

	virtual void initialize() = 0;
	virtual void update(float delta_time) = 0;
	virtual void render(ShaderProgram* shader_program) = 0;	

	GameState const get_state() const
	{
		return m_game_state;
	}

	int const get_num_enemies() const
	{
		return m_num_enemies;
	}
};