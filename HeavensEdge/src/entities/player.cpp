#include "player.hpp"
#include "../game.hpp"

#include "arrow.hpp"

Player::Player() : GameObject(PLAYER_OBJECT) {
	init();
}

Player::Player(int x, int y) : x(x), y(y), GameObject(PLAYER_OBJECT) {
	init();
}

Player::~Player() {

}

int Player::getX() const {
	return x;
}

int Player::getY() const {
	return y;
}

bool Player::facingRight() const {
	return face_right;
}

bool Player::onGround() const {
	return on_ground;
}

void Player::render() {
	texture->render(x - data->cam_x, y - data->cam_y);
	// render the charge/tick
	if (charging) {
		// TODO: remove magic numbers
		QcEngine::getTexture("charge")->render(x - data->cam_x, y - 30 - data->cam_y);
		float percentage = static_cast<float>(charge_timer.getTicks()) / static_cast<float>(1500);
		percentage = clamp(percentage, 0.f, 1.f);
		int x_diff = static_cast<int>(64 * percentage) - 6;
		QcEngine::getTexture("tick")->render(x + x_diff - data->cam_x, y - 32 - data->cam_y);
	}
}

void Player::update(Uint32 delta) {
	// handle player based on key states
	handleKeyStates(delta);
	// TODO: REMOVE MAGIC NUMBERS HERE
	Math::Rectangle top_test = Math::Rectangle(x, y - 1, PLAYER_WIDTH, PLAYER_HEIGHT);
	if (jumping && data->collidingWithTiles(top_test)) {
		y_vel = -y_vel / 4.f;
	}
	// calculate change in y position for the player
	y_vel += static_cast<int>(delta / 1000.f * GRAVITY);
	if (y_vel != 0.f) {
		move(DIR_DOWN, static_cast<int>(y_vel * delta / 1000.f));
	}
	// if the player is on the ground, set jumping to false again
	Math::Rectangle bottom_test = Math::Rectangle(x, y + 1, PLAYER_WIDTH, PLAYER_HEIGHT);
	if (data->collidingWithTiles(bottom_test)) {
		on_ground = true;
		jumping = false;
		y_vel = 0.f;
	} else {
		on_ground = false;
	}
	// update the player animation state
	if (!on_ground) {
		int cur = texture->getCurrentAnimation();
		if (cur != PLAYER_RIGHT_JUMP && cur != PLAYER_LEFT_JUMP && cur != PLAYER_RIGHT_AIR && cur != PLAYER_LEFT_AIR) {
			if (face_right) {
				texture->changeAnimation(PLAYER_RIGHT_AIR);
			} else {
				texture->changeAnimation(PLAYER_LEFT_AIR);
			}
		}
	} else if (data->keyStates & KEY_RIGHT) {
		texture->changeAnimation(PLAYER_RIGHT_RUN);
	} else if (data->keyStates & KEY_LEFT) {
		texture->changeAnimation(PLAYER_LEFT_RUN);
	} else {
		if (face_right) {
			texture->changeAnimation(PLAYER_RIGHT_IDLE);
		} else {
			texture->changeAnimation(PLAYER_LEFT_IDLE);
		}
	}
}

void Player::init() {
	// SETUP TEXTURE DATA
	texture = static_cast<AnimatedTexture*>(QcEngine::loadTexture("player", "../assets/player.png", T_ANIMATED));
	texture->generateAtlas(64, 128);
	texture->addAnimationState({ 0, 3 });		// IDLE RIGHT
	texture->addAnimationState({ 4, 7 });		// IDLE LEFT
	texture->addAnimationState({ 8, 11 });		// RUN RIGHT
	texture->addAnimationState({ 12, 15 });		// RUN LEFT
	texture->addAnimationState({ 16, 19 }, PLAYER_RIGHT_AIR);		// JUMP RIGHT
	texture->addAnimationState({ 20, 23 }, PLAYER_LEFT_AIR);		// JUMP LEFT
	texture->addAnimationState({ 24, 27 });		// AIR RIGHT
	texture->addAnimationState({ 28, 31 });		// AIR LEFT
	current_animation = PLAYER_LEFT_IDLE;
	// SETUP COLLISION DATA
	collision = Math::Rectangle(x, y, PLAYER_WIDTH, PLAYER_HEIGHT);
	// INITIALIZE ADDITIONAL NEEDED TEXTURES
	QcEngine::loadTexture("charge", "../assets/charge.png");
	QcEngine::loadTexture("tick", "../assets/tick.png");
}

void Player::handleKeyStates(Uint32 delta) {
	if (data->keyStates & KEY_RIGHT) {
		// MOVE RIGHT
		move(DIR_RIGHT, (int)(delta / 1000.f * PLAYER_SPEED));
		face_right = true;
	}
	if (data->keyStates & KEY_LEFT) {
		// MOVE LEFT
		move(DIR_LEFT, (int)(delta / 1000.f * PLAYER_SPEED));
		face_right = false;
	}
	if (data->keyStates & KEY_SPACE) {
		// JUMP
		// TODO: enapsulate in a function
		if (!jumping && on_ground) {
			y_vel = -PLAYER_JUMP_VEL;
			jumping = true;
			if (face_right) {
				texture->changeAnimation(PLAYER_RIGHT_JUMP);
			} else {
				texture->changeAnimation(PLAYER_LEFT_JUMP);
			}
		}
	}
	if (data->keyStates & KEY_ATTACK_DOWN) {
		// CHARGE BOW
		charging = true;
		charge_timer.reset(true);
	}
	if (data->keyStates & KEY_ATTACK_UP) {
		// RELEASE BOW
		charging = false;
		// TODO: remove magic numbers
		// calculate how charged the arrow is
		float percentage = static_cast<float>(charge_timer.getTicks()) / static_cast<float>(1500);
		percentage = clamp(percentage, 0.f, 1.f);
		int h_vel = 500 + static_cast<int>(percentage * 1500);
		Arrow * new_arrow = new Arrow(x + 32, y + 16, face_right, h_vel, -800);
		data->new_entities.push(new_arrow);
		
	}
}

void Player::move(Direction dir, int distance) {
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
