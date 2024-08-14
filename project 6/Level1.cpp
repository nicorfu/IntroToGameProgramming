#include "Level1.h"
#include "Utility.h"


constexpr char MAP_TILESET_FILEPATH[] = "assets/visual/medieval_tileset.png";
const float TILE_SIZE = 1.0f;
const int TILE_COUNT_X = 16;
const int TILE_COUNT_Y = 40;

const int LEVEL_WIDTH = 10;
const int LEVEL_HEIGHT = 10;
unsigned int LEVEL_DATA[] =
{
	154, 225, 226, 227, 228, 225, 226, 227, 228, 225,
	170,  81,  96,  65,  97,   6,   1,  53,  53,  18,
	186,   4,  21,  80,   5,  19,  22,  32,   4,  39,
	154,  18,  39,  64,  17,  80,  37,  54,  23,  50,
	170,  35,  50,  81,  65,  97,  64,  96,  65,  81,
	186,  52,   2,   0,  81,  33,   0,  20,   0,  22,
	154,  65,  97,  48,  96,  49,  16,  38,   7,  34,
	170,   5,  81,  80,  97,  50,    3, 18,  39,  53,
	186,   1,  21,  64,  37,  16,    7, 33,  48,  24,
	 -1, 225, 226,  65,  32,  52,    4, 20,  35,  55
};

constexpr char COIN_FILEPATH[] = "assets/visual/coin.png";

constexpr char PLAYER_FILEPATH[] = "assets/visual/hamza_spritesheet.png";


Level1::~Level1()
{
	//delete[] m_state.enemies;
	delete m_state.player;

	delete m_state.map;

	Mix_FreeMusic(m_state.music);
}


void Level1::initialize()
{
	GLuint map_texture_id = Utility::load_texture(MAP_TILESET_FILEPATH);
	m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, TILE_SIZE, TILE_COUNT_X, TILE_COUNT_Y);

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
			1.0f,
			1.0f
		);
	}

	m_state.coins[0].set_position(glm::vec3(8.0f, -4.0f, 0.0f));

	GLuint player_texture_id = Utility::load_texture(PLAYER_FILEPATH);

	glm::vec3 player_scale = glm::vec3(1.0f, 1.35f, 0.0f) * 1.6f;
	glm::vec3 player_position = glm::vec3(0.75f, -1.0f, 0.0f);
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
		7.0f,
		8,
		18,
		4,
		0,
		0.0f,
		player_animation
	);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	//m_state.music = Mix_LoadMUS("assets/audio/.mp3");
	Mix_PlayMusic(m_state.music, -1);
	Mix_VolumeMusic(0.0f);
}


void Level1::update(float delta_time)
{
	for (int i = 0; i < COIN_COUNT; i++)
	{
		m_state.coins[i].update(delta_time, m_state.player, nullptr, 0, m_state.map, 0.0f);
	}

	m_state.player->update(delta_time, m_state.player, m_state.enemies, 0, m_state.map, 0.0f);

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		//m_state.enemies[i].update(delta_time, m_state.player, m_state.player, 1, m_state.map, 0.0f);
	}
}


void Level1::render(ShaderProgram* g_shader_program)
{
	m_state.map->render(g_shader_program);

	for (int i = 0; i < COIN_COUNT; i++)
	{
		m_state.coins[i].render(g_shader_program);
	}
	
	m_state.player->render(g_shader_program);

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		//m_state.enemies[i].render(g_shader_program);
	}
}