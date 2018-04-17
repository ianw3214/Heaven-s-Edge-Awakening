#pragma once

#include "QcEngine.hpp"

#include <vector>

#define DEFAULT_TILE_SIZE	64
#define DEFAULT_MAP_WIDTH	20
#define DEFAULT_MAP_HEIGHT	20
#define PAN_SPEED			300

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
	Texture * tile_texture;

	// -------------------------------------------------------------------------------
	// EDITOR VARIABLES
	// -------------------------------------------------------------------------------
	int camera_x, camera_y;
	int pan_start_x, pan_start_y;
	int pan_mouse_x, pan_mouse_y;

	// -------------------------------------------------------------------------------
	// EDITOR FLAGS
	// -------------------------------------------------------------------------------
	bool panning;

	// -------------------------------------------------------------------------------
	// HELPER FUNCTIONS
	// -------------------------------------------------------------------------------
	void handleKeyPresses();

	std::vector<int> tilemap;
	std::vector<bool> collisionmap;
};