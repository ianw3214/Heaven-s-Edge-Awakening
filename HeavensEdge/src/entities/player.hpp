#pragma once

#include "QcEngine.hpp"
#include "gameObject.hpp"
#include "..\util.hpp"

#define PLAYER_WIDTH 64
#define PLAYER_HEIGHT 128

#define PLAYER_SPEED 500
#define PLAYER_JUMP_VEL 1600

// TODO: CHANGE THIS TO AN ENUM
#define PLAYER_RIGHT (0)
#define PLAYER_LEFT  (1)

class Player : public GameObject {

public:

	Player();
	Player(int x, int y);
	~Player();

	// accessor methods
	int getX() const;
	int getY() const;
	bool facingRight() const;
	bool onGround() const;

	void render();
	void update(Uint32 delta);

private:

	// just a pointer to the texture, not owned by the class
	AnimatedTexture * texture;

	int x, y;
	Math::Rectangle collision;

	float y_vel;
	bool on_ground, jumping;
	bool face_right;
	bool charging;
	Timer charge_timer;

	// -------------------------------------------------------------------------------
	// HELPER FUNCTIONS
	// -------------------------------------------------------------------------------
	void init();
	void handleKeyStates(Uint32 delta);
	void move(Direction dir, int distance);

};