#pragma once

#include <GL/gl.h>
#include "Math.hpp"

namespace physics {

struct Body {
	Vec2 position;
	Vec2 velocity;
	Vec2 force;
	Vec2 width;
	Vec2 terminalVelocity;
	float rotation;
	float angularVelocity;
	float torque;
	float friction;
	float mass, invMass;
	float I, invI;
	bool canJump;

	Body();
	void set(const Vec2& w, float m);
	void addForce(const Vec2& f);
	void draw();
	void setMass(float m);
};

}
