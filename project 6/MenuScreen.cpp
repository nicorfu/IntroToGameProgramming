#include "MenuScreen.h"
#include "Utility.h"


const char FONTSHEET_FILEPATH[] = "assets/visual/font.png";

constexpr int FONTBANK_SIZE = 16;


glm::vec3 text_position1 = glm::vec3(-4.25f, 2.5f, 0.0f);
glm::vec3 text_position2 = glm::vec3(-2.5f, -0.75f, 0.0f);
glm::vec3 text_position3 = glm::vec3(-1.75f, -1.5f, 0.0f);


MenuScreen::~MenuScreen()
{
	delete m_state.text;
}


void MenuScreen::initialize()
{
	GLuint font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
	m_state.text = new Entity();
	m_state.text->set_texture_id(font_texture_id);
}


void MenuScreen::update(float delta_time, float curr_ticks)
{
}


void MenuScreen::render(ShaderProgram* g_shader_program)
{
	m_state.text->draw_text(g_shader_program, "HAMZA'S REVENGE", 0.6f, 0.0001f, text_position1, FONTBANK_SIZE);
	m_state.text->draw_text(g_shader_program, "PRESS ENTER", 0.5f, 0.0001f, text_position2, FONTBANK_SIZE);
	m_state.text->draw_text(g_shader_program, "TO START", 0.5f, 0.0001f, text_position3, FONTBANK_SIZE);
}