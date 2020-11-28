// SwarmMemory.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AgentMemory.h"
using namespace std;

int main() {
	AgentMemory tom = AgentMemory(2);
	cout << tom.get_id() << endl;
};


