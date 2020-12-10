#include <string>
#include "Util.cpp"
#pragma once
class Data
{
public:
	int id;
	int data;
	int replication_num;
	int creator_id;

	Coord target_area;

	Data();
	Data(int, int, int, int, Coord);

	std::string to_string();
};

