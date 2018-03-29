#include "game.hpp"

#include <cmath>
#define PI 3.14159265

Game::Game() {

}

Game::~Game() {

}

void Game::init() {
	// initialize player data
	player = { TILE_SIZE * 2, TILE_SIZE * 2, 0, false, false, true };
	// initialize textures
	tile_texture = new Texture("assets/tile.png");
	arrow_texture = new Texture("assets/arrow.png");
	arrow_texture->setCentre(0, 12);
	player_texture = new AnimatedTexture("assets/player.png");
	player_texture->generateAtlas(64, 128, 2);
	player_texture->addAnimationState({ 0, 0 });
	player_texture->addAnimationState({ 1, 1 });
	// initialize map
	for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
		if (i / MAP_WIDTH == MAP_HEIGHT - 1) {
			tilemap.emplace_back(1);
			collisionmap.emplace_back(1);
		}
		else if (i % MAP_WIDTH == 0 || i % MAP_WIDTH == MAP_WIDTH - 1) {
			tilemap.emplace_back(1);
			collisionmap.emplace_back(1);
		}
		else {
			tilemap.emplace_back(0);
			collisionmap.emplace_back(0);
		}
	}
	// initialize game state
	cam_x = 0;
	cam_y = 0;
}

void Game::cleanup() {

}

void Game::pause() {

}

void Game::resume() {

}

void Game::update() {
	handleKeyPresses();
	updatePlayer();
	updateCamera();
	updateArrows();
}

void Game::render() {
	for (unsigned int i = 0; i < MAP_HEIGHT; ++i) {
		for (unsigned int j = 0; j < MAP_WIDTH; ++j) {
			ASSERT(i * MAP_WIDTH + j < tilemap.size());
			if (tilemap[i * MAP_WIDTH + j] == 1) {
				tile_texture->render(j * TILE_SIZE - cam_x + SCREEN_WIDTH / 2, i * TILE_SIZE);
			}
		}
	}
	player_texture->render(player.x - cam_x + SCREEN_WIDTH / 2, player.y);
	for (const Arrow& a : arrows) {
		arrow_texture->setAngle(a.angle);
		arrow_texture->render(a.x - cam_x + SCREEN_WIDTH / 2, a.y - 12);
	}
}

void Game::handleKeyPresses() {
	// quit the game if the escape key is pressed
	if (keyPressed(SDL_SCANCODE_ESCAPE)) {
		exit();
	}
	if (keyPressed(SDL_SCANCODE_RIGHT)) {
		movePlayer(RIGHT, (int)(delta / 1000.f * PLAYER_SPEED));
		player_texture->changeAnimation(PLAYER_RIGHT);
		player.face_right = true;
	}
	if (keyPressed(SDL_SCANCODE_LEFT)) {
		movePlayer(LEFT, (int)(delta / 1000.f * PLAYER_SPEED));
		player_texture->changeAnimation(PLAYER_LEFT);
		player.face_right = false;
	}
	if (keyPressed(SDL_SCANCODE_SPACE) && !player.jumping) {
		player.y_accel = -20;
		player.jumping = true;
	}
	// TODO: add bow scaling
	if (keyDown(SDL_SCANCODE_Z)) {
		LOG("BOW CHARGE")
	}
	if (keyUp(SDL_SCANCODE_Z)) {
		if (player.face_right) {
			arrows.push_back({ player.x + TILE_SIZE, player.y, true, 1500, -10, Math::Line(player.x + TILE_SIZE, player.y, player.x + TILE_SIZE, player.y + 48), false});
		}
		else {
			arrows.push_back({ player.x, player.y, false, 1500, -10, Math::Line(player.x , player.y, player.x , player.y + 48), false });
		}
	}
}

void Game::updatePlayer() {
	// if the player is on the ground, don't worry about calculating new y position
	if (!player.jumping && playerColliding(player.x, player.y + 1)) {
		player.y_accel = 0;
		player.on_ground = true;
	}
	else {
		// TODO: fix gravity to not rely on framerate
		// calculate change in y position for the player
		player.y_accel += (int)(delta / 1000.f * GRAVITY);
		if (player.y_accel != 0) {
			player.y_accel = player.y_accel > SPEED_CAP ? SPEED_CAP : player.y_accel;
			movePlayer(DOWN, player.y_accel);
		}
		// if the player is on the ground, set jumping to false again
		if (playerColliding(player.x, player.y + 1)) {
			player.on_ground = true;
			player.jumping = false;
		}
	}
}

// TODO: change camera focus based on what direction player is facing
void Game::updateCamera() {
	if (cam_x + CAMERA_MARGIN < player.x) {
		cam_x += (int)(CAMERA_SPEED * delta / 1000.f);
	}
	else if (cam_x - CAMERA_MARGIN > player.x) {
		cam_x -= (int)(CAMERA_SPEED * delta / 1000.f);
	}
	if (cam_y + CAMERA_MARGIN < player.y) {
		cam_y += (int)(CAMERA_SPEED * delta / 1000.f);
	}
	else if (cam_y - CAMERA_MARGIN > player.y) {
		cam_y -= (int)(CAMERA_SPEED * delta / 1000.f);
	}
}

void Game::updateArrows() {
	for (Arrow& a : arrows) {
		if (!a.stopped) {
			// calculate a new position based on current arrow state
			int distance = (int)(a.h_velocity * delta / 1000.f);
			if (a.right) {
				a.x += distance;
			}
			else {
				a.x -= distance;
			}
			// calculate new arrow y position
			a.y_accel += (int)(delta / 1000.f * GRAVITY);
			if (a.y_accel != 0) {
				a.y_accel = a.y_accel > SPEED_CAP ? SPEED_CAP : a.y_accel;
				a.y += a.y_accel;
			}
			// calculate angle
			double angle = std::atan((double)(a.y_accel) / (double)(a.right ? distance : -distance));
			if (!a.right) {
				angle += PI;
			}
			a.angle = (int)(angle * 180 / PI);
			// calculate a collision line based on the angle
			a.collision.pos = Vec2(a.x, a.y);
			a.collision.pos2 = Vec2(a.x, a.y);
			a.collision.pos2.x += (int)(48.0 * std::cos(a.angle));
			a.collision.pos2.y += (int)(48.0 * std::sin(a.angle));
			// the arrow should be stopped if it is colliding with something
			if (collidingWithTile(a.collision)) {
				a.stopped = true;
			}
		}
	}
}

// TODO: use some other stepping other than 1 pixel at a time to optimize
void Game::movePlayer(Direction dir, int distance) {
	int new_x = player.x;
	int new_y = player.y;
	if (dir == UP) new_y -= distance;
	if (dir == RIGHT) new_x += distance;
	if (dir == DOWN) new_y += distance;
	if (dir == LEFT) new_x -= distance;
	// tick represents how much to dial back on each iteration
	int tick = dir == UP || dir == LEFT ? -1 : 1;
	while (playerColliding(new_x, new_y)) {
		if (dir == UP || dir == DOWN) new_y -= tick;
		if (dir == RIGHT || dir == LEFT) new_x -= tick;
	}
	player.x = new_x;
	player.y = new_y;
}

// TODO: select tiles to calculate with instead of comparing against every single tile
bool Game::playerColliding(int x, int y) const {
	Math::Rectangle player_collision = Math::Rectangle(x, y, TILE_SIZE, TILE_SIZE * 2);
	return collidingWithTile(player_collision);
}

bool Game::collidingWithTile(Math::Shape& shape) const {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (collisionmap[i * MAP_WIDTH + j] == 1) {
				Math::Rectangle target = Math::Rectangle(j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE);
				if (Math::isColliding(shape, target)) {
					return true;
				}
			}
		}
	}
	return false;
}
