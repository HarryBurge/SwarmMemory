#include "AgentMemory.h"

AgentMemory::AgentMemory() {
}

bool AgentMemory::push_pri_mem(Data dt) {
	if (pri_mem.size() >= pri_max_size) {
		return false;
	}
	else {
		pri_mem.push_back(dt);
		return true;
	}
}

bool AgentMemory::push_pub_mem(Data dt) {
	if (pub_mem.size() >= pub_max_size) {
		return false;
	}
	else {
		pub_mem.push_back(dt);
		return true;
	}
}


int AgentMemory::get_pri_index_rep_more_than() {
	for (int i = 0; i < pri_mem.size(); i++) {
		if (pri_mem[i].replication_num > 0) {
			return i;
		}
	}
	return -1;
}

int AgentMemory::get_pub_index_rep_more_than() {
	for (int i = 0; pub_mem.size(); i++) {
		if (pub_mem[i].replication_num > 0) {
			return i;
		}
	}
	return -1;
}


Data AgentMemory::get_pri_index(int ind) {
	return pri_mem[ind];
}

Data AgentMemory::get_pub_index(int ind) {
	return pub_mem[ind];
}


void AgentMemory::set_pri_index(int ind, Data dt) {
	pri_mem[ind] = dt;
}

void AgentMemory::set_pub_index(int ind, Data dt) {
	pub_mem[ind] = dt;
}

string AgentMemory::to_string() {
	string line = "Pri Mem = [";

	for (int i = 0; i < pri_mem.size(); i++) {
		line += pri_mem[i].to_string() + ",";
	}

	line += "]\n Pub Mem = [";

	for (int i = 0; i < pub_mem.size(); i++) {
		line += pub_mem[i].to_string() + ",";
	}

	line += "]";

	return line;
}