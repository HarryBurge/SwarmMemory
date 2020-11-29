#include "Agent.h"

using namespace std;

Agent::Agent() {

}

Agent::Agent(int aid, float ax, float ay, float afacing) {
	id = aid;
	body = Circle(ax, ay, 0.01);
	conn_area = Circle(ax, ay, 0.2);

	facing = afacing;

	mem = AgentMemory();
};

void Agent::step() {
	move(0, 0.0001);
}

void Agent::move(float dangle, float speed) {
	facing += dangle;

	float dx = cos(facing) * speed;
	float dy = sin(facing) * speed;

	body.translate(dx, dy);
	conn_area.translate(dx, dy);
}