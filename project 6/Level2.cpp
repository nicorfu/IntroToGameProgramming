#include "Level2.h"
#include "Utility.h"


const char FONTSHEET_FILEPATH[] = "assets/visual/font.png";

constexpr int FONTBANK_SIZE = 16;

constexpr char MAP_TILESET_FILEPATH[] = "assets/visual/medieval_tileset.png";
const float TILE_SIZE = 1.0f;
const int TILE_COUNT_X = 16;
const int TILE_COUNT_Y = 40;

const int LEVEL_WIDTH = 18;
const int LEVEL_HEIGHT = 14;
unsigned int LEVEL2_DATA[] =
{
	154, 273, 274, 276, 277, 273, 274, 230, 277, 230, 274, 276, 277, 273, 274, 276, 277, 153,
	170, 289, 290, 292, 293, 289, 290, 246, 293, 246, 290, 292, 293, 289, 290, 292, 293, 169,
	186,  32,  52,  19,  81,  64,  97,  65,  33,  37,  36,   4,  19,  33,  52,   2,  16, 185,
	154,  65,  64,  96,  80,  17,  52,  97,  20,   0,   5,  16,  52,  39,  21,  51,   3, 153,
	170, 273, 274,  33,  53,  23,  16,  64,  65,  64,  81,  80,  81,  96,  64,  81,   6, 169,
	186, 289, 290,  19,  17,  65,  33,  81,  96,  97,  34,   3,  37,  97,  39,   5,  36, 185,
	154,  51,  33,  34,  36,  80,   5,  80,   1,  52,   6,  37,  33,   1,  54,  33,  23, 153,
	170,  17,   0,  65,  96,  81,  97,  65,  96,  52,  64,  81,   6,  96,   0,  50,   5, 169,
	186,   4,  21,  64,  52,  19,  33,  23,  97,  36,  51,  64,  80,  81,  65,  65,  21, 185,
	154,  55,  17,  80,  39,   0,   2,  48,  96,  20,  53,  65,  35,   3,  36,   1,  17, 153,
	170,  35,  35,  96,  18,  55,  16,  48,  80,  64,  81,  96,  17,   0, 274, 276, 277, 169,
	186,   6,   6,  81,  64,  64,   1,  36,   3,  35,  21,  53,  48,   0, 290, 292, 293, 185,
	154, 273, 230, 276, 230, 273, 274, 230, 277, 230, 274, 276, 277, 273,  -1,  -1,  -1,  -1,
	170, 289, 246, 292, 246, 289, 290, 246, 293, 246, 290, 292, 293, 289,  -1,  -1,  -1,  -1
};

constexpr char PORTAL_FILEPATH[] = "assets/visual/purple_portal.png";

constexpr char COIN_FILEPATH[] = "assets/visual/coin.png";

constexpr char PLAYER_FILEPATH[] = "assets/visual/hamza_spritesheet.png";
constexpr char ENEMY_FILEPATH[] = "assets/visual/akram_spritesheet.png";

const char WALK_SFX_FILEPATH[] = "assets/audio/stone_walk_1.wav";
const char HAMZAATTACK_SFX_FILEPATH[] = "assets/audio/hamza_attack.wav";
const char PUNCH_SFX_FILEPATH[] = "assets/audio/punch.wav";
const char HAMZADIE_SFX_FILEPATH[] = "assets/audio/hamza_die.wav";
const char COIN_SFX_FILEPATH[] = "assets/audio/coin_grab.wav";


Level2::~Level2()
{
	delete[] m_state.enemies;
	delete m_state.player;

	delete[] m_state.coins;

	delete m_state.portal;

	delete m_state.map;

	delete m_state.text;

	Mix_FreeChunk(m_state.walk_sfx);
	Mix_FreeChunk(m_state.hamza_attack_sfx);
	Mix_FreeChunk(m_state.punch_sfx);
	Mix_FreeChunk(m_state.hamza_die_sfx);
	Mix_FreeChunk(m_state.coin_sfx);
}


void Level2::initialize()
{
	GLuint font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
	m_state.text = new Entity();
	m_state.text->set_texture_id(font_texture_id);

	GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
	m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL2_DATA, map_texture_id, TILE_SIZE, TILE_COUNT_X, TILE_COUNT_Y);

	GLuint coin_texture_id = Utility::load_texture(COIN_FILEPATH);

	glm::vec3 coin_scale = glm::vec3(1.0f, 1.0f, 0.0f) * 0.5f;
	glm::vec3 coin1_position = glm::vec3(1.0f, -3.0f, 0.0f);
	glm::vec3 coin2_position = glm::vec3(5.0f, -11.0f, 0.0f);
	glm::vec3 coin3_position = glm::vec3(15.0f, -8.0f, 0.0f);

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

	m_state.coins[0].set_position(coin1_position);
	m_state.coins[1].set_position(coin2_position);
	m_state.coins[2].set_position(coin3_position);

	GLuint portal_texture_id = Utility::load_texture(PORTAL_FILEPATH);

	glm::vec3 portal_scale = glm::vec3(1.0f, 1.4f, 0.0f) * 1.0f;
	glm::vec3 portal_position = glm::vec3(15.0f, -3.7f, 0.0f);

	m_state.portal = new Entity
	(
		PORTAL,
		portal_texture_id,
		portal_scale,
		0.5f,
		0.5f
	);

	m_state.portal->set_position(portal_position);

	GLuint player_texture_id = Utility::load_texture(PLAYER_FILEPATH);

	glm::vec3 player_scale = glm::vec3(1.0f, 1.35f, 0.0f) * 1.6f;
	glm::vec3 player_position = glm::vec3(8.0f, -4.3f, 0.0f);
	const float player_speed = 2.1f;

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
	glm::vec3 enemy1_position = glm::vec3(3.0f, -7.0f, 0.0f);
	glm::vec3 enemy2_position = glm::vec3(15.0f, -8.0f, 0.0f);
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

	m_state.enemies = new Entity[ENEMY_COUNT];
	
	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		m_state.enemies[i] = Entity
		(
			ENEMY,
			enemy_texture_id,
			enemy_scale,
			glm::vec3(0.0f),
			0.45f,
			0.4f,
			enemy_speed,
			24,
			8,
			4,
			0,
			0.0f,
			enemy_animation
		);

		m_state.enemies[i].set_ai_type(WALKER);
		m_state.enemies[i].set_ai_state(IDLING);
	}

	float walking_range1[2] = { -7.0f, -11.0f };
	float walking_range2[2] = { 11.0f, 15.0f };

	m_state.enemies[0].set_position(enemy1_position);
	m_state.enemies[0].set_ai_walking_orientation(VERTICAL);
	m_state.enemies[0].set_ai_walking_range(walking_range1);

	m_state.enemies[1].set_position(enemy2_position);
	m_state.enemies[1].set_ai_walking_orientation(HORIZONTAL);
	m_state.enemies[1].set_ai_walking_range(walking_range2);

	m_state.walk_sfx = Mix_LoadWAV(WALK_SFX_FILEPATH);
	Mix_VolumeChunk(m_state.walk_sfx, int(MIX_MAX_VOLUME * 0.05));

	m_state.hamza_attack_sfx = Mix_LoadWAV(HAMZAATTACK_SFX_FILEPATH);
	Mix_VolumeChunk(m_state.hamza_attack_sfx, int(MIX_MAX_VOLUME * 0.3));

	m_state.punch_sfx = Mix_LoadWAV(PUNCH_SFX_FILEPATH);
	Mix_VolumeChunk(m_state.punch_sfx, int(MIX_MAX_VOLUME * 0.3));

	m_state.hamza_die_sfx = Mix_LoadWAV(HAMZADIE_SFX_FILEPATH);
	Mix_VolumeChunk(m_state.hamza_die_sfx, int(MIX_MAX_VOLUME * 0.3));

	m_state.coin_sfx = Mix_LoadWAV(COIN_SFX_FILEPATH);
	Mix_VolumeChunk(m_state.coin_sfx, int(MIX_MAX_VOLUME * 0.4));
}


void Level2::update(float delta_time, float curr_ticks)
{
	for (int i = 0; i < COIN_COUNT; i++)
	{
		m_state.coins[i].update(delta_time, m_state.player, m_state.player, 1, m_state.map, curr_ticks);
	}

	m_state.portal->update(delta_time, m_state.player, m_state.player, 1, m_state.map, curr_ticks);

	m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map, curr_ticks);

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		m_state.enemies[i].update(delta_time, m_state.player, m_state.player, 1, m_state.map, curr_ticks);
	}

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
				m_state.portal->set_portal_touched(false);
				break;
			}
		}

		for (int i = 0; (i < COIN_COUNT) && level_done; i++)
		{
			if (m_state.coins[i].get_is_active())
			{
				level_done = false;
				m_state.portal->set_portal_touched(false);
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

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		m_state.enemies[i].render(g_shader_program);
	}

	m_state.player->render(g_shader_program);

	m_state.portal->render(g_shader_program);

	if (!m_state.player->get_is_active())
	{
		glm::vec3 text_position = glm::vec3(m_state.player->get_position().x - 2.2f, m_state.player->get_position().y + 2.0f, 0.0f);
		m_state.text->draw_text(g_shader_program, "YOU LOST", 0.6f, 0.0001f, text_position, FONTBANK_SIZE);
	}
}