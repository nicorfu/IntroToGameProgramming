#include "Level1.h"
#include "Utility.h"


constexpr char MAP_TILESET_FILEPATH[] = "assets/visual/summer_plains_tileset.png";
const float TILE_SIZE = 1.0f;
const int TILE_COUNT_X = 13;
const int TILE_COUNT_Y = 12;

const int LEVEL_WIDTH = 10;
const int LEVEL_HEIGHT = 3;
unsigned int LEVEL_DATA[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

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

	GLuint player_texture_id = Utility::load_texture(PLAYER_FILEPATH);

	glm::vec3 player_scale = glm::vec3(1.0f, 1.25f, 0.0f) * 1.3f;
	glm::vec3 player_position = glm::vec3(5.0f, -0.5f, 0.0f);
	const float player_speed = 2.4f;

	int player_animation[12][4] =
	{
		{  24,  26,  28,  30 },		// idle up
		{   0,   2,   4,   6 },		// idle down
		{   8,  10,  12,  14 },		// idle left
		{  40,  42,  44,  46 },		// idle right
		{  72,  74,  76,  68 },		// walk up
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
		glm::vec3(0.0f),
		0.58f,
		1.5f,
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
	m_state.player->update(delta_time, m_state.player, m_state.enemies, 0, m_state.map, 0.0f);

	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		//m_state.enemies[i].update(delta_time, m_state.player, m_state.player, 1, m_state.map, 0.0f);
	}
}


void Level1::render(ShaderProgram* g_shader_program)
{
	m_state.map->render(g_shader_program);
	m_state.player->render(g_shader_program);
	for (int i = 0; i < ENEMY_COUNT; i++)
	{
		//m_state.enemies[i].render(g_shader_program);
	}
}