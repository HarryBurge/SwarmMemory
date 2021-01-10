#include "Packet.h"

Packet::Packet() {
	senderid = -1;
	recieverid = -1;
}

Packet::Packet(int t) {
	type = t;
	senderid = -1;
	recieverid = -1;
}

Packet::Packet(int t, int sid, int rid) {
	type = t;
	senderid = sid;
	recieverid = rid;
}

Packet::Packet(int t, int sid, int rid, int add) {
	type = t;
	senderid = sid;
	recieverid = rid;
	additional = add;
}

Packet::Packet(Data dt, int t, int sid, int rid) {
	data = dt;
	type = t;
	senderid = sid;
	recieverid = rid;
}