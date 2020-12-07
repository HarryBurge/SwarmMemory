#ifndef AGENTMEM_H
#define AGENTMEM_H

#include <vector>

#include "Data.h"
using namespace std;

const int pri_max_size = 10;
const int pub_max_size = 10;

class AgentMemory {

public:
	vector<Data> pri_mem;
	vector<Data> pub_mem;

	AgentMemory();

	bool push_pri_mem(Data);
	bool push_pub_mem(Data);

	int get_pri_index_rep_more_than();
	int get_pub_index_rep_more_than();

	bool space_in_pri();
	bool space_in_pub();

	bool pub_has_data_id(int);

	string to_string();
};

#endif
