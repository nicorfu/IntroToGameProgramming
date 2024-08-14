#include "Scene.h"


class MenuScreen : public Scene
{
	~MenuScreen();

	void initialize() override;
	void update(float delta_time) override;
	void render(ShaderProgram* shader_program) override;
};