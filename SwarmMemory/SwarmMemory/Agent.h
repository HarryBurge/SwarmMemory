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
	Circle body;
	Circle conn_area;
	vector<pair<Coord, Coord>> conns;
	
	Agent();
	Agent(int, float, float, float);

	void step(vector<Agent*>);
	bool recieved(Packet);

	string to_string();

private:
	float id;
	float facing;

	AgentMemory* mem;

	void move(float, float);
	void broadcast(vector<Agent*>, Packet);
};

#endif
