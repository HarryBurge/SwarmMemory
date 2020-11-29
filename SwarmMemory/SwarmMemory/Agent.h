#ifndef AGENT_H
#define AGENT_H

#include "AgentMemory.h"
#include "Util.cpp"

class Agent
{
	public:
		float id;
		Circle body;
		Circle conn_area;

		AgentMemory mem;

		float facing;

		Agent();
		Agent(int, float, float, float);

		void step();
		void move(float, float);
};

#endif
