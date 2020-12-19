#include "Data.h"

Data::Data() {
	id = -1;
	creator_id = -1;
	data = 0;
}

Data::Data(int did, int cid, int dt, Coord point) {
	id = did;
	creator_id = cid;
	data = dt;

	target_area = point;
}

std::string Data::to_string() {
	std::string line = "{id=" + std::to_string(id) + ",cid=" + std::to_string(creator_id);
	line += ",data=" + std::to_string(data) + "}";
	return line;
}