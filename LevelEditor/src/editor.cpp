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
	show_HUD = true;
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
	// load HUD textures
	QcEngine::loadTexture(ICON_TILE, ICON_TILE_IMG);
	QcEngine::loadTexture(SELECT_TILE, SELECT_TILE_IMG);
	QcEngine::loadTexture(ICON_COLLISION, ICON_COLLISION_IMG);
	QcEngine::loadTexture(SELECT_COLLISION, SELECT_COLL_IMG);
	QcEngine::loadTexture(ICON_ENTITY, ICON_ENTITY_IMG);
	QcEngine::loadTexture(SELECT_ENTITY, SELECT_ENTITY_IMG);
	QcEngine::loadTexture(PLA_ENTITY_ICON, PLA_ENTITY_ICON_IMG);
	QcEngine::loadTexture(PLA_ENTITY_SEL, PLA_ENTITY_SEL_IMG);
	QcEngine::loadTexture(ENE_ENTITY_ICON, ENE_ENTITY_ICON_IMG);
	QcEngine::loadTexture(ENE_ENTITY_SEL, ENE_ENTITY_SEL_IMG);
	// load MISC textures
	QcEngine::loadTexture(SAVEMAP, SAVEMAP_IMG);
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
	if (state == STATE_SAVEMAP) {
		int x = (QcEngine::getCVARint("WINDOW WIDTH") - QcEngine::getTexture(SAVEMAP)->getWidth()) / 2;
		int y = (QcEngine::getCVARint("WINDOW HEIGHT") - QcEngine::getTexture(SAVEMAP)->getHeight()) / 2;
		QcEngine::getTexture(SAVEMAP)->render(x, y);
		// render text if there is any
		const std::string& ref = managerRef->getTextInput();
		if (ref.size() > 0) {
			SDL_Texture * text_texture = getTextTexture(ref, "default_16", { 0, 0, 0 });
			Texture * tex = new Texture(text_texture);
			tex->render(530, 350);
		}
	}
}

void Editor::handleKeyPresses() {
	// use the escape key to return a state
	if (keyDown(SDL_SCANCODE_ESCAPE)) {
		if (state == STATE_SAVEMAP) {
			state = STATE_EDITOR;
		} else {
			exit();
		}
	}
	if (keyDown(SDL_SCANCODE_RETURN)) {
		if (state == STATE_SAVEMAP) {
			const std::string& ref = managerRef->getTextInput();
			if (ref.size() > 0) {
				std::string dest = DEFAULT_MAP_FOLDER + ref + ".txt";
				saveMap(dest);
			} else {
				saveMap(DEFAULT_MAP_FILE);
			}
			state = STATE_EDITOR;
			managerRef->stopTextInput();
		}
	}
	/*
	// temporary key to put file loading
	if (keyDown(SDL_SCANCODE_R)) {
		state = STATE_FILE;
	}
	*/
	// handle editor key presses
	if (state == STATE_EDITOR) {
		handleKeyPressEditor();
	}
	// toggle the HUD upon pressing the specified key
	if (keyDown(SDL_SCANCODE_H)) {
		show_HUD = !show_HUD;
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
	// if the user clicks the box, focus in to handle text input
	if (state == STATE_SAVEMAP) {
		Vec2 m_pos = Vec2(getMouseX(), getMouseY());
		int x = (QcEngine::getCVARint("WINDOW WIDTH") - QcEngine::getTexture(SAVEMAP)->getWidth()) / 2;
		int y = (QcEngine::getCVARint("WINDOW HEIGHT") - QcEngine::getTexture(SAVEMAP)->getHeight()) / 2;
		if (Math::isColliding(m_pos, Math::Rectangle(x, y, 256, 128))) {
			managerRef->startTextInput();
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
	// load the map data
	if (map_file.is_open()) {
		json map_data;
		map_file >> map_data;
		tile_size = map_data["tile size"];
		start_x = map_data["start x"];
		start_y = map_data["start y"];
		num_entities = map_data["entity count"];
		for (const auto& entity : map_data["entities"]) {
			if (entity["type"] == "E") {
				entities.push_back({ E_ENEMY, entity["x"], entity["y"] });
			}
		}
		tilemap_source = map_data["tilemap source"].get<std::string>();
		background_source = map_data["background source"].get<std::string>();
		map_width = map_data["map width"];
		map_height = map_data["map height"];
		tilemap = map_data["tiles"].get<std::vector<int>>();
		collisionmap = map_data["collision"].get<std::vector<bool>>();
		std::cout << map_data.dump(4) << std::endl;
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
	map_file.open(path, std::fstream::out);

	json map_data;
	map_data["tile size"] = tile_size;
	map_data["start x"] = start_x;
	map_data["start y"] = start_y;
	map_data["entity count"] = num_entities;
	map_data["entities"] = json::array();
	for (const EntityEntry& e : entities) {
		map_data["entities"].push_back(json::object({ { "type", "enemy" }, { "x", e.x }, { "y", e.y } }));
	}
	map_data["tilemap source"] = tilemap_source;
	map_data["background source"] = background_source;
	map_data["map width"] = map_width;
	map_data["map height"] = map_height;
	map_data["tiles"] = tilemap;
	map_data["collision"] = collisionmap;

	map_file << map_data;

	map_file.close();
}
