#include "Data.h"

Data::Data() {
	id = -1;
	creator_id = -1;
	replication_num = 0;
	data = 0;
	locked = false;
}

Data::Data(int did, int cid, int rep_num, int dt) {
	id = did;
	creator_id = cid;
	replication_num = rep_num;
	data = dt;
	locked = false;
}

std::string Data::to_string() {
	std::string line = "{id=" + std::to_string(id) + ",cid=" + std::to_string(creator_id) + ",rep_num=" + std::to_string(replication_num);
	line += ",data=" + std::to_string(data) + ",locked=" + std::to_string(locked) + "}";
	return line;
}