#include "../../include/physics/World.hpp"
#include <cstdio>

namespace physics {

using std::map;
using std::pair;

typedef pair<ArbiterKey, Arbiter> ArbPair;
typedef map<ArbiterKey, Arbiter>::iterator ArbIter;

bool World::accumulateImpulses = true;
bool World::warmStarting = true;
bool World::positionCorrection = true;

World::World() {}

void World::add(Body* body) {
	bodies.emplace_back(body);
}

void World::add(Joint* joint) {
	joints.emplace_back(joint);
}

void World::clear() {
	bodies.clear();
	joints.clear();
	arbiters.clear();
}

void World::broadPhase() {
	// O(n^2) broad-phase
	for (int i = 0; i < (int)bodies.size(); ++i) {
		Body* bi = bodies[i];

		for (int j = i + 1; j < (int)bodies.size(); ++j) {

			Body* bj = bodies[j];

			if (bi->invMass == 0.0f && bj->invMass == 0.0f) {
				continue;
			}

			Arbiter newArb(bi, bj);
			ArbiterKey key(bi, bj);

			if (newArb.numContacts > 0) {
				printf("broadPhase <==> i: %d; j: %d\n", i, j);
				bi->inTouch = true;
				bj->inTouch = true;
				ArbIter iter = arbiters.find(key);
				if (iter == arbiters.end()) {
					arbiters.insert(ArbPair(key, newArb));
				} else {
					iter->second.update(newArb.contacts, newArb.numContacts);
				}
			} else {
				bi->inTouch = false;
				bj->inTouch = false;
				arbiters.erase(key);
			}
		}
	}
}

void World::step(float dt) {
	float invDt = dt > 0.0f ? 1.0f / dt : 0.0f;

	broadPhase();

	for (auto& body : bodies) {
		if (body->invMass == 0.0f) {
			continue;
		}

		body->velocity += dt * (gravity + body->invMass * body->force);
		body->angularVelocity += dt * body->invI * body->torque;
	}

	for (int i = 0; i < (int) bodies.size(); i++) {
		std::cout << "body " << i << ": " << bodies[i]->inTouch << std::endl;
	}

	for (auto& arb : arbiters) {
		arb.second.preStep(invDt);
	}

	for (auto& joint : joints) {
		joint->preStep(invDt);
	}

	for (int i = 0; i < iterations; ++i) {
		for (auto& arb : arbiters) {
			arb.second.applyImpulse();
		}

		for (auto& joint : joints) {
			joint->applyImpulse();
		}
	}

	for (auto& body : bodies) {
		body->position += dt * body->velocity;
		body->rotation += dt * body->angularVelocity;

		body->force.set(0.0f, 0.0f);
		body->torque = 0.0f;
	}
}

}
