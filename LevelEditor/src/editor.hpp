#pragma once

#include "QcEngine.hpp"

#include <vector>
#include <fstream>
#include <experimental/filesystem>

#include <nlohmann\json.hpp>

namespace fs = std::experimental::filesystem::v1;
using json = nlohmann::json;

#include "util.hpp"

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
	bool show_HUD;

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
	// EDITOR HELPER FUNCTIONS
	// -------------------------------------------------------------------------------
	void renderEditor();
	void handleKeyPressEditor();
	void handleLeftMouseClickEditor();
	void handleLeftMouseHeldEditor();
	void handleRightMouseHeldEditor();

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