#include "editor.hpp"

Editor::Editor() {

}

Editor::~Editor() {

}

void Editor::init() {
	// initialize editor state
	state = STATE_DEFAULT;
	current_tile = 0;
	createFont("default_16", DEFAULT_FONT, 16);
	// initialize tilemap textures
	tiles = new TileMap("../assets/tilemap.png");
	tiles->generateTiles(DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
	white_bar = new Texture("../assets/white_bar.png");
	tile_select = new Texture("../assets/tile_select.png");
	QcEngine::loadTexture("collision", "../assets/collision.png");
	// initialize a default map
	for (int i = 0; i < DEFAULT_MAP_HEIGHT; ++i) {
		for (int j = 0; j < DEFAULT_MAP_WIDTH; ++j) {
			if (i == 0 || i == DEFAULT_MAP_HEIGHT - 1 || j == 0 || j == DEFAULT_MAP_WIDTH - 1) {
				tilemap.push_back(0);
				collisionmap.push_back(true);
			}
			else {
				tilemap.push_back(-1);
				collisionmap.push_back(false);
			}
		}
	} 
	// initialize file things
	int cur_y = 10;
	for (auto& p : fs::directory_iterator(BASE_DIR)) {
		if (!fs::is_directory(p)) {
			std::string entry = p.path().string();
			Math::Rectangle rect = Math::Rectangle(10, cur_y, 256, 24);
			cur_y += 24;
			files.push_back({ entry, rect, new Texture(getTextTexture(entry, "default_16", {255, 255, 255})) });
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
	// update editor state
	cur_tile_x = (getMouseX() + camera_x) / DEFAULT_TILE_SIZE;
	cur_tile_y = (getMouseY() + camera_y) / DEFAULT_TILE_SIZE;
	handleKeyPresses();
	// the default click is to change the current tile
	if (leftMouseHeld() && state == STATE_DEFAULT) {
		int current = TILEMAP(cur_tile_x, cur_tile_y);
		tilemap[current] = current_tile;
	}
	// the user is currently dragging the mouse around to pan the screen
	if (leftMousePressed() && state == STATE_PANNING) {
		pan_mouse_x = getMouseX();
		pan_mouse_y = getMouseY();
		pan_start_x = camera_x;
		pan_start_y = camera_y;
	}
	// the user clicked to start panning the camera around
	if (leftMouseHeld() && state == STATE_PANNING) {
		camera_x = pan_start_x - getMouseX() + pan_mouse_x;
		camera_y = pan_start_y - getMouseY() + pan_mouse_y;
	}
	// the user clicked to edit collisions
	if (leftMouseHeld() && state == STATE_EDIT_COLLISION) {
		int current = TILEMAP(cur_tile_x, cur_tile_y);
		collisionmap[current] = true;
	}
	if (rightMouseHeld() && state == STATE_EDIT_COLLISION) {
		int current = TILEMAP(cur_tile_x, cur_tile_y);
		collisionmap[current] = false;
	}
	// the user just clicked on a file to load
	if (leftMousePressed() && state == STATE_CHOOSE_FILE) {
		for (unsigned int i = 0; i < files.size(); ++i) {
			if (getMouseX() > files[i].collision.pos.x && getMouseX() < files[i].collision.pos.x + files[i].collision.w &&
				getMouseY() > files[i].collision.pos.y && getMouseY() < files[i].collision.pos.y + files[i].collision.h) 
			{
				TileMap * old = tiles;
				tiles = new TileMap(files[i].name);
				delete old;
				state = STATE_DEFAULT;
				// TODO: fix metadata loading when opening file
				tiles->generateTiles(DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
				break;
			}
		}
	}
}

void Editor::render() {
	for (int i = 0; i < DEFAULT_MAP_HEIGHT; ++i) {
		for (int j = 0; j < DEFAULT_MAP_WIDTH; ++j) {
			if (tilemap[TILEMAP(j, i)] >= 0) {
				tiles->render(j * DEFAULT_TILE_SIZE - camera_x, i * DEFAULT_TILE_SIZE - camera_y, tilemap[TILEMAP(j, i)]);
			}
		}
	}
	if (state == STATE_EDIT_COLLISION) {
		for (int i = 0; i < DEFAULT_MAP_HEIGHT; ++i) {
			for (int j = 0; j < DEFAULT_MAP_WIDTH; ++j) {
				if (collisionmap[TILEMAP(j, i)] == true) {
					QcEngine::getTexture("collision")->render(j * DEFAULT_TILE_SIZE - camera_x, i * DEFAULT_TILE_SIZE - camera_y);
				}
			}
		}
	}
	if (state == STATE_CHOOSE_FILE) {
		for (unsigned int i = 0; i < files.size(); ++i) {
			// if the mouse is hovering over current item, render white overlay
			if (getMouseX() > files[i].collision.pos.x && getMouseX() < files[i].collision.pos.x + files[i].collision.w &&
				getMouseY() > files[i].collision.pos.y && getMouseY() < files[i].collision.pos.y + files[i].collision.h) {
				white_bar->render(10, 10 + 24 * i);
			}
			files[i].tex->render(10, 10 + 24 * i);
		}
	}
	// render the tile selection image
	if (state == STATE_DEFAULT || state == STATE_EDIT_COLLISION) {
		tile_select->render(cur_tile_x * DEFAULT_TILE_SIZE - camera_x, cur_tile_y * DEFAULT_TILE_SIZE - camera_y);
	}
}

void Editor::handleKeyPresses() {
	if (keyPressed(SDL_SCANCODE_ESCAPE)) {
		exit();
	}
	if (keyPressed(SDL_SCANCODE_SPACE)) {
		if (state != STATE_CHOOSE_FILE) {
			state = STATE_PANNING;
		}
	} else {
		if (state == STATE_PANNING) state = STATE_DEFAULT;
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
	if (keyDown(SDL_SCANCODE_Q)) {
		state = STATE_CHOOSE_FILE;
	}
	if (keyDown(SDL_SCANCODE_W)) {
		state = STATE_EDIT_COLLISION;
	}
	if (keyDown(SDL_SCANCODE_D)) {
		current_tile += current_tile >= tiles->getNumTiles() - 1 ? 0 : 1;
	}
	if (keyDown(SDL_SCANCODE_A)) {
		current_tile -= current_tile <= 0 ? 0 : 1;
	}
}
