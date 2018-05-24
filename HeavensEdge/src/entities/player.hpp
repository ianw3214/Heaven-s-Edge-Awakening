#pragma once

#include "QcEngine.hpp"
#include "gameObject.hpp"
#include "..\util.hpp"

#define PLAYER_WIDTH 64
#define PLAYER_HEIGHT 128

#define PLAYER_X_MARGIN 32

#define PLAYER_SPEED 500
#define PLAYER_JUMP_VEL 1600

// TODO: CHANGE THIS TO AN ENUM
#define PLAYER_RIGHT_IDLE	(0)
#define PLAYER_LEFT_IDLE	(1)
#define PLAYER_RIGHT_RUN	(2)
#define PLAYER_LEFT_RUN		(3)
#define PLAYER_RIGHT_JUMP	(4)
#define PLAYER_LEFT_JUMP	(5)
#define PLAYER_RIGHT_AIR	(6)
#define PLAYER_LEFT_AIR		(7)

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
	const Math::Rectangle& getCollision() const;

	void render();
	void update(Uint32 delta);

private:

	// just a pointer to the texture, not owned by the class
	AnimatedTexture * texture;
	int current_animation;

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