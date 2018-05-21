#pragma once

#include "QcEngine.hpp"

#include <vector>
#include <fstream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem::v1;

#define DEFAULT_TILE_SIZE	64
#define DEFAULT_MAP_WIDTH	40
#define DEFAULT_MAP_HEIGHT	40
#define PAN_SPEED			300

#define BASE_DIR			"../assets"
#define DEFAULT_FONT		"../assets/fonts/Munro.ttf"

#define DEFAULT_TILEMAP		"../assets/tilemap.png"
#define DEFAULT_BACKGROUND	"../assets/parallax_test.png"

#define DEFAULT_MAP_FILE	"../assets/maps/default.txt"

#define DEFAULT_PALETTE_X	0
#define DEFAULT_PALETTE_Y	0

#define TILEMAP				"tilemap"
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

#define PALETTE_BASE		"palette_base"
#define PALETTE_BASE_IMG	"../assets/editor/palette_base.png"
#define PALETTE_SELECT		"palette_select"
#define PALETTE_SELECT_IMG	"../assets/editor/palette_select.png"

// ENTITY SPRITE DEFINITIONS
#define PLAYER				"player"
#define PLAYER_IMG			"../assets/editor/player.png"
#define ENEMY				"enemy"
#define ENEMY_IMG			"../assets/editor/enemy.png"

// HUD SPRITE DEFINITIONS
#define ICON_TILE			"edit_icon"
#define ICON_TILE_IMG		"../assets/editor/tile_icon_normal.png"
#define SELECT_TILE			"tile_select_icon"
#define SELECT_TILE_IMG		"../assets/editor/tile_icon_select.png"
#define ICON_COLLISION		"collision_icon"
#define ICON_COLLISION_IMG	"../assets/editor/collision_icon_normal.png"
#define SELECT_COLLISION	"collision_select_icon"
#define SELECT_COLL_IMG		"../assets/editor/collision_icon_select.png"
#define ICON_ENTITY			"entity_icon"
#define ICON_ENTITY_IMG		"../assets/editor/entity_icon_normal.png"
#define SELECT_ENTITY		"entity_select_icon"
#define SELECT_ENTITY_IMG	"../assets/editor/entity_icon_select.png"

enum ProgramState {
	STATE_EDITOR,
	STATE_FILE,
	STATE_SETTINGS
};

enum EditorState {
	EDITOR_EDIT,
	EDITOR_PANNING,
	EDITOR_PICK
};

enum EditMode {
	EDIT_TILE,
	EDIT_COLLISION,
	EDIT_ENTITIES
};

enum EntityEditMode {
	E_EDIT_PLAYER,
	E_EDIT_ENEMY
};

enum EntityType {
	E_ENEMY
};

struct EntityEntry {
	EntityType type;
	int x;
	int y;
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
	int palette_x, palette_y;

	// -------------------------------------------------------------------------------
	// EDITOR FLAGS
	// -------------------------------------------------------------------------------
	ProgramState state;
	EditorState editor_state;
	EditMode edit_mode;
	EntityEditMode e_edit_mode;
	int current_tile;
	bool pan_started;

	// -------------------------------------------------------------------------------
	// MAP SETTINGS
	// -------------------------------------------------------------------------------
	int tile_size;
	int start_x;
	int start_y;
	int num_entities;
	std::vector<EntityEntry> entities;
	std::string tilemap_source;
	std::string background_source;
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

	// -------------------------------------------------------------------------------
	// MISC. UTILITY FUNCTIONS
	// -------------------------------------------------------------------------------
	inline int tileIndex(int x, int y) const {
		return y * map_width + x;
	}

	std::vector<int> tilemap;
	std::vector<bool> collisionmap;

	// the tilemap is just a pointer, no ownership
	TileMap * tiles;

	std::vector<FileMenuItem> files;

};