#include "game.hpp"

#include "entities\enemy.hpp"

Game::Game() {
	current_map = DEFAULT_MAP_FILE;
	start_index = 0;
}

Game::Game(const std::string& path) {
	current_map = path;
	start_index = 0;
}

Game::Game(const std::string& path, int start_index) {
	this->current_map = path;
	this->start_index = start_index;
}

Game::~Game() {
	
}

void Game::init() {
	// setup the game data struct
	data = new GameData{};
	GameObject::setGameData(data);
	data->cam_x = 0;
	data->cam_y = 0;
	data->change_state = false;
	// initialize textures
	tiles = nullptr;
	QcEngine::loadTexture(ARROW, ARROW_IMG);
	AnimatedTexture * arrow_particle_texture = static_cast<AnimatedTexture*>(QcEngine::loadTexture(ARROW_P, ARROW_P_IMG, T_ANIMATED));
	arrow_particle_texture->generateAtlas(16, 16);
	arrow_particle_texture->addAnimationState(0, 2);
	QcEngine::loadTexture(PORTAL, PORTAL_IMG);
	AnimatedTexture * t = static_cast<AnimatedTexture*>(QcEngine::loadTexture("enemy", "../assets/enemy.png", T_ANIMATED));
	t->generateAtlas(64, 128);
	t->addAnimationState({ 0, 3 });
	createFont("default_24", DEFAULT_FONT, 24);
	// load the map data from a specified path
	loadMap(current_map, true);
	// adjust the camera to be centered on the player
	// TODO: get rid of the magic numbers
	data->cam_x = player->getX() - 1280 / 2;
	data->cam_y = player->getY() - 720 / 2;

	dialogue.push("test1");
	dialogue.push("test2");
	dialogue.push("test3");
	dialogue.push("test4");
	dialogue.push("test5");
}

void Game::cleanup() {
}

void Game::pause() {
}

void Game::resume() {
}

void Game::update() {
	handleKeyPresses();
	for (GameObject* obj : data->entities) {
		obj->update(delta);
	}
	// add entities that should be added to the vector
	while (!data->new_entities.empty()) {
		data->entities.push_back(data->new_entities.front());
		data->new_entities.pop();
	}
	// remove entities that should be removed
	for (int i = data->entities.size() - 1; i >= 0; --i) {
		if (data->entities[i]->shouldRemove()) {
			data->entities.erase(data->entities.begin() + i);
		}
	}
	// call any other game update functions there are
	updateCamera();
	// update the dialogue text
	if (current_text.size() == 0 && !dialogue.empty()) {
		current_text = dialogue.front();
		dialogue.pop();
	}
	// -------------------------------------------------------------------------------
	// DEBUG CODE
	// -------------------------------------------------------------------------------
	if (data->change_state) {
		changeState(std::make_unique<Game>(MAP_FOLDER + data->next_map, data->next_index));
	}
}

void Game::render() {
	// TOOD: figure out better way to store backgrounds
	// render parallax background
	QcEngine::getTexture("bg")->render(-data->cam_x / 2, -data->cam_y / 2);
	int x;
	int y;
	for (int i = 0; i < data->map_height; ++i) {
		for (int j = 0; j < data->map_width; ++j) {
			x = j * data->tile_size - data->cam_x;
			y = i * data->tile_size - data->cam_y;
			if (x > -64 && y > -64 && x < 1280 + 64 && y < 728 + 64) {
				ASSERT(tileIndex(j, i) < static_cast<int>(data->tilemap.size()));
				if (data->tilemap[tileIndex(j, i)] >= 0) {
					tiles->render(x, y, data->tilemap[tileIndex(j, i)]);
				}
			}
		}
	}
	for (GameObject* obj : data->entities) {
		obj->render();
	}
	// TODO: determine if portals need to be rendered or if they are invisible
	for (const PortalEntry& v : data->portals) {
		QcEngine::getTexture(PORTAL)->render(v.x * data->tile_size - data->cam_x, v.y * data->tile_size - data->cam_y);
	}
	// render the dialogue if there is any
	if (current_text.size() > 0) {
		SDL_Texture * text = getTextTexture(current_text, "default_24", { 255, 255, 255 });
		Texture tex(text);
		tex.render(20, 650);
	}
}

void Game::handleKeyPresses() {
	// clear the key state at the start of every frame
	data->keyStates = 0;
	// quit the game if the escape key is pressed
	if (keyPressed(SDL_SCANCODE_ESCAPE)) {
		exit();
	}
	// update the key states
	if (keyPressed(SDL_SCANCODE_RIGHT)) {
		data->keyStates |= KEY_RIGHT;
	}
	if (keyPressed(SDL_SCANCODE_LEFT)) {
		data->keyStates |= KEY_LEFT;
	}
	if (keyPressed(SDL_SCANCODE_SPACE)) {
		data->keyStates |= KEY_SPACE;
	}
	if (keyDown(SDL_SCANCODE_Z)) {
		data->keyStates |= KEY_ATTACK_DOWN;
	}
	if (keyUp(SDL_SCANCODE_Z)) {
		data->keyStates |= KEY_ATTACK_UP;
	}
	if (keyDown(SDL_SCANCODE_UP)) {
		data->keyStates |= KEY_UP;
	}
	if (keyDown(SDL_SCANCODE_C)) {
		current_text = "";
	}
}

void Game::addEntity(GameObject * obj) {
	data->entities.push_back(obj);
}

void Game::updateCamera() {
	// TODO: remove magic numbers by using percentages
	// TODO: adjust for player sprite size
	// TODO: use acceleration
	if (player->facingRight()) {
		if (player->getX() - data->cam_x > 500) {
			data->cam_x += static_cast<int>(CAMERA_SPEED * delta / 1000.f);
			if (player->getX() - data->cam_x < 500) {
				data->cam_x = player->getX() - 500;
			}
		}
	} else {
		if (player->getX() - data->cam_x < 1280 - 500) {
			data->cam_x -= static_cast<int>(CAMERA_SPEED * delta / 1000.f);
			if (player->getX() - data->cam_x > 1280 - 500) {
				data->cam_x = player->getX() - 1280 + 500;
			}
		}
	}
	// TODO: refactor code (a lot of duplicate code here)
	// TODO: take into account player size
	if (player->onGround()) {
		if (player->getY() - data->cam_y > 520) {
			data->cam_y += static_cast<int>(CAMERA_SPEED * delta / 1000.f);
			if (player->getY() - data->cam_y < 520) {
				data->cam_y = player->getY() - 520;
			}
		}
		if (player->getY() - data->cam_y < 520) {
			data->cam_y -= static_cast<int>(CAMERA_SPEED * delta / 1000.f);
			if (player->getY() - data->cam_y > 520) {
				data->cam_y = player->getY() - 520;
			}
		}
	} else {
		if (player->getY() - data->cam_y > 720 - 100) {
			data->cam_y = player->getY() - 720 + 100;
		}
		if (player->getY() - data->cam_y < 100) {
			data->cam_y = player->getY() - 100;
		}
	}
	// finally restrict the camera to be within the play space
	// TODO: get rid of magic numbers
	if (data->tile_size * data->map_width > 1280) {
		if (data->cam_x < 0) data->cam_x = 0;
		if (data->cam_x > data->map_width * data->tile_size - 1280) data->cam_x = data->map_width * data->tile_size - 1280;
	} else {
		data->cam_x = -(1280 - data->tile_size * data->map_width) / 2;
	}
	if (data->tile_size * data->map_height > 720) {
		if (data->cam_y < 0) data->cam_y = 0;
		if (data->cam_y > data->map_height * data->tile_size - 720) data->cam_y = data->map_height * data->tile_size - 720;
	} else {
		data->cam_y = -(720 - data->tile_size * data->map_height) / 2;
	}
}

void Game::clearMap() {
	for (GameObject* obj : data->entities) {
		delete obj;
	}
	data->entities.clear();
	while (!data->new_entities.empty()) {
		delete data->new_entities.front();
		data->new_entities.pop();
	}
	data->tilemap.clear();
	data->collisionmap.clear();
	data->portals.clear();
}

bool GameData::collidingWithTiles(const Math::Shape & collision, int range, bool quad) {
	if (range > 0) {
		int start_x = collision.pos.x / tile_size;
		int start_y = collision.pos.y / tile_size;
		for (int i = quad ? -range : 0; i < range; i++) {
			for (int j = quad ? -range : 0; j < range; j++) {
				int t_x = start_x + j;
				int t_y = start_y + i;
				// make sure the tile coordinates aren't out of bounds
				if (t_x < 0 || t_y < 0) continue;
				if (t_x >= map_width || t_y >= map_height) continue;
				if (collisionmap[t_y * map_width + t_x] == 1) {
					Math::Rectangle target = Math::Rectangle(t_x * tile_size, t_y * tile_size, tile_size, tile_size);
					if (Math::isColliding(collision, target)) {
						return true;
					}
				}
			}
		}
	} else {
		for (int i = 0; i < map_height; i++) {
			for (int j = 0; j < map_width; j++) {
				if (collisionmap[i * map_width + j] == 1) {
					// REPLACE MAGIC NUMBERS
					Math::Rectangle target = Math::Rectangle(j * tile_size, i * tile_size, tile_size, tile_size);
					if (Math::isColliding(collision, target)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

void Game::loadMap(const std::string & path, bool verbose) {
	if (verbose) LOG("+= LOADING MAP: " << path << " =+");
	// things to be loaded
	int start_x = -1;
	int start_y = -1;
	std::string tilemap_source;
	std::string background_source;
	// clear previous map data before loading in new data
	clearMap();
	// open the map file to read
	std::fstream map_file;
	map_file.open(path, std::fstream::in);
	// load the map data
	if (map_file.is_open()) {
		try {
			json map_data;
			map_file >> map_data;
			data->tile_size = map_data["tile size"];
			if (verbose) LOG("TILE SIZE: " << data->tile_size);
			int i = 0;
			for (const auto& vec : map_data["start"]) {
				if (i == start_index) {
					start_x = vec["x"];
					start_y = vec["y"];
					if (verbose) LOG("START POS: " << start_x << ", " << start_y);
					break;
				}
			}
			// check to make sure we assigned a start, or else default to 0
			if (start_x < 0 || start_y < 0) {
				if (verbose) LOG("NO START FOUND, DEFAULTING TO 0");
				start_x = 0;
				start_y = 0;
			}
			for (const auto& v : map_data["portals"]) {
				data->portals.push_back({v["x"], v["y"], v["file"], v["index"]});
				if (verbose) LOG("ADDED PORTAL AT: " << v["x"] << ", " << v["y"] << " TO FILE: " << v["file"] << " AT INDEX " << v["index"]);
			}
			for (const auto& entity : map_data["entities"]) {
				if (entity["type"] == "enemy") {
					addEntity(new Enemy(entity["x"] * data->tile_size, entity["y"] * data->tile_size));
					if (verbose) LOG("ENEMY SPAWNED AT: " << entity["x"] << ", " << entity["y"]);
				}
			}
			tilemap_source = map_data["tilemap source"].get<std::string>();
			if (verbose) LOG("TILEMAP SOURCE: " << tilemap_source);
			background_source = map_data["background source"].get<std::string>();
			if (verbose) LOG("BACKGROUND SOURCE: " << background_source);
			data->map_width = map_data["map width"];
			data->map_height = map_data["map height"];
			if (verbose) LOG("MAP WIDTH: " << data->map_width);
			if (verbose) LOG("MAP HEIGHT: " << data->map_height);
			data->tilemap = map_data["tiles"].get<std::vector<int>>();
			data->collisionmap = map_data["collision"].get<std::vector<bool>>();
			map_file.close();
		} catch (const std::exception& e) {
			std::cerr << e.what();
			// handle "WRONG MAP FORMAT" error
		}
	} else {
		map_file.close();
		// hanlde "LOAD FAILED" error
	}
	// assume the file loaded successfully for now
	player = new Player(start_x * data->tile_size, start_y * data->tile_size);	// starting position in tile coords
	addEntity(player);
	LOG("PLAYER ADDED AT: " << player->getX() << ", " << player->getY());
	tiles = static_cast<TileMap*>(QcEngine::loadTexture(TILEMAP, tilemap_source, T_TILEMAP));
	tiles->generateTiles(data->tile_size, data->tile_size);
	// TOOD: figure out better way to store backgrounds
	QcEngine::loadTexture("bg", background_source);

	LOG("+= FINISHED LOADING MAP =+");

}