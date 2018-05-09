#pragma once

#include "QcEngine.hpp"
#include "gameObject.hpp"
#include "../util.hpp"

#define ENEMY_WIDTH		64
#define ENEMY_HEIGHT	128

#define ENEMY_DEATH_TIME (2000)

class Enemy : public GameObject {

public:

	Enemy();
	Enemy(int x, int y);
	~Enemy();

	void update(Uint32 delta);
	void render();

private:

	// movement variables
	int x, y;
	float y_vel;
	bool on_ground;

	Math::Rectangle collision;

	// enemy state variables
	int health;
	Timer death_timer;

	// animation helper variables
	int currentFrame;

	void init();
	void move(Direction dir, int distance);

};