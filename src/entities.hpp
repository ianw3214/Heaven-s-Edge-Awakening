#pragma once

#include "QcEngine.hpp"

// struct to represent the data of the player
struct Player {
	int x, y;
	int y_accel;
	bool on_ground, jumping;
	bool face_right;
};

enum PlayerAnimation {
	PLAYER_RIGHT = 0,
	PLAYER_LEFT = 1
};

// struct to represent the data of each arrow the player shoots
struct Arrow {
	// the x and y represent the tip of the arrow
	int x, y;
	bool right;
	int h_velocity;
	int y_accel;
	Math::Line collision;
	int angle;
	bool stopped;
};