#pragma once

#define TILE_SIZE 64

#define MAP_WIDTH 10
#define MAP_HEIGHT 10

#define GRAVITY 100
#define SPEED_CAP 100

#define PLAYER_SPEED 500

#include "QcEngine.hpp"

// struct to represent the data of the player
struct Player {
	int x, y;
	int y_accel;
};

class Game : public State {
public:
	Game();
	~Game();

	void init();
	void cleanup();
	void pause();
	void resume();
	void update();
	void render();

private:
	Texture * tile_texture;
	Texture * player_texture;

	Player player_data;

	std::vector<int> tilemap;
	std::vector<bool> collisionmap;

	// helper function to check if player is colliding with a tile
	bool playerColliding(int x, int y) const;
};