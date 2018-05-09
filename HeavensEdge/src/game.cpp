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
	player = new Player(100, 50);
	addEntity(player);
	addEntity(new Enemy(200, 50));
	data->cam_x = 0;
	data->cam_y = 0;
	// initialize textures
	QcEngine::loadTexture("tile", "../assets/tile.png");
	QcEngine::loadTexture("arrow", "../assets/arrow.png");
	AnimatedTexture * t = static_cast<AnimatedTexture*>(QcEngine::loadTexture("enemy", "../assets/enemy.png", T_ANIMATED));
	t->generateAtlas(64, 128);
	t->addAnimationState({ 0, 3 });
	// TODO: have the map file load more game state data
	std::fstream map_file;
	map_file.open(DEFAULT_MAP_FILE, std::fstream::in);
	if (map_file.is_open()) {
int version = 0;
map_file >> version;
if (version <= 1) {
	map_file >> data->map_width;
	map_file >> data->map_height;
	while (!map_file.eof()) {
		int tile;
		map_file >> tile;
		data->tilemap.push_back(tile);
		if (tile != 0) data->collisionmap.push_back(true);
		else data->collisionmap.push_back(false);
	}
} else {
	// handle "LOAD FAILED: UNHANDLED VERSION" error
}
map_file.close();
	} else {
	// hanlde "LOAD FAILED" error
	}
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
	for (int i = 0; i < data->map_height; ++i) {
		for (int j = 0; j < data->map_width; ++j) {
			ASSERT(i * data->map_width + j < data->tilemap.size());
			if (data->tilemap[i * data->map_width + j] == 1) {
				QcEngine::getTexture("tile")->render(j * 64 - data->cam_x, i * 64 - data->cam_y);
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
	// int screen_x = player->getX() - data->cam_x;
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
	// int screen_y = player->getY() - data->cam_y;
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
