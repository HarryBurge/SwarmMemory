#ifndef AGENTMEM_H
#define AGENTMEM_H

#include <vector>
#include <mutex>

#include "Data.h"
using namespace std;

const int pri_max_size = 10;
const int pub_max_size = 10;

class AgentMemory {

public:
	vector<Data> pri_mem;
	vector<Data> pub_mem;

	std::mutex myMutex;

	AgentMemory();

	bool push_pri_mem(Data);
	bool push_pub_mem(Data);

	bool space_in_pri();
	bool space_in_pub();

	bool pub_has_data_id(int);
	bool pri_has_data_id(int did);

	void remove_pub(int);

	string to_string();
};

#endif
