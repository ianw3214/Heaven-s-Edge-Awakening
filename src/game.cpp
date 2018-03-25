#include "game.hpp"

Game::Game() {

}

Game::~Game() {

}

void Game::init() {
	// initialize player data
	player_data = { 0, 0, 0 };
	tile_texture = new Texture("assets/tile.png");
	player_texture = new Texture("assets/player.png");
	for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
		if (i / MAP_WIDTH == MAP_HEIGHT - 1) {
			tilemap.emplace_back(1);
			collisionmap.emplace_back(1);
		}
		else {
			tilemap.emplace_back(0);
			collisionmap.emplace_back(0);
		}
	}
}

void Game::cleanup() {

}

void Game::pause() {

}

void Game::resume() {

}

void Game::update() {
	// handle key presses
	if (keyDown(SDL_SCANCODE_ESCAPE)) {
		exit();
	}
	if (keyPressed(SDL_SCANCODE_RIGHT)) {
		player_data.x += (int)(delta / 1000.f * PLAYER_SPEED);
	}
	if (keyPressed(SDL_SCANCODE_LEFT)) {
		player_data.x -= (int)(delta / 1000.f * PLAYER_SPEED);
	}
	if (keyPressed(SDL_SCANCODE_SPACE)) {
		player_data.y_accel = -20;
	}
	// handle gravity
	player_data.y_accel += (int)(delta / 1000.f * GRAVITY);
	if (player_data.y_accel != 0) {
		player_data.y_accel = player_data.y_accel > SPEED_CAP ? SPEED_CAP : player_data.y_accel;
		int new_player_y = player_data.y + player_data.y_accel;
		int direction = player_data.y_accel / std::abs(player_data.y_accel);
		while (playerColliding(player_data.x, new_player_y)) {
			new_player_y -= direction;
		}
		player_data.y = new_player_y;
	}
}

void Game::render() {
	for (unsigned int i = 0; i < MAP_HEIGHT; ++i) {
		for (unsigned int j = 0; j < MAP_WIDTH; ++j) {
			ASSERT(i * MAP_WIDTH + j < tilemap.size());
			if (tilemap[i * MAP_WIDTH + j] == 1) {
				tile_texture->render(j * TILE_SIZE, i * TILE_SIZE);
			}
		}
	}
	player_texture->render(player_data.x, player_data.y);
}

bool Game::playerColliding(int x, int y) const {
	Math::Rectangle player_collision = Math::Rectangle(x, y, TILE_SIZE, TILE_SIZE);
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (collisionmap[i * MAP_WIDTH + j] == 1) {
				Math::Rectangle target = Math::Rectangle(j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE);
				if (Math::isColliding(player_collision, target)) {
					return true;
				}
			}
		}
	}
	return false;
}
