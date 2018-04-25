#pragma once

#include <vector>
#include <list>

#include "QcEngine.hpp"
#include "entities.hpp"

#define TILE_SIZE 64

#define MAP_WIDTH 15
#define MAP_HEIGHT 10

#define GRAVITY 3000
#define SPEED_CAP 2000

#define PLAYER_SPEED 500
#define PLAYER_JUMP_VEL -1200.f

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define CAMERA_SPEED 200
#define CAMERA_MARGIN 200

#define MAX_BOW_CHARGE 1000
#define BASE_ARROW_SPEED 500
#define ARROW_SPEED 1500
#define ARROW_WIDTH 48.f
#define ARROW_EXPIRE 10000

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
	Texture * enemy_texture;
	AnimatedTexture * player_texture;

	// -------------------------------------------------------------------------------
	// ENTITIES
	// -------------------------------------------------------------------------------
	Player player;
	// use a list for arrows because arrows in the middle might have to be deleted
	std::list<Arrow> arrows;
	std::list<Enemy> enemies;

	// -------------------------------------------------------------------------------
	// GAME STATE DATA
	// -------------------------------------------------------------------------------
	int cam_x;
	int cam_y;
	bool charging;
	Timer charge_timer;

	// -------------------------------------------------------------------------------
	// MAP DATA
	// -------------------------------------------------------------------------------
	std::vector<int> tilemap;
	std::vector<bool> collisionmap;

	// -------------------------------------------------------------------------------
	// HELPER FUNCTIONS
	// -------------------------------------------------------------------------------
	void handleKeyPresses();
	void updatePlayer();
	void updateCamera();
	void updateArrows();
	void updateEnemies();
	void moveEntity(Direction dir, int distance, Math::Shape& collision, EntityType type, int& x, int& y);
	bool entityColliding(int x, int y, Math::Shape& collision, EntityType type);
	bool collidingWithTile(Math::Shape& shape) const;
	bool collidingWithEnemy(Math::Shape& shape) const;
	bool collidingWithPlayer(Math::Shape& shape) const;

};