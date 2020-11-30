#include "Data.h"

Data::Data() {
	data = 0;
}

Data::Data(int dt) {
	data = dt;
}

std::string Data::to_string() {
	std::string line = std::to_string(data);
	return line;
}