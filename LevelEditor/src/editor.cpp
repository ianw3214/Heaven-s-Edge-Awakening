#include "editor.hpp"

// static variable declaration
std::vector<Texture*> Editor::numbers;

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
	createFont("default_48", DEFAULT_FONT, 48);
	// initialize tilemap textures
	// TODO: load tilemap from specified file
	tiles = static_cast<TileMap*>(QcEngine::loadTexture(TILEMAP, DEFAULT_TILEMAP, T_TILEMAP));
	tiles->generateTiles(DEFAULT_TILE_SIZE, DEFAULT_TILE_SIZE);
	// load basic used editor textures
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
	QcEngine::loadTexture(PORTAL, PORTAL_IMG);
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
	QcEngine::loadTexture(POR_ENTITY_ICON, POR_ENTITY_ICON_IMG);
	QcEngine::loadTexture(POR_ENTITY_SEL, POR_ENTITY_SEL_IMG);
	// load MISC textures
	QcEngine::loadTexture(SAVEMAP, SAVEMAP_IMG);
	QcEngine::loadTexture(SPAWN_BLOCK, SPAWN_BLOCK_IMG);
	QcEngine::loadTexture(SPAWN_SELECT, SPAWN_SELECT_IMG);
	QcEngine::loadTexture(BLANK_MENU, BLANK_MENU_IMG);
	QcEngine::loadTexture(LARGE_MENU, LARGE_MENU_IMG);
	QcEngine::loadTexture(DIMENSION, DIMENSION_IMG);
	// initialize a default map
	loadMap();
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
		renderFile();
	}
	if (state == STATE_EDITOR) {
		renderEditor();
	}
	if (state == STATE_MENU) {
		renderMenu();
	}
}

void Editor::handleKeyPresses() {
	// use the escape key to return a state
	if (keyDown(SDL_SCANCODE_ESCAPE)) {
		if (state == STATE_MENU || state == STATE_FILE) {
			state = STATE_EDITOR;
		} else {
			exit();
		}
	}
	if (state == STATE_MENU) {
		handleKeyPressMenu();
	}
	/*
	// temporary key to put file loading
	if (keyDown(SDL_SCANCODE_R)) {
		enterFileState();
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
	if (keyDown(SDL_SCANCODE_TAB)) {
		state = STATE_MENU;
		menu_state = MENU_MAP_SETTINGS;
		new_width = map_width;
		new_height = map_height;
	}
}

void Editor::handleLeftMouseClick() {
	// the user is currently dragging the mouse around to pan the screen
	if (state == STATE_EDITOR) {
		handleLeftMouseClickEditor();
	}
	// TODO: generalize this so multiple things can ask for a file menu and have one be returned
	if (state == STATE_FILE) {
		handleLeftMouseClickFile();
	}
	// if the user clicks the box, focus in to handle text input
	if (state == STATE_MENU) {
		handleLeftMouseClickMenu();
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
	player_spawns.clear();
	portals.clear();
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
	resetMap();
	// open the map file to read
	std::fstream map_file;
	map_file.open(path, std::fstream::in);
	// load the map data
	if (map_file.is_open()) {
		try{
			json map_data;
			map_file >> map_data;
			tile_size = map_data["tile size"];
			for (const auto& v : map_data["start"]) {
				player_spawns.push_back(Vec2(v["x"], v["y"]));
			}
			for (const auto& v : map_data["portals"]) {
				PortalEntry temp;
				temp.x = v["x"];
				temp.y = v["y"];
				if (v.find("file") != v.end()) temp.file = v["file"].get<std::string>();
				else temp.file = DEFAULT_MAP_NAME;
				if (v.find("index") != v.end()) temp.num = v["index"];
				else temp.num = 0;
				portals.push_back(temp);
			}
			for (const auto& entity : map_data["entities"]) {
				if (entity["type"] == "enemy") {
					entities.push_back({ E_ENEMY, entity["x"], entity["y"] });
				}
			}
			tilemap_source = map_data["tilemap source"].get<std::string>();
			background_source = map_data["background source"].get<std::string>();
			map_width = map_data["map width"];
			map_height = map_data["map height"];
			tilemap = map_data["tiles"].get<std::vector<int>>();
			collisionmap = map_data["collision"].get<std::vector<bool>>();
			map_file.close();
		} catch(const std::exception& e) {
			std::cerr << e.what();
			map_file.close();
			// reset the map to clear out any data we might have already loaded
			resetMap();
			saveMap(path);
		}
	} else {
		map_file.close();
		// hanlde "LOAD FAILED" error
		resetMap();
		saveMap(path);
	}
	current_map = path;
	QcEngine::loadTexture("background", background_source);
}

void Editor::saveMap(const std::string & path) {

	std::fstream map_file;
	map_file.open(path, std::fstream::out);

	json map_data;
	map_data["tile size"] = tile_size;
	// input the player spawns into an array one by one
	map_data["start"] = json::array();
	for (const Vec2& v : player_spawns) {
		json vec = { {"x", v.x}, {"y", v.y} };
		map_data["start"].push_back(vec);
	}
	// input the portals into an array one by one
	map_data["portals"] = json::array();
	for (const PortalEntry& v : portals) {
		json vec = { {"x", v.x}, {"y", v.y }, {"file", v.file}, {"index", v.num} };
		map_data["portals"].push_back(vec);
	}
	// input the entities into an array one by one
	map_data["entities"] = json::array();
	for (const EntityEntry& e : entities) {
		json entity = { {"type", "enemy"}, {"x", e.x}, {"y", e.y} };
		map_data["entities"].push_back(entity);
	}
	map_data["tilemap source"] = tilemap_source;
	map_data["background source"] = background_source;
	map_data["map width"] = map_width;
	map_data["map height"] = map_height;
	map_data["tiles"] = tilemap;
	map_data["collision"] = collisionmap;
	
	map_file << map_data;
	// std::cout << map_data.dump(4) << std::endl;

	map_file.close();
}

// utility function to get a texture number
// only stores size 48 black font numbers
Texture * Editor::getNumberTexture(unsigned int num) {
	while (numbers.size() <= num) {
		SDL_Texture * num_tex = getTextTexture(std::to_string(numbers.size()), "default_48", {0, 0, 0});
		numbers.push_back(new Texture(num_tex));
	}
	return numbers[num];
}
