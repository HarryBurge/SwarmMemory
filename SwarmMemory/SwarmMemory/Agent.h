#ifndef AGENT_H
#define AGENT_H

#include <vector>

#include "AgentMemory.h"
#include "Packet.h"
#include "Util.cpp"
using namespace std;

/* Used to work out bounds for the memory threshold */
const float max_dupes = 6;
const float distance_from_point_where_1 = 0.7;
const float steper = distance_from_point_where_1 / max_dupes;

class Agent
{
public:
	float id;

	Circle body;
	Circle conn_area;
	vector<pair<int, pair<Coord, Coord>>> conns;

	AgentMemory* mem;
	
	Agent();
	Agent(int, float, float, float);

	void step(vector<Agent*>);
	Packet recieved(Packet);

	string to_string();

private:
	float facing;

	void move(float, float);
	vector<Packet> message(vector<Agent*> swarm, Packet packet);
};

#endif
