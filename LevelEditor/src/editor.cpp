#include "editor.hpp"

Editor::Editor() {

}

Editor::~Editor() {

}

void Editor::init() {
	// initialize textures
	tile_texture = new Texture("../assets/tile.png");
	// initialize a default map
	for (int i = 0; i < DEFAULT_MAP_HEIGHT; ++i) {
		for (int j = 0; j < DEFAULT_MAP_WIDTH; ++j) {
			if (i == 0 || i == DEFAULT_MAP_HEIGHT - 1 || j == 0 || j == DEFAULT_MAP_WIDTH - 1) {
				tilemap.push_back(1);
			}
			else {
				tilemap.push_back(0);
			}
		}
	}
}

void Editor::cleanup() {

}

void Editor::pause() {

}

void Editor::resume() {
	
}

void Editor::update() {
	handleKeyPresses();
	if (getMousePressed() && panning) {
		pan_mouse_x = getMouseX();
		pan_mouse_y = getMouseY();
		pan_start_x = camera_x;
		pan_start_y = camera_y;
	}
	if (getMouseDown() && panning) {
		camera_x = pan_start_x - getMouseX() + pan_mouse_x;
		camera_y = pan_start_y - getMouseY() + pan_mouse_y;
	}
}

void Editor::render() {
	for (int i = 0; i < DEFAULT_MAP_HEIGHT; ++i) {
		for (int j = 0; j < DEFAULT_MAP_WIDTH; ++j) {
			if (tilemap[i * DEFAULT_MAP_WIDTH + j] == 1) {
				tile_texture->render(j * DEFAULT_TILE_SIZE - camera_x, i * DEFAULT_TILE_SIZE - camera_y);
			}
		}
	}
}

void Editor::handleKeyPresses() {
	if (keyPressed(SDL_SCANCODE_ESCAPE)) {
		exit();
	}
	if (keyPressed(SDL_SCANCODE_SPACE)) {
		panning = true;
	} else {
		panning = false;
	}
	if (keyPressed(SDL_SCANCODE_UP)) {
		camera_y -= static_cast<int>(delta * PAN_SPEED / 1000.f);
	}
	if (keyPressed(SDL_SCANCODE_DOWN)) {
		camera_y += static_cast<int>(delta * PAN_SPEED / 1000.f);
	}
	if (keyPressed(SDL_SCANCODE_RIGHT)) {
		camera_x += static_cast<int>(delta * PAN_SPEED / 1000.f);
	}
	if (keyPressed(SDL_SCANCODE_LEFT)) {
		camera_x -= static_cast<int>(delta * PAN_SPEED / 1000.f);
	}
}
