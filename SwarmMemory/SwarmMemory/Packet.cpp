#include "Packet.h"

Packet::Packet() {
	data = "   ";
}

Packet::Packet(int dt) {
	data = "    ";
	type = dt;
}