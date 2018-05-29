#pragma once

#include <vector>
#include <queue>
#include <fstream>

#include <nlohmann\json.hpp>
using json = nlohmann::json;

#include "QcEngine.hpp"

#include "entities\gameObject.hpp"
#include "entities\player.hpp"
#include "entities\enemy.hpp"

#define KEY_RIGHT	(1 << 0)
#define KEY_LEFT	(1 << 1)
#define KEY_SPACE	(1 << 2)
#define KEY_ATTACK_DOWN	(1 << 3)
#define KEY_ATTACK_UP   (1 << 4)
#define KEY_UP		(1 << 5)

#define MAP_FOLDER			"../assets/maps/"
#define DEFAULT_MAP_FILE	"../assets/maps/default.txt"

#define CAMERA_SPEED 700

#define TILEMAP	"tilemap"

#define ARROW		"arrow"
#define ARROW_IMG	"../assets/arrow.png"
#define PORTAL		"portal"
#define PORTAL_IMG	"../assets/portal.png"

// -------------------------------------------------------------------------------
// UTILITY STRUCT TO STORE PORTAL DATA LOADED FROM MAPS
// -------------------------------------------------------------------------------
struct PortalEntry {
	int x;
	int y;
	std::string file;
	int num;
};

// -------------------------------------------------------------------------------
// GLOBAL STRUCT CONTAINING MAIN GAME DATA
// -------------------------------------------------------------------------------
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
	std::vector<PortalEntry> portals;

	// -------------------------------------------------------------------------------
	// GAME STATE VARIABLES
	// -------------------------------------------------------------------------------
	int keyStates;
	int cam_x;
	int cam_y;
	bool change_state;
	std::string next_map;
	int next_index;

	// UTILITY FUNCTIONS (MAY MOVE LATER)
	bool collidingWithTiles(const Math::Shape& collision, int range = 0, bool quad = false);
};

class Game : public State {

public:

	Game();
	Game(const std::string& path);
	Game(const std::string& path, int start_index);
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

	// variables to keep track of map transitions and data
	std::string current_map;
	int start_index;

	// -------------------------------------------------------------------------------
	// UTILITY FUNCTIONS
	// -------------------------------------------------------------------------------
	void handleKeyPresses();
	void addEntity(GameObject * obj);
	void updateCamera();
	void clearMap();
	void loadMap(const std::string& path = DEFAULT_MAP_FILE, bool verbose = false);
	
	inline int tileIndex(int x, int y) const {
		return y * data->map_width + x;
	}
};