#include "Data.h"

Data::Data() {
	creator_id = -1;
	replication_num = 0;
	data = 0;
	locked = false;
}

Data::Data(int cid, int rep_num, int dt) {
	creator_id = cid;
	replication_num = rep_num;
	data = dt;
	locked = false;
}

std::string Data::to_string() {
	std::string line = std::to_string(data);
	return line;
}