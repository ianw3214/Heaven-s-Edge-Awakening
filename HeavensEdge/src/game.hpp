#pragma once

#include <vector>
#include <queue>
#include <fstream>

#include "QcEngine.hpp"

#include "entities\gameObject.hpp"
#include "entities\player.hpp"
#include "entities\enemy.hpp"

#define KEY_RIGHT	(1 << 0)
#define KEY_LEFT	(1 << 1)
#define KEY_SPACE	(1 << 2)
#define KEY_ATTACK_DOWN	(1 << 3)
#define KEY_ATTACK_UP   (1 << 4)

#define DEFAULT_MAP_FILE "../assets/maps/default.txt"

#define CAMERA_SPEED 700

#define TILEMAP	"tilemap"

struct GameData {
	// -------------------------------------------------------------------------------
	// GAME ENTITIES
	// -------------------------------------------------------------------------------
	std::vector<GameObject*> entities;
	std::queue<GameObject*> new_entities;

	// -------------------------------------------------------------------------------
	// MAP DATA
	// -------------------------------------------------------------------------------
	int tile_size;
	int map_width;
	int map_height;
	std::vector<int> tilemap;
	std::vector<bool> collisionmap;

	// -------------------------------------------------------------------------------
	// GAME STATE VARIABLES
	// -------------------------------------------------------------------------------
	int keyStates;
	int cam_x;
	int cam_y;

	// UTILITY FUNCTIONS (MAY MOVE LATER)
	bool collidingWithTiles(const Math::Shape& collision, int range = 0, bool quad = false);
};

class Game : public State {

public:

	Game();
	~Game();

	void init();
	void cleanup();
	void pause();
	void resume();
	void update();
	void render();

private:

	GameData * data;

	// keep a player pointer to easily access instead of having to search every time
	Player * player;

	// the tilemap is just a pointer, no ownership
	TileMap * tiles;

	// -------------------------------------------------------------------------------
	// UTILITY FUNCTIONS
	// -------------------------------------------------------------------------------
	void handleKeyPresses();
	void addEntity(GameObject * obj);
	void updateCamera();
	void clearMap();
	void loadMap(const std::string& path = DEFAULT_MAP_FILE);
	
	inline int tileIndex(int x, int y) const {
		return y * data->map_width + x;
	}
};