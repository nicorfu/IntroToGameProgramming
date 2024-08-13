/**
* Author: Nico Flores
* Assignment: [Your game's name here]
* Date due: 2024-08-15, 1:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define NUMBER_OF_TEXTURES 1 
#define LEVEL_OF_DETAIL 0    
#define TEXTURE_BORDER 0     
#define FONTBANK_SIZE 16

#include "Utility.h"
#include <SDL_image.h>
#include "stb_image.h"

GLuint Utility::load_texture(const char* filepath)
{
	int width;
	int height;
	int num_components;

	unsigned char* image = stbi_load(filepath, &width, &height, &num_components, STBI_rgb_alpha);

	if (image == NULL)
	{
		LOG("Can't load image. Smh. Check filepath.");
		assert(false);
	}

	GLuint textureID;
	glGenTextures(NUMBER_OF_TEXTURES, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
		GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(image);

	return textureID;
}


void Utility::draw_text(ShaderProgram* shader_program, GLuint font_texture_id, std::string text, float font_size, float spacing, glm::vec3 position,
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

	glBindTexture(GL_TEXTURE_2D, font_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

	glDisableVertexAttribArray(shader_program->get_position_attribute());
	glDisableVertexAttribArray(shader_program->get_tex_coordinate_attribute());
}