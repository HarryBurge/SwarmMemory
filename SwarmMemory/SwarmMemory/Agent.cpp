#include "Agent.h"
const double pi = 2 * acos(0.0);

Agent::Agent() {
}

Agent::Agent(int aid, float ax, float ay, float afacing) {
	id = aid;
	facing = afacing;

	body = Circle(ax, ay, 0.01);
	conn_area = Circle(ax, ay, 0.2);

	mem = new AgentMemory();
};

void Agent::step(vector<Agent*> swarm) {
	broadcast(swarm, Packet(2));
	for (int i = 0; i < 11; i++) {
		mem->pop_radio();
	}
	move(0.001, 0.0001);
}

void Agent::move(float dangle, float speed) {
	facing += dangle;

	if (facing > 2 * 2*pi) {
		facing - 2 * pi;
	}
	else if (facing < 0) {
		facing + 2 * pi;
	}

	float dx = cos(facing) * speed;
	float dy = sin(facing) * speed;

	body.translate(dx, dy);
	conn_area.translate(dx, dy);
}

bool Agent::recieved(Packet packet) {
	if (mem->push_radio_mem(packet)) {
		return true;
	}
	return false;
}

void Agent::broadcast(vector<Agent*> swarm, Packet packet) {
	for (int i = 0; i < swarm.size(); i++) {
		if (swarm[i]->id != id && conn_area.point_in_circle(swarm[i]->body.center)) {
			if (swarm[i]->recieved(packet)) {
				conns.push_back(pair<Coord,Coord>(body.center, swarm[i]->body.center));
			}
		}
	}
}

std::string Agent::to_string() {
	string line = "Agent " + std::to_string(id) + " X = " + std::to_string(body.center.x) + " Y = " + std::to_string(body.center.y) + "\n";
	line += mem->to_string();
	return line;
}