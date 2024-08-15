#include "Level2.h"
#include "Utility.h"


constexpr char MAP_TILESET_FILEPATH[] = "assets/visual/medieval_tileset.png";
const float TILE_SIZE = 1.0f;
const int TILE_COUNT_X = 16;
const int TILE_COUNT_Y = 40;

const int LEVEL_WIDTH = 5;
const int LEVEL_HEIGHT = 2;
unsigned int LEVEL2_DATA[] =
{
	64, 65, 64, 65, 64,
	64, 65, 64, 65, 64,
};

constexpr char PORTAL_FILEPATH[] = "assets/visual/purple_portal.png";

constexpr char COIN_FILEPATH[] = "assets/visual/coin.png";

constexpr char PLAYER_FILEPATH[] = "assets/visual/hamza_spritesheet.png";
constexpr char ENEMY_FILEPATH[] = "assets/visual/akram_spritesheet.png";


Level2::~Level2()
{
	delete m_state.enemies;
	delete m_state.player;

	delete[] m_state.coins;

	delete m_state.portal;

	delete m_state.map;

	Mix_FreeMusic(m_state.music);
}


void Level2::initialize()
{
	GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
	m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL2_DATA, map_texture_id, TILE_SIZE, TILE_COUNT_X, TILE_COUNT_Y);

	GLuint coin_texture_id = Utility::load_texture(COIN_FILEPATH);

	glm::vec3 coin_scale = glm::vec3(1.0f, 1.0f, 0.0f) * 0.5f;

	m_state.coins = new Entity[COIN_COUNT];

	for (int i = 0; i < COIN_COUNT; i++)
	{
		m_state.coins[i] = Entity
		(
			COIN,
			coin_texture_id,
			coin_scale,
			0.1f,
			0.1f
		);
	}

	m_state.coins[0].set_position(glm::vec3(1.0f, -1.0f, 0.0f));
	m_state.coins[1].set_position(glm::vec3(2.0f, -1.0f, 0.0f));
	m_state.coins[2].set_position(glm::vec3(3.0f, -1.0f, 0.0f));

	GLuint portal_texture_id = Utility::load_texture(PORTAL_FILEPATH);

	glm::vec3 portal_scale = glm::vec3(1.0f, 1.4f, 0.0f) * 1.0f;

	m_state.portal = new Entity
	(
		PORTAL,
		portal_texture_id,
		portal_scale,
		0.5f,
		0.5f
	);

	m_state.portal->set_position(glm::vec3(5.0f, -1.0f, 0.0f));

	GLuint player_texture_id = Utility::load_texture(PLAYER_FILEPATH);

	glm::vec3 player_scale = glm::vec3(1.0f, 1.35f, 0.0f) * 1.6f;
	glm::vec3 player_position = glm::vec3(0.0f, 0.0f, 0.0f);
	const float player_speed = 1.7f;

	int player_animation[12][4] =
	{
		{  24,  26,  28,  30 },		// idle up
		{   0,   2,   4,   6 },		// idle down
		{   8,  10,  12,  14 },		// idle left
		{  40,  42,  44,  46 },		// idle right
		{  72,  74,  76,  78 },		// walk up
		{  48,  50,  52,  54 },		// walk down
		{  56,  58,  60,  62 },		// walk left
		{  88,  90,  92,  94 },		// walk right
		{ 120, 122, 124, 126 },		// die up
		{  96,  98, 100, 102 },		// die down
		{ 104, 106, 108, 110 },		// die left
		{ 136, 138, 140, 142 },		// die right
	};

	m_state.player = new Entity
	(
		PLAYER,
		player_texture_id,
		player_scale,
		player_position,
		0.4f,
		0.8f,
		player_speed,
		8,
		18,
		4,
		0,
		0.0f,
		player_animation
	);

	GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

	glm::vec3 enemy_scale = glm::vec3(0.8f, 1.0f, 0.0f) * 1.6f;
	const float enemy_speed = 1.8f;

	int enemy_animation[12][4] =
	{
		{  96,  97,  96,  97 },		// idle up
		{   0,   1,   0,   1 },		// idle down
		{ 144, 145, 144, 145 },		// idle left
		{  48,  49,  48,  49 },		// idle right
		{  98,  99, 100,  99 },		// walk up
		{   2,   3,   4,   3 },		// walk down
		{ 146, 147, 148, 147 },		// walk left
		{  50,  51,  52,  51 },		// walk right
		{ 116, 117, 118, 119 },		// die up
		{  20,  21,  22,  23 },		// die down
		{ 164, 165, 166, 167 },		// die left
		{  68,  69,  70,  71 }		// die right
	};

	m_state.enemies = new Entity
	(
		ENEMY,
		enemy_texture_id,
		enemy_scale,
		glm::vec3(0.0f),
		0.6f,
		0.7f,
		0.0f,
		24,
		8,
		4,
		0,
		0.0f,
		enemy_animation
	);

	float walking_range[2] = { -3.0f, -6.0f };

	m_state.enemies->set_ai_type(WALKER);
	m_state.enemies->set_ai_state(IDLING);
	m_state.enemies->set_position(glm::vec3(10.0f, -3.0f, 0.0f));
	m_state.enemies->set_speed(enemy_speed);
	m_state.enemies->set_ai_walking_orientation(VERTICAL);
	m_state.enemies->set_ai_walking_range(walking_range);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	//m_state.music = Mix_LoadMUS("assets/audio/.mp3");
	Mix_PlayMusic(m_state.music, -1);
	Mix_VolumeMusic(0.0f);
}


void Level2::update(float delta_time, float curr_ticks)
{
	for (int i = 0; i < COIN_COUNT; i++)
	{
		m_state.coins[i].update(delta_time, m_state.player, m_state.player, 1, m_state.map, curr_ticks);
	}

	m_state.portal->update(delta_time, m_state.player, m_state.player, 1, m_state.map, curr_ticks);

	m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map, curr_ticks);

	m_state.enemies->update(delta_time, m_state.player, m_state.player, 1, m_state.map, curr_ticks);

	if (m_state.player->get_attacking())
	{
		m_state.player->attack(m_state.enemies, ENEMY_COUNT);

		m_state.player->set_last_attack_time(curr_ticks);

		m_state.player->set_attacking(false);
	}

	if (m_state.portal->get_portal_touched())
	{
		bool level_done = true;

		for (int i = 0; i < ENEMY_COUNT; i++)
		{
			if (m_state.enemies[i].get_is_active())
			{
				level_done = false;
				break;
			}
		}

		for (int i = 0; (i < COIN_COUNT) && level_done; i++)
		{
			if (m_state.coins[i].get_is_active())
			{
				level_done = false;
				break;
			}
		}

		if (level_done)
		{
			m_state.next_scene_index = 3;
		}
	}
}


void Level2::render(ShaderProgram* g_shader_program)
{
	m_state.map->render(g_shader_program);

	for (int i = 0; i < COIN_COUNT; i++)
	{
		m_state.coins[i].render(g_shader_program);
	}

	m_state.enemies->render(g_shader_program);

	m_state.player->render(g_shader_program);

	m_state.portal->render(g_shader_program);
}