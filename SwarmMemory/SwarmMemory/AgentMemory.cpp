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
	myMutex.lock();

	if (pub_mem.size() >= pub_max_size) {
		myMutex.unlock();
		return false;
	}
	else {
		pub_mem.push_back(dt);
		myMutex.unlock();
		return true;
	}
}


bool AgentMemory::space_in_pri() {
	if (pri_mem.size() < pri_max_size) {
		return true;
	}
	return false;
}

bool AgentMemory::space_in_pub() {
	myMutex.lock();

	if (pub_mem.size() < pub_max_size) {
		myMutex.unlock();
		return true;
	}
	myMutex.unlock();
	return false;
}


bool AgentMemory::pub_has_data_id(int did) {
	myMutex.lock();

	for (int i = 0; i < pub_mem.size(); i++) {
		if (pub_mem[i].id == did) {
			myMutex.unlock();
			return true;
		}
	}
	myMutex.unlock();
	return false;
}

bool AgentMemory::pri_has_data_id(int did) {
	myMutex.lock();

	for (int i = 0; i < pri_mem.size(); i++) {
		if (pri_mem[i].id == did) {
			myMutex.unlock();
			return true;
		}
	}
	myMutex.unlock();
	return false;
}

void AgentMemory::remove_pub(int index) {
	myMutex.lock();
	pub_mem.erase(pub_mem.begin() + index);
	myMutex.unlock();
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