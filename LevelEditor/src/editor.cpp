#include "editor.hpp"

Editor::Editor() {

}

Editor::~Editor() {
	
}

void Editor::init() {
	// initialize editor state
	state = STATE_EDITOR;
	editor_state = EDITOR_EDIT;
	edit_mode = EDIT_TILE;
	current_tile = 0;
	// intiialize editor variables
	camera_x = 0, camera_y = 0;
	palette_x = DEFAULT_PALETTE_X, palette_y = DEFAULT_PALETTE_Y;
	// setup a font
	createFont("default_16", DEFAULT_FONT, 16);
	// TODO: load tilemap from specified file
	// initialize tilemap textures
	tiles = static_cast<TileMap*>(QcEngine::loadTexture(TILEMAP, DEFAULT_TILEMAP, T_TILEMAP));
	tiles->generateTiles(DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
	// load commonly used editor textures
	QcEngine::loadTexture(TILE_SELECT, TILE_SELECT_IMG);
	QcEngine::loadTexture(COLLISION, COLLISION_IMG);
	QcEngine::loadTexture(WHITE_BAR, WHITE_BAR_IMG);
	QcEngine::loadTexture(BORDER_HOR, BORDER_HOR_IMG);
	QcEngine::loadTexture(BORDER_VER, BORDER_VER_IMG);
	QcEngine::loadTexture(BORDER_CORNER, BORDER_CORNER_IMG);
	// load palette textures
	QcEngine::loadTexture(PALETTE_BASE, PALETTE_BASE_IMG);
	QcEngine::loadTexture(PALETTE_SELECT, PALETTE_SELECT_IMG);
	// initialize a default map
	loadMap();
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
	if (getMouseX() + camera_x < 0) cur_tile_x--;
	cur_tile_y = (getMouseY() + camera_y) / DEFAULT_TILE_SIZE;
	if (getMouseY() + camera_y < 0) cur_tile_y--;
	handleKeyPresses();
	// handle mouse clicks/holds/releases
	if (leftMousePressed()) handleLeftMouseClick();
	if (leftMouseHeld()) handleLeftMouseHeld();
	if (rightMouseHeld()) handleRightMouseHeld();
}

void Editor::render() {
	// render the background
	QcEngine::getTexture("background")->render(-camera_x / 2, -camera_y / 2);
	// render the tile map
	for (int i = 0; i < map_height; ++i) {
		for (int j = 0; j < map_width; ++j) {
			if (tilemap[tileIndex(j, i)] >= 0) {
				tiles->render(j * tile_size - camera_x, i * tile_size - camera_y, tilemap[tileIndex(j, i)]);
			}
		}
	}
	// render the map borders
	for (int i = 0; i < map_width; ++i) {
		QcEngine::getTexture(BORDER_HOR)->render(i * tile_size - camera_x, - 6 - camera_y);
		QcEngine::getTexture(BORDER_HOR)->render(i * tile_size - camera_x, map_height * tile_size - camera_y);
	}
	for (int i = 0; i < map_height; ++i) {
		QcEngine::getTexture(BORDER_VER)->render(-6 - camera_x, i * tile_size - camera_y);
		QcEngine::getTexture(BORDER_VER)->render(map_width * tile_size - camera_x, i * tile_size - camera_y);
	}
	QcEngine::getTexture(BORDER_CORNER)->render(-6 - camera_x, -6 - camera_y);
	QcEngine::getTexture(BORDER_CORNER)->render(map_width * tile_size - camera_x, -6 - camera_y);
	QcEngine::getTexture(BORDER_CORNER)->render(map_width * tile_size - camera_x, map_height * tile_size - camera_y);
	QcEngine::getTexture(BORDER_CORNER)->render(-6 - camera_x, map_height * tile_size - camera_y);
	// render the collision boxes if user is currently editing collisions
	if (edit_mode == EDIT_COLLISION) {
		for (int i = 0; i < map_height ; ++i) {
			for (int j = 0; j < map_width; ++j) {
				if (collisionmap[tileIndex(j, i)] == true) {
					QcEngine::getTexture(COLLISION)->render(j * tile_size - camera_x, i * tile_size- camera_y);
				}
			}
		}
	}
	if (state == STATE_FILE) {
		for (unsigned int i = 0; i < files.size(); ++i) {
			// if the mouse is hovering over current item, render white overlay
			if (getMouseX() > files[i].collision.pos.x && getMouseX() < files[i].collision.pos.x + files[i].collision.w &&
				getMouseY() > files[i].collision.pos.y && getMouseY() < files[i].collision.pos.y + files[i].collision.h) {
				QcEngine::getTexture(WHITE_BAR)->render(10, 10 + 24 * i);
			}
			files[i].tex->render(10, 10 + 24 * i);
		}
	}
	if (state == STATE_EDITOR) {
		// render the tile selection image
		QcEngine::getTexture(TILE_SELECT)->render(cur_tile_x * tile_size - camera_x, cur_tile_y * tile_size - camera_y);
		if (edit_mode == EDIT_TILE) {
			// render the tile palette
			QcEngine::getTexture(PALETTE_BASE)->render(palette_x, palette_y);
			for (int i = 0; i < tiles->getNumTiles(); ++i) {
				int x = (i % 3) * tile_size + 6 + palette_x;
				int y = (i / 3) * tile_size + 6 + palette_y;
				tiles->render(x, y, i);
				if (i == current_tile) {
					QcEngine::getTexture(PALETTE_SELECT)->render(x, y);
				}
			}
		}
	}
}

void Editor::handleKeyPresses() {
	if (keyPressed(SDL_SCANCODE_ESCAPE)) {
		exit();
	}
	if (keyPressed(SDL_SCANCODE_SPACE)) {
		editor_state = EDITOR_PANNING;
	} else {
		if (editor_state == EDITOR_PICK) editor_state = EDITOR_PICK; 
		else editor_state = EDITOR_EDIT;
		pan_started = false;
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
	// QWERTY for common controls
	if (keyDown(SDL_SCANCODE_Q)) {
		state = STATE_EDITOR;
		edit_mode = EDIT_TILE;
	}
	if (keyDown(SDL_SCANCODE_W)) {
		state = STATE_EDITOR;
		edit_mode = EDIT_COLLISION;
	}
	// temporary key to put file loading
	if (keyDown(SDL_SCANCODE_E)) {
		state = STATE_FILE;
	}
	if (keyDown(SDL_SCANCODE_S)) {
		// TODO: move this somewhere else
		saveMap(DEFAULT_MAP_FILE);
	}
	// RESET THE MAP
	if (keyDown(SDL_SCANCODE_DELETE)) {
		resetMap();
	}
	if (keyPressed(SDL_SCANCODE_LALT)) {
		if (editor_state != EDITOR_PANNING)editor_state = EDITOR_PICK;
	} else {
		if (editor_state != EDITOR_PANNING) editor_state = EDITOR_EDIT;
	}
	// using numbers to select a tile
	if (keyDown(SDL_SCANCODE_1)) {
		if (tiles->getNumTiles() >= 1) {
			current_tile = 0;
		}
	}
	if (keyDown(SDL_SCANCODE_2)) {
		if (tiles->getNumTiles() >= 2) {
			current_tile = 1;
		}
	}
	if (keyDown(SDL_SCANCODE_3)) {
		if (tiles->getNumTiles() >= 3) {
			current_tile = 2;
		}
	}
	if (keyDown(SDL_SCANCODE_4)) {
		if (tiles->getNumTiles() >= 4) {
			current_tile = 3;
		}
	}
	if (keyDown(SDL_SCANCODE_5)) {
		if (tiles->getNumTiles() >= 5) {
			current_tile = 4;
		}
	}
	if (keyDown(SDL_SCANCODE_6)) {
		if (tiles->getNumTiles() >= 6) {
			current_tile = 5;
		}
	}
	if (keyDown(SDL_SCANCODE_7)) {
		if (tiles->getNumTiles() >= 7) {
			current_tile = 6;
		}
	}
	if (keyDown(SDL_SCANCODE_8)) {
		if (tiles->getNumTiles() >= 8) {
			current_tile = 7;
		}
	}
	if (keyDown(SDL_SCANCODE_9)) {
		if (tiles->getNumTiles() >= 9) {
			current_tile = 8;
		}
	}
}

void Editor::handleLeftMouseClick() {
	// the user is currently dragging the mouse around to pan the screen
	if (state == STATE_EDITOR) {
		if (editor_state == EDITOR_PANNING) {
			pan_started = true;
			pan_mouse_x = getMouseX();
			pan_mouse_y = getMouseY();
			pan_start_x = camera_x;
			pan_start_y = camera_y;
		}
	}
	// TODO: generalize this so multiple things can ask for a file menu and have one be returned
	if (state == STATE_FILE) {
		for (unsigned int i = 0; i < files.size(); ++i) {
			if (getMouseX() > files[i].collision.pos.x && getMouseX() < files[i].collision.pos.x + files[i].collision.w &&
				getMouseY() > files[i].collision.pos.y && getMouseY() < files[i].collision.pos.y + files[i].collision.h) {
				TileMap * old = tiles;
				tiles = new TileMap(files[i].name);
				delete old;
				state = STATE_EDITOR;
				// TODO: fix metadata loading when opening file
				tiles->generateTiles(DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
				break;
			}
		}
	}
}

void Editor::handleLeftMouseHeld() {
	if (state == STATE_EDITOR) {
		// the user clicked to start panning the camera around
		if (editor_state == EDITOR_PANNING) {
			if (!pan_started) {
				pan_started = true;
				pan_mouse_x = getMouseX();
				pan_mouse_y = getMouseY();
				pan_start_x = camera_x;
				pan_start_y = camera_y;
			}
			camera_x = pan_start_x - getMouseX() + pan_mouse_x;
			camera_y = pan_start_y - getMouseY() + pan_mouse_y;
		}
		if (editor_state == EDITOR_PICK) {
			int cur = cur_tile_y * map_width + cur_tile_x;
			if (edit_mode == EDIT_TILE) {
				current_tile = tilemap[cur];
			}
			if (edit_mode == EDIT_COLLISION) {
				// do nothing I guess :o
			}
		}
		// the default click is to change the current tile
		if (editor_state == EDITOR_EDIT) {
			if (edit_mode == EDIT_TILE) {
				// the user is clicking to switch palette tiles 
				// TODO: remove magic numbers here
				if (Math::isColliding(Vec2(getMouseX(), getMouseY()), Math::Rectangle(palette_x + 6, palette_y + 6, 196, 196))) {
					int tile_x = (getMouseX() - palette_x - 6) / 64;
					int tile_y = (getMouseY() - palette_y - 6) / 64;
					int new_tile = tile_y * 3 + tile_x;
					if (new_tile < tiles->getNumTiles()) {
						current_tile = new_tile;
					}
				} else {
					// the user is clicking to edit a tile
					int current = tileIndex(cur_tile_x, cur_tile_y);
					if (current < 0 || current > map_width * map_height - 1) return;
					tilemap[current] = current_tile;
				}
			}
			if (edit_mode == EDIT_COLLISION) {
				// the user is clicking to edit a collision tile
				int current = tileIndex(cur_tile_x, cur_tile_y);
				if (current < 0 || current > map_width * map_height - 1) return;
				collisionmap[current] = true;
			}
		}
	}
}

void Editor::handleRightMouseHeld() {
	if (state == STATE_EDITOR) {
		if (editor_state == EDITOR_EDIT) {
			int current = tileIndex(cur_tile_x, cur_tile_y);
			if (current < 0 || current > map_width * map_height - 1) return;
			if (edit_mode == EDIT_TILE) {
				tilemap[current] = -1;
			}
			if (edit_mode == EDIT_COLLISION) {
				collisionmap[current] = false;
			}
		}
	}
}

void Editor::resetMap() {
	tile_size = DEFAULT_TILE_SIZE;
	start_x = 0;
	start_y = 0;
	num_entities = 0;
	tilemap_source = DEFAULT_TILEMAP;
	background_source = DEFAULT_BACKGROUND;
	map_width = DEFAULT_MAP_WIDTH;
	map_height = DEFAULT_MAP_HEIGHT;
	tilemap.clear();
	collisionmap.clear();
	for (int i = 0; i < map_width * map_height; ++i) {
		tilemap.push_back(-1);
	}
	for (int i = 0; i < map_width * map_height; ++i) {
		collisionmap.push_back(false);
	}
}

void Editor::loadMap(const std::string & path) {
	// clear previous tile data before loading in new data
	tilemap.clear();
	collisionmap.clear();
	// open the map file to read
	std::fstream map_file;
	map_file.open(path, std::fstream::in);
	if (map_file.is_open()) {
		// check a version number
		int version;
		map_file >> version;
		if (version != 1) {
			resetMap();
			saveMap(path);
		}
		map_file >> tile_size;
		map_file >> start_x;
		map_file >> start_y;
		map_file >> num_entities;
		// TODO: Add entity data here somehow
		map_file >> tilemap_source;
		map_file >> background_source;
		map_file >> map_width;
		map_file >> map_height;
		int tile;
		for (int i = 0; i < map_width * map_height; ++i) {
			map_file >> tile;
			tilemap.push_back(tile);
		}
		for (int i = 0; i < map_width * map_height; ++i) {
			map_file >> tile;
			collisionmap.push_back(tile);
		}
		map_file.close();
	} else {
		map_file.close();
		// hanlde "LOAD FAILED" error
		resetMap();
		saveMap(path);
	}
	QcEngine::loadTexture("background", background_source);
}

void Editor::saveMap(const std::string & path) {
	std::fstream map_file;
	// create a new file to work with
	map_file.open(path, std::fstream::out);
	map_file << 1 << '\n';			// VERSION NUMBER
	map_file << tile_size << '\n';
	map_file << start_x << '\n';
	map_file << start_y << '\n';
	map_file << num_entities << '\n';
	map_file << tilemap_source << '\n';
	map_file << background_source << '\n';
	map_file << map_width << '\n';
	map_file << map_height << '\n';
	for (int i = 0; i < map_height; ++i) {
		for (int j = 0; j < map_width; ++j) {
			map_file << tilemap[tileIndex(j, i)] << ' ';
		}
		map_file << '\n';
	}
	for (int i = 0; i < map_height; ++i) {
		for (int j = 0; j < map_width; ++j) {
			map_file << collisionmap[tileIndex(j, i)] << ' ';
		}
		map_file << '\n';
	}
	map_file.close();
}
