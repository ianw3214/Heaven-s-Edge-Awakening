#pragma once

#include "QcEngine.hpp"

#include <vector>
#include <fstream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem::v1;

#define DEFAULT_TILE_SIZE	64
#define DEFAULT_MAP_WIDTH	20
#define DEFAULT_MAP_HEIGHT	20
#define PAN_SPEED			300

#define BASE_DIR			"../assets"
#define DEFAULT_FONT		"../assets/fonts/Munro.ttf"

#define TILEMAP(x, y)		(y * DEFAULT_MAP_WIDTH + x)

#define DEFAULT_TILEMAP		"../assets/tilemap.png"
#define DEFAULT_MAP_FILE	"../assets/maps/default.txt"

#define TILE_SELECT			"tile_select"
#define TILE_SELECT_IMG		"../assets/editor/tile_select.png"
#define COLLISION			"collision"
#define COLLISION_IMG		"../assets/editor/collision.png"
#define WHITE_BAR			"white_bar"
#define WHITE_BAR_IMG		"../assets/editor/white_bar.png"
#define BORDER_HOR			"border_horizontal"
#define BORDER_HOR_IMG		"../assets/editor/border_horizontal.png"
#define BORDER_VER			"border_vertical"
#define BORDER_VER_IMG		"../assets/editor/border_vertical.png"
#define BORDER_CORNER		"border_corner"
#define BORDER_CORNER_IMG	"../assets/editor/border_corner.png"

enum ProgramState {
	STATE_EDITOR,
	STATE_FILE,
	STATE_SETTINGS
};

enum EditorState {
	EDITOR_EDIT,
	EDITOR_PANNING
};

enum EditMode {
	EDIT_TILE,
	EDIT_COLLISION
};

// struct representing each file item in the menu
struct FileMenuItem {
	std::string name;
	Math::Rectangle collision;
	Texture * tex;
};

// -------------------------------------------------------------------------------
// MAIN EDITOR CODE
// -------------------------------------------------------------------------------
class Editor : public State {
public:
	Editor();
	~Editor();

	void init();
	void cleanup();
	void pause();
	void resume();
	void update();
	void render();

private:
	// -------------------------------------------------------------------------------
	// EDITOR VARIABLES
	// -------------------------------------------------------------------------------
	int camera_x, camera_y;
	int pan_start_x, pan_start_y;
	int pan_mouse_x, pan_mouse_y;
	int cur_tile_x, cur_tile_y;

	// -------------------------------------------------------------------------------
	// EDITOR FLAGS
	// -------------------------------------------------------------------------------
	ProgramState state;
	EditorState editor_state;
	EditMode edit_mode;
	int current_tile;
	bool pan_started;

	// -------------------------------------------------------------------------------
	// MAP SETTINGS
	// -------------------------------------------------------------------------------
	int tile_size;
	int start_x;
	int start_y;
	int num_entities;
	std::string tilemap_source;
	int map_width;
	int map_height;

	// -------------------------------------------------------------------------------
	// HELPER FUNCTIONS
	// -------------------------------------------------------------------------------
	void handleKeyPresses();
	void handleLeftMouseClick();
	void handleLeftMouseHeld();
	void handleRightMouseHeld();
	void resetMap();
	void loadMap(const std::string& path = DEFAULT_MAP_FILE);
	void saveMap(const std::string& path);

	std::vector<int> tilemap;
	std::vector<bool> collisionmap;

	TileMap * tiles;

	std::vector<FileMenuItem> files;

};