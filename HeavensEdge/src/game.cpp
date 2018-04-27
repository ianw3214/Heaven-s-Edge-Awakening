#include "game.hpp"

#include <cmath>
#define PI 3.14159265

Game::Game() {

}

Game::~Game() {

}

void Game::init() {
	// initialize player data
	player = { TILE_SIZE * 2, TILE_SIZE * 2, 0, false, false, true, 5 };
	player.collision = Math::Rectangle(player.x, player.y, 64, 128);
	// initialize other entity data
	enemies.push_back({ 248, 300 , 0.f, false, 5, Math::Rectangle(238, 300, 64, 128) });
	// initialize textures
	loadTexture("tile", "../assets/tile.png");
	loadTexture("arrow", "../assets/arrow.png")->setCentre(0, 12);
	loadTexture("charge", "../assets/charge.png");
	loadTexture("tick", "../assets/tick.png");
	player_texture = new AnimatedTexture("../assets/player.png");
	enemy_texture = new Texture("../assets/enemy.png");
	player_texture->generateAtlas(64, 128, 2);
	player_texture->addAnimationState({ 0, 0 });
	player_texture->addAnimationState({ 1, 1 });
	/*
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
	*/
	// TODO: have the map file load more game state data
	std::fstream map_file;
	map_file.open(DEFAULT_MAP_FILE, std::fstream::in);
	if (map_file.is_open()) {
		int version = 0;
		map_file >> version;
		if (version <= 1) {
			map_file >> map_width;
			map_file >> map_height;
			while (!map_file.eof()) {
				int tile;
				map_file >> tile;
				tilemap.push_back(tile);
				if (tile != 0) collisionmap.push_back(true);
				else collisionmap.push_back(false);
				LOG(tile);
			}
		} else {
			// handle "LOAD FAILED: UNHANDLED VERSION" error
		}
		map_file.close();
	} else {
		// hanlde "LOAD FAILED" error
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
	updateEnemies();
	updateArrows();
}

void Game::render() {
	for (unsigned int i = 0; i < map_height; ++i) {
		for (unsigned int j = 0; j < map_width; ++j) {
			ASSERT(i * map_width + j < tilemap.size());
			if (tilemap[i * map_width + j] == 1) {
				getTexture("tile")->render(j * TILE_SIZE - cam_x + SCREEN_WIDTH / 2, i * TILE_SIZE);
			}
		}
	}
	player_texture->render(player.x - cam_x + SCREEN_WIDTH / 2, player.y);
	for (const Arrow& a : arrows) {
		getTexture("arrow")->setAngle(a.angle);
		getTexture("arrow")->render(a.x - cam_x + SCREEN_WIDTH / 2, a.y - 12);
	}
	for (const Enemy& e : enemies) {
		enemy_texture->render(e.x - cam_x + SCREEN_WIDTH / 2 , e.y);
	}
	if (charging) {
		getTexture("charge")->render(player.x - cam_x + SCREEN_WIDTH / 2, player.y - 30);
		float percentage = static_cast<float>(charge_timer.getTicks()) / static_cast<float>(MAX_BOW_CHARGE);
		percentage = clamp(percentage, 0.f, 1.f);
		int x_diff = static_cast<int>(64 * percentage) - 6;
		getTexture("tick")->render(player.x + x_diff - cam_x + SCREEN_WIDTH / 2, player.y - 32);
	}
}

void Game::handleKeyPresses() {
	// quit the game if the escape key is pressed
	if (keyPressed(SDL_SCANCODE_ESCAPE)) {
		exit();
	}
	if (keyPressed(SDL_SCANCODE_RIGHT)) {
		Math::Rectangle r = Math::Rectangle(player.x, player.y, 64, 128);
		moveEntity(RIGHT, (int)(delta / 1000.f * PLAYER_SPEED), r, E_PLAYER, player.x, player.y);
		player_texture->changeAnimation(PLAYER_RIGHT);
		player.face_right = true;
	}
	if (keyPressed(SDL_SCANCODE_LEFT)) {
		Math::Rectangle r = Math::Rectangle(player.x, player.y, 64, 128);
		moveEntity(LEFT, (int)(delta / 1000.f * PLAYER_SPEED), r, E_PLAYER, player.x, player.y);
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
	// TODO: REMOVE MAGIC NUMBERS HERE
	if (!player.jumping && entityColliding(player.x, player.y + 1, player.collision, E_PLAYER)) {
		player.y_vel = 0.f;
		player.on_ground = true;
	}
	else {
		// calculate change in y position for the player
		player.y_vel += static_cast<int>(delta / 1000.f * GRAVITY);
		if (player.y_vel != 0.f) {
			player.y_vel = player.y_vel > SPEED_CAP ? SPEED_CAP : player.y_vel;
			moveEntity(DOWN, static_cast<int>(player.y_vel * delta / 1000.f), player.collision, E_PLAYER, player.x, player.y);
		}
		// if the player is on the ground, set jumping to false again
		if (entityColliding(player.x, player.y + 1, player.collision, E_PLAYER)) {
			player.on_ground = true;
			player.jumping = false;
		}
	}
	// update the collision of the player to match its new position
	player.collision.pos.x = player.x;
	player.collision.pos.y = player.y;
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

// ------------------------------------------------------------------------------
// Central update function for arrows
//
//	- Arrow movement functions are not done with the generalized function because
//		the collisions aren't exact, and the object that it hit has to be known
//		in order for the right calculations to be made.
// ------------------------------------------------------------------------------
void Game::updateArrows() {
	auto it = arrows.begin();
	while(it != arrows.end()) {
		Arrow& a = *it;
		if (!a.stopped) {
			// check if the arrow is colliding with anything before moving it
			if (collidingWithTile(a.collision)) {
				a.timer.reset(true);
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
		} else {
			if (a.timer.getTicks() > ARROW_EXPIRE) {
				it = arrows.erase(it);
				continue;
			}
		}
		// check if the arrow hit an enemy
		bool enemy_hit = false;
		for (Enemy& e : enemies) {
			if (Math::isColliding(a.collision, e.collision)) {
				it = arrows.erase(it);
				enemy_hit = true;
				e.health -= 1;
				if (e.health <= 0) {
					e.death_timer.reset(true);
				}
				break;
			}
		}
		if (enemy_hit) continue;
		++it;
	}
}

void Game::updateEnemies() {
	for (Enemy& e : enemies) {
		// if the player is on the ground, don't worry about calculating new y position
		// TODO: REMOVE MAGIC NUMBERS HERE
		if (entityColliding(e.x, e.y + 1, e.collision, E_ENEMY)) {
			e.y_vel = 0.f;
			e.on_ground = true;
		} else {
			// calculate change in y position for the player
			e.y_vel += static_cast<int>(GRAVITY * delta / 1000.f);
			if (e.y_vel != 0.f) {
				e.y_vel = e.y_vel > SPEED_CAP ? SPEED_CAP : e.y_vel;
				moveEntity(DOWN, static_cast<int>(e.y_vel * delta / 1000.f), e.collision, E_ENEMY, e.x, e.y);
			}
		}
		// update the collision of the enemy to its position
		e.collision.pos.x = e.x;
		e.collision.pos.y = e.y;
	}
	// TODO: refactor so that everything is done in a single loop
	// iterate again to see if we should delete the enemy from the game
	auto it = enemies.begin();
	while (it != enemies.end()) {
		Enemy& e = *it;
		if (e.death_timer.getTicks() > ENEMY_DEATH_TIME) {
			it = enemies.erase(it);
		} else {
			++it;
		}
	}
}

// ------------------------------------------------------------------------------
// Generalized movement function for any entity in the game
//
//	@type represents the type of the entity to be moved, so the correct calculations
//		can be made for each entity.
//
//	@x and @y are references to the x and y variables of the entity so that it can
//		be directly changed from within the function itself.
// ------------------------------------------------------------------------------
// TODO: use some other stepping other than 1 pixel at a time to optimize
void Game::moveEntity(Direction dir, int distance, Math::Shape& collision, EntityType type, int& x, int& y) {
	int new_x = x;
	int new_y = y;
	if (dir == UP) new_y -= distance;
	if (dir == RIGHT) new_x += distance;
	if (dir == DOWN) new_y += distance;
	if (dir == LEFT) new_x -= distance;
	// tick represents how much to dial back on each iteration
	int tick = dir == UP || dir == LEFT ? -1 : 1;
	while (entityColliding(new_x, new_y, collision, type)) {
		if (dir == UP || dir == DOWN) new_y -= tick;
		if (dir == RIGHT || dir == LEFT) new_x -= tick;
	}
	x = new_x;
	y = new_y;
}

bool Game::entityColliding(int x, int y, Math::Shape& collision, EntityType type) {
	collision.pos.x = x;
	collision.pos.y = y;
	switch (type) {
	case E_PLAYER: {
		return collidingWithTile(collision) || collidingWithEnemy(collision);
	} break;
	case E_ENEMY: {
		return collidingWithTile(collision) || collidingWithPlayer(collision);
	} break;
	case E_ARROW: {
		return collidingWithTile(collision) || collidingWithEnemy(collision);
	} break;
	}
	return false;
}

// TODO: select tiles to calculate with instead of comparing against every single tile
bool Game::collidingWithTile(Math::Shape& shape) const {
	for (int i = 0; i < map_height; i++) {
		for (int j = 0; j < map_width; j++) {
			if (collisionmap[i * map_width + j] == 1) {
				Math::Rectangle target = Math::Rectangle(j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE);
				if (Math::isColliding(shape, target)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool Game::collidingWithEnemy(Math::Shape & shape) const {
	for (const Enemy& e : enemies) {
		// TODO: REMOVE THESE MAGIC NUMBERS
		if (Math::isColliding(shape, e.collision)) {
			return true;
		}
	}
	return false;
}

bool Game::collidingWithPlayer(Math::Shape & shape) const {
	return Math::isColliding(shape, player.collision);
}
