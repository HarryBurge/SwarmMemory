#include "AgentMemory.h"
#include <boost/optional.hpp>

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

bool AgentMemory::push_radio_mem(Packet dt) {
	if (radio_buffer.size() >= radio_max_size) {
		return false;
	}
	else {
		radio_buffer.push_back(dt);
		return true;
	}
}

Packet AgentMemory::pop_radio() {
	if (radio_buffer.size() == 0) {
		return Packet(0);
	}
	else {
		Packet temp = radio_buffer.front();
		radio_buffer.pop_front();
		return temp;
	}
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