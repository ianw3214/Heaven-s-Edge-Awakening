#pragma once

#include "QcEngine.hpp"

// -------------------------------------------------------------------------------
// Enumeration for game to pass around entity type information when needed
// -------------------------------------------------------------------------------
enum EntityType {
	E_PLAYER,
	E_ARROW,
	E_ENEMY
};

// -------------------------------------------------------------------------------
// Struct to represent the data of the player
// -------------------------------------------------------------------------------
struct Player {
	// movement variables
	int x, y;
	float y_vel;
	bool on_ground, jumping;
	bool face_right;
	// player state variables
	int health;
};

enum PlayerAnimation {
	PLAYER_RIGHT = 0,
	PLAYER_LEFT = 1
};

// -------------------------------------------------------------------------------
// Struct to represent the data of each arrow the player shoots
// -------------------------------------------------------------------------------
struct Arrow {
	// the x and y represent the tip of the arrow
	int x, y;
	bool right;
	int h_velocity;
	float y_vel;
	Math::Line collision;
	int angle;
	bool stopped;
	// timer to keep track of when the arrow should dissapear after stopping
	Timer timer;
};

// -------------------------------------------------------------------------------
// Struct to represent a basic enemy
// -------------------------------------------------------------------------------
struct Enemy {
	// movement variables
	int x, y;
	float y_vel;
	bool on_ground;
	// enemy state variables
	int health;
};