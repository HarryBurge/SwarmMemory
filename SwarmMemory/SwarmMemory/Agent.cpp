#include "Agent.h"
#include <iostream>

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

void Agent::step(Agent* swarm[], int swarmsize) {
}

void Agent::move(float dangle, float speed) {
	facing += dangle;

	float dx = cos(facing) * speed;
	float dy = sin(facing) * speed;

	body.translate(dx, dy);
	conn_area.translate(dx, dy);
}

void Agent::recieve(Data packet) {
	cout << "Agent " << id << " Recieved packet = " << packet.data << endl;
}

void Agent::send(Agent* swarm[], int swarmsize, Data packet) {
	for (int i = 0; i < swarmsize; i++) {
		if (swarm[i]->id != id && conn_area.point_in_circle(swarm[i]->body.center)) {
			swarm[i]->recieve(packet);
		}
	}
}


std::string Agent::to_string() {
	string line = "Agent " + std::to_string(id) + " X = " + std::to_string(body.center.x) + " Y = " + std::to_string(body.center.y) + "\n";
	line += mem.to_string();
	return line;
}