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
	e_edit_mode = E_EDIT_PLAYER;
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
	// load entity textures
	QcEngine::loadTexture(PLAYER, PLAYER_IMG);
	QcEngine::loadTexture(ENEMY, ENEMY_IMG);
	// load HUG textures
	QcEngine::loadTexture(ICON_TILE, ICON_TILE_IMG);
	QcEngine::loadTexture(SELECT_TILE, SELECT_TILE_IMG);
	QcEngine::loadTexture(ICON_COLLISION, ICON_COLLISION_IMG);
	QcEngine::loadTexture(SELECT_COLLISION, SELECT_COLL_IMG);
	QcEngine::loadTexture(ICON_ENTITY, ICON_ENTITY_IMG);
	QcEngine::loadTexture(SELECT_ENTITY, SELECT_ENTITY_IMG);
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
	// render the files if we are in that state
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
		renderEditor();
	}
	// render HUD
	QcEngine::getTexture(ICON_TILE)->render(0, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
	QcEngine::getTexture(ICON_COLLISION)->render(64, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
	QcEngine::getTexture(ICON_ENTITY)->render(128, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
	if (edit_mode == EDIT_TILE) {
		QcEngine::getTexture(SELECT_TILE)->render(0, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
	}
	if (edit_mode == EDIT_COLLISION) {
		QcEngine::getTexture(SELECT_COLLISION)->render(64, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
	}
	if (edit_mode == EDIT_ENTITIES) {
		QcEngine::getTexture(SELECT_ENTITY)->render(128, QcEngine::getCVARint("WINDOW HEIGHT") - 64);
	}
}

void Editor::handleKeyPresses() {
	if (keyPressed(SDL_SCANCODE_ESCAPE)) {
		exit();
	}
	// temporary key to put file loading
	if (keyDown(SDL_SCANCODE_R)) {
		state = STATE_FILE;
	}
	// handle editor key presses
	if (state == STATE_EDITOR) {
		handleKeyPressEditor();
	}
}

void Editor::handleLeftMouseClick() {
	// the user is currently dragging the mouse around to pan the screen
	if (state == STATE_EDITOR) {
		handleLeftMouseClickEditor();
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
		handleLeftMouseHeldEditor();
	}
}

void Editor::handleRightMouseHeld() {
	if (state == STATE_EDITOR) {
		handleRightMouseHeldEditor();
	}
}

void Editor::resetMap() {
	tile_size = DEFAULT_TILE_SIZE;
	start_x = 0;
	start_y = 0;
	num_entities = 0;
	entities.clear();
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
		for (int i = 0; i < num_entities; ++i) {
			char type;
			map_file >> type;
			// ENEMY TYPE
			if (type == 'E') {
				int x;
				int y;
				map_file >> x;
				map_file >> y;
				entities.push_back({E_ENEMY, x, y});
			}
		}
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
	for (const EntityEntry& e : entities) {
		if (e.type == E_ENEMY) {
			map_file << "E ";
			map_file << e.x << ' ';
			map_file << e.y << '\n';
		}
	}
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
