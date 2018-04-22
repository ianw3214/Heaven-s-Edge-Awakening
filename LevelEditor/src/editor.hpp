#pragma once

#include "QcEngine.hpp"

#include <vector>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem::v1;

#define DEFAULT_TILE_SIZE	64
#define DEFAULT_MAP_WIDTH	20
#define DEFAULT_MAP_HEIGHT	20
#define PAN_SPEED			300

#define BASE_DIR			"../assets"
#define DEFAULT_FONT		"../assets/fonts/Munro.ttf"

#define TILEMAP(x, y)		(y * DEFAULT_MAP_WIDTH + x)

enum EditorState {
	STATE_DEFAULT,
	STATE_PANNING,
	STATE_CHOOSE_FILE
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
	EditorState state;
	int current_tile;

	// -------------------------------------------------------------------------------
	// HELPER FUNCTIONS
	// -------------------------------------------------------------------------------
	void handleKeyPresses();

	std::vector<int> tilemap;
	std::vector<bool> collisionmap;

	TileMap * tiles;

	std::vector<FileMenuItem> files;

	// -------------------------------------------------------------------------------
	// TEXTURES
	// -------------------------------------------------------------------------------
	Texture * white_bar;
	Texture * tile_select;
};