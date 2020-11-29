#ifndef AGENTMEM_H
#define AGENTMEM_H

#include <vector>
using namespace std;

class AgentMemory {
	public:
		AgentMemory(void);
	private:
		vector<char> pri_mem;
		vector<char> pub_mem;
};

#endif
