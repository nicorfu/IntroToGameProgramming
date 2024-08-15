#include "Scene.h"


class Level3 : public Scene
{
public:
	int COIN_COUNT = 4;
	int ENEMY_COUNT = 3;

	~Level3();

	void initialize() override;
	void update(float delta_time, float curr_ticks) override;
	void render(ShaderProgram* shader_program) override;
};