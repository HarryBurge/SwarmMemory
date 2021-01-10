#ifndef PACKET_H
#define PACKET_H

#include "Data.h"

/* 
Packet types

0 - False
1 - True
2 - Replicate data
3 - Have data
4 - If don't have data how much free space

*/
class Packet
{
public:
	Data data;
	int type;
	int senderid;
	int recieverid;
	int additional;

	Packet();
	Packet(int);
	Packet(int, int, int);
	Packet(int, int, int, int);
	Packet(Data, int, int, int);
};

#endif
