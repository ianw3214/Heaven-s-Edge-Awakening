#include "game.hpp"

#include "entities\enemy.hpp"

Game::Game() {

}

Game::~Game() {

}

void Game::init() {
	// setup the game data struct
	data = new GameData{};
	GameObject::setGameData(data);
	data->cam_x = 0;
	data->cam_y = 0;
	// initialize textures
	tiles = nullptr;
	QcEngine::loadTexture("arrow", "../assets/arrow.png");
	AnimatedTexture * t = static_cast<AnimatedTexture*>(QcEngine::loadTexture("enemy", "../assets/enemy.png", T_ANIMATED));
	t->generateAtlas(64, 128);
	t->addAnimationState({ 0, 3 });
	// load the map data from a specified path
	loadMap();
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
}

void Game::render() {
	// TOOD: figure out better way to store backgrounds
	// render parallax background
	QcEngine::getTexture("bg")->render(-data->cam_x / 2, -data->cam_y / 2);
	for (int i = 0; i < data->map_height; ++i) {
		for (int j = 0; j < data->map_width; ++j) {
			ASSERT(i * data->map_width + j < data->tilemap.size());
			if (data->tilemap[tileIndex(j, i)] >= 0) {
				tiles->render(j * data->tile_size - data->cam_x, i * data->tile_size - data->cam_y, data->tilemap[tileIndex(j, i)]);
			}
		}
	}
	for (GameObject* obj : data->entities) {
		obj->render();
	}
}

void Game::handleKeyPresses() {
	// clear the key state at the start of every frame
	data->keyStates = 0;
	// quit the game if the escape key is pressed
	if (keyPressed(SDL_SCANCODE_ESCAPE)) {
		exit();
	}
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
}

bool GameData::collidingWithTiles(const Math::Shape & collision) {
	for (int i = 0; i < map_height; i++) {
		for (int j = 0; j < map_width; j++) {
			if (collisionmap[i * map_width + j] == 1) {
				// REPLACE MAGIC NUMBERS
				Math::Rectangle target = Math::Rectangle(j * 64, i * 64, 64, 64);
				if (Math::isColliding(collision, target)) {
					return true;
				}
			}
		}
	}
	return false;
}

void Game::loadMap(const std::string & path) {
	// things to be loaded
	int start_x;
	int start_y;
	std::string tilemap_source;
	std::string background_source;
	// clear previous map data before loading in new data
	clearMap();
	// open the map file to read
	std::fstream map_file;
	map_file.open(path, std::fstream::in);
	if (map_file.is_open()) {
		// check a version number
		int version;
		map_file >> version;
		if (version != 1) {
			// handle "VERSION INVALID" error
		}
		map_file >> data->tile_size;
		map_file >> start_x;
		map_file >> start_y;
		// TODO: handle this correctly
		int num_entities;
		map_file >> num_entities;
		// TODO: handle this correctly
		map_file >> tilemap_source;
		map_file >> background_source;
		map_file >> data->map_width;
		map_file >> data->map_height;
		int tile;
		for (int i = 0; i < data->map_width * data->map_height; ++i) {
			map_file >> tile;
			data->tilemap.push_back(tile);
		}
		for (int i = 0; i < data->map_width * data->map_height; ++i) {
			map_file >> tile;
			data->collisionmap.push_back(tile);
		}
		map_file.close();
	} else {
		// handle "LOAD FAILED" error
	}
	// assume the file loaded successfully for now
	player = new Player(start_x, start_y);
	addEntity(player);
	tiles = static_cast<TileMap*>(QcEngine::loadTexture(TILEMAP, tilemap_source, T_TILEMAP));
	tiles->generateTiles(data->tile_size, data->tile_size);
	// TOOD: figure out better way to store backgrounds
	QcEngine::loadTexture("bg", background_source);
}