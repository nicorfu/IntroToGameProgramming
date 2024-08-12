#include "Scene.h"

class Level1 : public Scene
{
public:
	int ENEMY_COUNT = 1;

	~Level1();

	void initialize() override;
	void update(float delta_time) override;
	void render(ShaderProgram* shader_program) override;
};