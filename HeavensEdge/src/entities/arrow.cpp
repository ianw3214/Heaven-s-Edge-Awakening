#include "arrow.hpp"
#include "../game.hpp"

Arrow::Arrow(int x, int y, bool r, int h, float y_vel) : x(x), y(y), right(r), h_velocity(h), y_vel(y_vel), GameObject(ARROW_OBJECT) {
	collision = Math::Line(x, y, x, y);
	stopped = false;
	angle = r ? 0.0 : 180.0;
}

Arrow::~Arrow() {

}

void Arrow::render() {
	QcEngine::getTexture(ARROW_TEXTURE_ID)->setAngle(angle);
	QcEngine::getTexture(ARROW_TEXTURE_ID)->render(x - data->cam_x, y - data->cam_y);
	// render the particles (ASSUME particle frames are in order so we always remove from the end)
	Particle * temp = head;
	Particle * prev = nullptr;
	while (temp != nullptr) {
		if (temp->frame > 2) {
			if (prev) {
				prev->next = nullptr;
				delete temp;
			}
			else {
				head = nullptr;
				delete temp;
			}
			break;
		}
		AnimatedTexture * t = static_cast<AnimatedTexture*>(QcEngine::getTexture(ARROW_P));
		t->render(temp->x - data->cam_x, temp->y - data->cam_y, temp->frame);
		temp->frame++;
		temp = temp->next;
	}
}

void Arrow::update(Uint32 delta) {
	if (!stopped) {
		// add a new particle to the arrow
		Particle * part = new Particle();
		// account for offset of particle sprite and height of arrow
		part->x = x - 8;
		part->y = y - 8 - (right ? -8 : 8);
		part->frame = 0;
		part->next = head;
		head = part;
		// check if the arrow is colliding with anything before moving it
		if (data->collidingWithTiles(collision, 2, true)) {
			timer.reset(true);
			stopped = true;
			return;
		}
		// calculate a new position based on current arrow state
		int distance = (int)(h_velocity * delta / 1000.f);
		if (right) {
			x += distance;
		} else {
			x -= distance;
		}
		// calculate new arrow y position
		y_vel += static_cast<float>(delta / 1000.f * GRAVITY);
		int y_move = static_cast<int>(y_vel * delta / 1000.f);
		y += y_move;
		// calculate angle
		double new_angle = std::atan2((double)(y_move), (double)(right ? distance : -distance));
		angle = static_cast<int>(new_angle * 180 / PI);
		// calculate a collision line based on the angle
		collision.pos = Vec2(x, y);
		collision.pos2 = Vec2(x, y);
		collision.pos2.x += (int)(ARROW_WIDTH * std::cos(new_angle));
		collision.pos2.y += (int)(ARROW_WIDTH * std::sin(new_angle));
	} else {
		if (timer.getTicks() > ARROW_EXPIRE) {
			remove = true;
		}
	}
}