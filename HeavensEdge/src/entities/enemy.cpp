#include "enemy.hpp"
#include "../game.hpp"

Enemy::Enemy() : GameObject(ENEMY_OBJECT) {
	init();
}

Enemy::Enemy(int x, int y) : x(x), y(y), GameObject(ENEMY_OBJECT) {
	init();
}

Enemy::~Enemy() {

}

void Enemy::update(Uint32 delta) {
	// if the player is on the ground, don't worry about calculating new y position
	// TODO: REMOVE MAGIC NUMBERS HERE
	Math::Rectangle bottom_test = Math::Rectangle(x, y + 1, ENEMY_WIDTH, ENEMY_HEIGHT);
	if (data->collidingWithTiles(bottom_test)) {
		y_vel = 0.f;
		on_ground = true;
	} else {
		// calculate change in y position for the player
		y_vel += static_cast<int>(GRAVITY * delta / 1000.f);
		if (y_vel != 0.f) {
			move(DIR_DOWN, static_cast<int>(y_vel * delta / 1000.f));
		}
	}
	// check if the enemy should be removed
	if (health <= 0 && death_timer.getTicks() > ENEMY_DEATH_TIME) {
		remove = true;
	}
}

void Enemy::render() {
	currentFrame = static_cast<AnimatedTexture*>(QcEngine::getTexture("enemy"))->getNextFrame(currentFrame);
	static_cast<AnimatedTexture*>(QcEngine::getTexture("enemy"))->render(x - data->cam_x, y - data->cam_y, currentFrame);
}

void Enemy::init() {
	currentFrame = 0;
	y_vel = 0.f;
	// SETUP COLLISION DATA
	collision = Math::Rectangle(x, y, ENEMY_WIDTH, ENEMY_HEIGHT);
}

void Enemy::move(Direction dir, int distance) {
	if (distance == 0) return;
	if (dir == DIR_UP) y -= distance;
	if (dir == DIR_RIGHT) x += distance;
	if (dir == DIR_DOWN) y += distance;
	if (dir == DIR_LEFT) x -= distance;
	collision.pos = Vec2(x, y);
	// tick represents how much to dial back on each iteration
	int tick = (dir == DIR_UP || dir == DIR_LEFT ? -1 : 1) * (distance / std::abs(distance));
	while (data->collidingWithTiles(collision)) {
		if (dir == DIR_UP || dir == DIR_DOWN) y -= tick;
		if (dir == DIR_RIGHT || dir == DIR_LEFT) x -= tick;
		collision.pos = Vec2(x, y);
	}
}