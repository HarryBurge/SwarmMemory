#include "Agent.h"
const double pi = 2 * acos(0.0);
const float threashold = 3;

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
	//move(0.01, 0.001);
	//message(swarm, Packet(1, id, -1));

	// Replicate private memory item
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

	// Replicate public memory item
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
	
	// Suicide
	for (int i = 0; i < mem->pub_mem.size(); i++) {
		// Count how many repliceres around
		vector<Packet> collect = message(swarm, Packet(mem->pub_mem[i], 3, id, -1));

		int counter = 0;

		for (int j = 0; j < collect.size(); j++) {
			if (collect[0].type != 0) {
				counter++;
			}
		}

		// if not locked then check heruristic
		float heuristic = counter * (1 / body.center.distance(mem->pub_mem[i].target_area));

		// If above threshold then suicide
		if (heuristic >= threashold) {
			mem->pub_mem.erase(mem->pub_mem.begin() + i);
			conns.push_back(pair<int, pair<Coord, Coord>>(3, pair<Coord, Coord>(body.center, Coord(0, 0))));
		}
	}

	// TODO: Here

	// Sucide complete heuristic stuff

	// If above threshold then do

	// Optimise
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

		if (packet.type == 3 && mem->pub_has_data_id(packet.data.id)) {
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
				collect.push_back(temp);
			}

			if (packet.type == 2 && temp.type == 1) {
				conns.push_back(pair<int, pair<Coord, Coord>>(2, pair<Coord, Coord>(body.center, swarm[i]->body.center)));
			}
			else if (packet.type == 2 && temp.type == 0) {
				conns.push_back(pair<int, pair<Coord, Coord>>(1, pair<Coord, Coord>(body.center, swarm[i]->body.center)));
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