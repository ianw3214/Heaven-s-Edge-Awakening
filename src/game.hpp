#pragma once

#define TILE_SIZE 64

#define MAP_WIDTH 15
#define MAP_HEIGHT 10

#define GRAVITY 100
#define SPEED_CAP 100

#define PLAYER_SPEED 500

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define CAMERA_SPEED 200
#define CAMERA_MARGIN 200

#include "QcEngine.hpp"
#include "entities.hpp"

enum Direction {
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3
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
	Texture * arrow_texture;
	AnimatedTexture * player_texture;

	// entities
	Player player;
	std::vector<Arrow> arrows;

	// game state data
	int cam_x;
	int cam_y;

	std::vector<int> tilemap;
	std::vector<bool> collisionmap;

	// helper functions
	void handleKeyPresses();
	void updatePlayer();
	void updateCamera();
	void updateArrows();
	void movePlayer(Direction dir, int distance);
	bool playerColliding(int x, int y) const;
	bool arrowColliding(Arrow& a) const;

};