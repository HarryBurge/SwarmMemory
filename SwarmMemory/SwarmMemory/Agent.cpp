#include "Agent.h"
const double pi = 2 * acos(0.0);

Agent::Agent() {
}

Agent::Agent(int aid, float ax, float ay, float afacing) {
	id = aid;
	facing = afacing;

	body = Circle(ax, ay, 0.01);
	conn_area = Circle(ax, ay, 0.2);

	mem = new AgentMemory();
};

void Agent::step(vector<Agent*> swarm) {
	move(0.001, 0.0001 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.0004 - 0.0001))));
	//message(swarm, Packet(1, id, -1));

	if (mem->get_pri_index_rep_more_than() != -1) {
		// Replicate
		vector<Packet> collect = message(swarm, Packet(mem->pri_mem[mem->get_pri_index_rep_more_than()], 2, id, -1));

		bool all_failed = true;

		for (int i = 0; i < collect.size(); i++) {
			if (collect[0].type != 0) {
				all_failed = false;
				mem->pri_mem[mem->get_pri_index_rep_more_than()].replication_num--;
				break;
			}
		}
	}

	if (mem->get_pub_index_rep_more_than() != -1) {
		//Replicate
		vector<Packet> collect = message(swarm, Packet(mem->pub_mem[mem->get_pub_index_rep_more_than()], 2, id, -1));

		bool all_failed = true;

		for (int i = 0; i < collect.size(); i++) {
			if (collect[0].type != 0) {
				all_failed = false;
				mem->pub_mem[mem->get_pub_index_rep_more_than()].replication_num--;
				break;
			}
		}
	}

	//TODO: Continue
	// Suidcide thing
	// Optimise thing
}

void Agent::move(float dangle, float speed) {
	facing += dangle;

	if (facing > 2 * 2*pi) {
		facing - 2 * pi;
	}
	else if (facing < 0) {
		facing + 2 * pi;
	}

	float dx = cos(facing) * speed;
	float dy = sin(facing) * speed;

	body.translate(dx, dy);
	conn_area.translate(dx, dy);
}

Packet Agent::recieved(Packet packet) {
	if (packet.senderid != id && (packet.recieverid == id || packet.recieverid == -1)) {

		if (packet.type == 1) {
			return Packet(1, packet.recieverid, packet.senderid);
		}

		if (packet.type == 2 && mem->space_in_pub() && packet.data.creator_id != id && !mem->pub_has_data_id(packet.data.id)) {
			packet.data.replication_num--;
			mem->push_pub_mem(packet.data);
			return Packet(1, packet.recieverid, packet.senderid);
		}

	}
	return Packet(0, packet.recieverid, packet.senderid);
}

//void Agent::broadcast(vector<Agent*> swarm, Packet packet) {
//	for (int i = 0; i < swarm.size(); i++) {
//		if (swarm[i]->id != id && conn_area.point_in_circle(swarm[i]->body.center)) {
//			if (swarm[i]->recieved(packet).type!=0) {
//				conns.push_back(pair<Coord,Coord>(body.center, swarm[i]->body.center));
//			}
//		}
//	}
//}

vector<Packet> Agent::message(vector<Agent*> swarm, Packet packet) {
	vector<Packet> collect;

	for (int i = 0; i < swarm.size(); i++) {
		if (swarm[i]->id != id && conn_area.point_in_circle(swarm[i]->body.center)) {
			Packet temp = swarm[i]->recieved(packet);

			if (temp.type != 0) {
				conns.push_back(pair<Coord, Coord>(body.center, swarm[i]->body.center));
				collect.push_back(temp);
			}
		}
	}

	return collect;
}

std::string Agent::to_string() {
	string line = "Agent " + std::to_string(id) + " X = " + std::to_string(body.center.x) + " Y = " + std::to_string(body.center.y) + "\n";
	line += mem->to_string();
	return line;
}