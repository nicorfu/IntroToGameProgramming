#include "Scene.h"


class Level2 : public Scene
{
public:
	int COIN_COUNT = 3;
	int ENEMY_COUNT = 1;

	~Level2();

	void initialize() override;
	void update(float delta_time, float curr_ticks) override;
	void render(ShaderProgram* shader_program) override;
};