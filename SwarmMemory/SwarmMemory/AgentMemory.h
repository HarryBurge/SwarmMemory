#ifndef AGENTMEM_H
#define AGENTMEM_H

#include <vector>
#include <deque>

#include "Data.h"
#include "Packet.h"
using namespace std;

class AgentMemory {

public:
	AgentMemory();

	bool push_pri_mem(Data);
	bool push_pub_mem(Data);
	bool push_radio_mem(Packet);

	Packet pop_radio();

	string to_string();

private:
	vector<Data> pri_mem;
	vector<Data> pub_mem;
	deque<Packet> radio_buffer;

	const int pri_max_size = 10;
	const int pub_max_size = 10;
	const int radio_max_size = 10;
};

#endif
