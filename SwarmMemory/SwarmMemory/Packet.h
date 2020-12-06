#ifndef PACKET_H
#define PACKET_H

#include <string>
class Packet
{
public:
	std::string data;
	int type;

	Packet();
	Packet(int);
};

#endif
