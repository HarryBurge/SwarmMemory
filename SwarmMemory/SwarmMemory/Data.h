#include <string>
#pragma once
class Data
{
public:
	int data;
	int replication_num;
	int creator_id;
	bool locked;

	Data();
	Data(int, int, int);

	std::string to_string();
};

