#ifndef AGENT_H
#define AGENT_H

#include <vector>

#include "AgentMemory.h"
#include "Packet.h"
#include "Util.cpp"
using namespace std;

class Agent
{
public:
	float id;

	Circle body;
	Circle conn_area;
	vector<pair<Coord, Coord>> conns;

	AgentMemory* mem;
	
	Agent();
	Agent(int, float, float, float);

	void step(vector<Agent*>);
	Packet recieved(Packet);

	string to_string();

private:
	float facing;

	void move(float, float);
	/*void broadcast(vector<Agent*>, Packet);*/
	vector<Packet> message(vector<Agent*> swarm, Packet packet);
};

#endif
