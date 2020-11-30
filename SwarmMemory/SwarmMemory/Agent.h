#ifndef AGENT_H
#define AGENT_H

#include "AgentMemory.h"
#include "Util.cpp"
#include "Data.h"

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

		void step(Agent*[], int);
		void move(float, float);
		void recieve(Data);
		void send(Agent* [], int, Data);

		std::string to_string();
};

#endif
