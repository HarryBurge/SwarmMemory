#include "AgentMemory.h"
#include <iostream>

AgentMemory::AgentMemory(void) {
	pri_mem_pc = 0;
	pub_mem_pc = 0;
}

bool AgentMemory::push_pri_mem(Data dt) {
	if (pri_mem_pc == sizeof(pri_mem) - 1) {
		return false;
	}
	pri_mem[pri_mem_pc] = dt;
	pri_mem_pc++;
	return true;
}

Data AgentMemory::pull_pri_mem() {
	if (pri_mem_pc == 0) {
		return false;
	}
	pri_mem_pc--;
	return pri_mem[pri_mem_pc];
}

string AgentMemory::to_string() {
	string line = "";
	line += "Pri\tPub\n";

	for (int i = 0; i < max(pri_mem_pc, pub_mem_pc); i++) {
		line += pri_mem[i].to_string() + "\t" + pub_mem[i].to_string() + "\n";
	}

	return line;
}