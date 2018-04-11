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
	charge_texture = new Texture("assets/charge.png");
	tick_texture = new Texture("assets/tick.png");
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
	if (charging) {
		charge_texture->render(player.x - cam_x + SCREEN_WIDTH / 2, player.y - 30);
		float percentage = static_cast<float>(charge_timer.getTicks()) / static_cast<float>(MAX_BOW_CHARGE);
		percentage = clamp(percentage, 0.f, 1.f);
		int x_diff = static_cast<int>(64 * percentage) - 6;
		tick_texture->render(player.x + x_diff - cam_x + SCREEN_WIDTH / 2, player.y - 32);
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
		player.y_vel = PLAYER_JUMP_VEL;
		player.jumping = true;
	}
	// TODO: add bow scaling
	if (keyDown(SDL_SCANCODE_Z)) {
		charging = true;
		charge_timer.reset(true);
	}
	if (keyUp(SDL_SCANCODE_Z)) {
		charging = false;
		// calculate how charged the arrow is
		float percentage = static_cast<float>(charge_timer.getTicks()) / static_cast<float>(MAX_BOW_CHARGE);
		percentage = clamp(percentage, 0.f, 1.f);
		Arrow arrow{player.x + 32, player.y + 16, player.face_right, 0, -800, Math::Line(), player.face_right ? 0 : 180, false};
		arrow.h_velocity = BASE_ARROW_SPEED + static_cast<int>(percentage * ARROW_SPEED);
		arrow.collision = Math::Line(player.x, player.y, player.x, player.y);
		arrows.push_back(arrow);
	}
}

void Game::updatePlayer() {
	// if the player is on the ground, don't worry about calculating new y position
	if (!player.jumping && playerColliding(player.x, player.y + 1)) {
		player.y_vel = 0.f;
		player.on_ground = true;
	}
	else {
		// TODO: fix gravity to not rely on framerate
		// calculate change in y position for the player
		player.y_vel += static_cast<int>(delta / 1000.f * GRAVITY);
		if (player.y_vel != 0.f) {
			player.y_vel = player.y_vel > SPEED_CAP ? SPEED_CAP : player.y_vel;
			movePlayer(DOWN, static_cast<int>(player.y_vel * delta / 1000.f));
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
			// check if the arrow is colliding with anything before moving it
			if (collidingWithTile(a.collision)) {
				a.stopped = true;
				break;
			}
			// calculate a new position based on current arrow state
			int distance = (int)(a.h_velocity * delta / 1000.f);
			if (a.right) {
				a.x += distance;
			}
			else {
				a.x -= distance;
			}
			// calculate new arrow y position
			a.y_vel += static_cast<float>(delta / 1000.f * GRAVITY);
			int y_move = static_cast<int>(a.y_vel * delta / 1000.f);
			a.y_vel = a.y_vel > SPEED_CAP ? SPEED_CAP : a.y_vel;
			a.y += y_move;
			// calculate angle
			double angle = std::atan2((double)(y_move), (double)(a.right ? distance : -distance));
			a.angle = static_cast<int>(angle * 180 / PI);
			// calculate a collision line based on the angle
			a.collision.pos = Vec2(a.x, a.y);
			a.collision.pos2 = Vec2(a.x, a.y);
			a.collision.pos2.x += (int)(ARROW_WIDTH * std::cos(angle));
			a.collision.pos2.y += (int)(ARROW_WIDTH * std::sin(angle));
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
