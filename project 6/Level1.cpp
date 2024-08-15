#include "Level1.h"
#include "Utility.h"


const char FONTSHEET_FILEPATH[] = "assets/visual/font.png";

constexpr int FONTBANK_SIZE = 16;

constexpr char MAP_TILESET_FILEPATH[] = "assets/visual/medieval_tileset.png";
const float TILE_SIZE = 1.0f;
const int TILE_COUNT_X = 16;
const int TILE_COUNT_Y = 40;

const int LEVEL_WIDTH = 16;
const int LEVEL_HEIGHT = 13;
unsigned int LEVEL1_DATA[] =
{
	154, 225, 226, 227, 228, 225, 226, 227, 228, 225, 226, 227, 228, 230, 225, 153,
	170, 241, 242, 243, 244, 241, 242, 243, 244, 241, 242, 243, 244, 246, 241, 169,
	186,  81,  96,  65,  97,   6,   1,  53,  53,  18,   5,  22,  34,  50,   3, 185,
	154,   4,  21,  80,   5,  19,  22,  32,   4,  39,  64,  65,  65,  81,  21, 153,
	170,  18,  39,  64,  17,  80,  37,  54,  23,  50,  81,  16,   7,  33,  48, 169,
	186,  35,  50,  81,  65,  97,  64,  96,  65,   0,  80,   6,  19,  32,  52, 185,
	154,  52,   2,   0,  81,  33,   0,  20,  81,  96,  65,  20,  35,  55,   2, 153,
	170,  65,  97,  48,  96,  49,  16,  38,   7,  96,  23,  38,  49,   0,  17, 169,
	186,   5,  81,  80,  97,  50,  65,  80,  97,  81,  16,  22,   7,   7,  39, 185,
	154,   1,  21,  64,  37,  16,   7,  33,  48,  23,  54,   0,   4,  55,  19, 153,
	170, 225, 226,  65,  32,  52,   4,  20,  35,  55,  33,  55, 228, 225, 226, 169,
	186, 241, 242, 225, 226, 227, 228, 225, 230, 225, 226, 227, 244, 241, 242, 185,
	 -1,  -1,  -1, 241, 242, 243, 244, 241, 246, 241, 242, 243,  -1,  -1,  -1,  -1
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
const char BOOM_SFX_FILEPATH[] = "assets/audio/vine_boom.wav";


Level1::~Level1()
{
	delete m_state.enemies;
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
	Mix_FreeChunk(m_state.boom_sfx);
}


void Level1::initialize()
{
	GLuint font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
	m_state.text = new Entity();
	m_state.text->set_texture_id(font_texture_id);

	GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
	m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL1_DATA, map_texture_id, TILE_SIZE, TILE_COUNT_X, TILE_COUNT_Y);

	GLuint coin_texture_id = Utility::load_texture(COIN_FILEPATH);

	glm::vec3 coin_scale = glm::vec3(1.0f, 1.0f, 0.0f) * 0.5f;
	glm::vec3 coin1_position = glm::vec3(5.0f, -4.0f, 0.0f);
	glm::vec3 coin2_position = glm::vec3(13.0f, -3.0f, 0.0f);
	glm::vec3 coin3_position = glm::vec3(6.0f, -8.0f, 0.0f);

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
	glm::vec3 portal_position = glm::vec3(1.0f, -6.7f, 0.0f);

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
	glm::vec3 player_position = glm::vec3(0.75f, -2.0f, 0.0f);
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
	glm::vec3 enemy_position = glm::vec3(10.0f, -3.0f, 0.0f);
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
		enemy_position,
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

	float walking_range[2] = {-3.0f, -6.0f};

	m_state.enemies->set_ai_type(WALKER);
	m_state.enemies->set_ai_state(IDLING);
	m_state.enemies->set_ai_walking_orientation(VERTICAL);
	m_state.enemies->set_ai_walking_range(walking_range);

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

	m_state.boom_sfx = Mix_LoadWAV(BOOM_SFX_FILEPATH);
	Mix_VolumeChunk(m_state.boom_sfx, int(MIX_MAX_VOLUME * 0.8));
}


void Level1::update(float delta_time, float curr_ticks)
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
			m_state.next_scene_index = 2;
		}
	}
}


void Level1::render(ShaderProgram* g_shader_program)
{
	m_state.map->render(g_shader_program);

	for (int i = 0; i < COIN_COUNT; i++)
	{
		m_state.coins[i].render(g_shader_program);
	}

	m_state.enemies->render(g_shader_program);
	
	m_state.player->render(g_shader_program);

	m_state.portal->render(g_shader_program);

	if (!m_state.player->get_is_active())
	{
		glm::vec3 text_position = glm::vec3(m_state.player->get_position().x - 2.2f, m_state.player->get_position().y + 2.0f, 0.0f);
		m_state.text->draw_text(g_shader_program, "YOU LOST", 0.6f, 0.0001f, text_position, FONTBANK_SIZE);
	}
}