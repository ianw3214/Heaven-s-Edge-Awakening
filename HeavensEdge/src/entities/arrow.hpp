#pragma once

#include <queue>

#include "QcEngine.hpp"
#include "gameObject.hpp"
#include "../util.hpp"

#define ARROW_WIDTH 44.f
#define ARROW_EXPIRE 5000

#define ARROW_TEXTURE_ID "arrow"

struct Particle {
	int x;
	int y;
	int frame;
	Particle * next;
};

class Arrow : public GameObject {

public:

	Arrow(int x, int y, bool right, int h_velocity, float y_vel);
	~Arrow();

	void render();
	void update(Uint32 delta);

private:

	// the x and y represent the tip of the arrow
	int x, y;
	bool right;
	int h_velocity;
	float y_vel;
	Math::Line collision;
	double angle;
	bool stopped;
	// timer to keep track of when the arrow should dissapear after stopping
	Timer timer;

	// linked list of arrow particles
	Particle * head;

};