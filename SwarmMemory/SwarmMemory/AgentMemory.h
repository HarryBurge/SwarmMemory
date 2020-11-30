#ifndef AGENTMEM_H
#define AGENTMEM_H

#include <vector>
#include "Data.h"
using namespace std;

class AgentMemory {
	public:
		AgentMemory(void);
		bool push_pri_mem(Data);
		Data pull_pri_mem();
		string to_string();
	private:
		Data pri_mem[10];
		Data pub_mem[10];
		int pri_mem_pc, pub_mem_pc;
};

#endif
