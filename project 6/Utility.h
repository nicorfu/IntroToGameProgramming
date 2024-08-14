#include <GL/glew.h>

#pragma once

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"


class Utility
{
public:
	static GLuint load_texture(const char* filepath);
};