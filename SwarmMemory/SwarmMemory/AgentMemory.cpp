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
	for (int i = 0; i<pub_mem.size(); i++) {
		if (pub_mem[i].replication_num > 0) {
			return i;
		}
	}
	return -1;
}


bool AgentMemory::space_in_pri() {
	if (pri_mem.size() < pri_max_size) {
		return true;
	}
	return false;
}

bool AgentMemory::space_in_pub() {
	if (pub_mem.size() < pub_max_size) {
		return true;
	}
	return false;
}


bool AgentMemory::pub_has_data_id(int did) {
	for (int i = 0; i < pub_mem.size(); i++) {
		if (pub_mem[i].id == did) {
			return true;
		}
	}
	return false;
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